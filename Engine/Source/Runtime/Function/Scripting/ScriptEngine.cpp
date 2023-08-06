#include "ScriptEngine.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/mono-debug.h"
#include "mono/metadata/threads.h"

#include <string>

#include "ScriptRegister.h"
#include "Runtime/Core/Tools/FileSystem/Buffer.h"
#include "Runtime/Core/Tools/FileSystem/FileSystem.h"

namespace LitchiRuntime
{
	static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap =
	{
		{ "System.Single", ScriptFieldType::Float },
		{ "System.Double", ScriptFieldType::Double },
		{ "System.Boolean", ScriptFieldType::Bool },
		{ "System.Char", ScriptFieldType::Char },
		{ "System.Int16", ScriptFieldType::Short },
		{ "System.Int32", ScriptFieldType::Int },
		{ "System.Int64", ScriptFieldType::Long },
		{ "System.Byte", ScriptFieldType::Byte },
		{ "System.UInt16", ScriptFieldType::UShort },
		{ "System.UInt32", ScriptFieldType::UInt },
		{ "System.UInt64", ScriptFieldType::ULong },

		{ "LitchiEngine.Vector2", ScriptFieldType::Vector2 },
		{ "LitchiEngine.Vector3", ScriptFieldType::Vector3 },
		{ "LitchiEngine.Vector4", ScriptFieldType::Vector4 },
	};

	namespace Utils {

		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath, bool loadPDB = false)
		{
			ScopedBuffer fileData = FileSystem::ReadFileBinary(assemblyPath);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData.As<char>(), fileData.Size(), 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				// Log some error message using the errorMessage data
				return nullptr;
			}

			if (loadPDB)
			{
				std::filesystem::path pdbPath = assemblyPath;
				pdbPath.replace_extension(".pdb");

				if (std::filesystem::exists(pdbPath))
				{
					ScopedBuffer pdbFileData = FileSystem::ReadFileBinary(pdbPath);
					mono_debug_open_image_from_memory(image, pdbFileData.As<const mono_byte>(), pdbFileData.Size());
					// DEBUG_LOG_INFO("Loaded PDB {}", pdbPath.c_str());
				}
			}

			std::string pathString = assemblyPath.string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);
			mono_image_close(image);

			return assembly;
		}

		void PrintAssemblyTypes(MonoAssembly* assembly)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
				// DEBUG_LOG_INFO("{}.{}", nameSpace, name);
			}
		}

		ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType)
		{
			std::string typeName = mono_type_get_name(monoType);

			auto it = s_ScriptFieldTypeMap.find(typeName);
			if (it == s_ScriptFieldTypeMap.end())
			{
				// DEBUG_LOG_INFO("Unknown type: {}", typeName);
				return ScriptFieldType::None;
			}

			return it->second;
		}

	}

	/**
	 * \brief 脚本引擎数据
	 */
	struct ScriptEngineData
	{
		/**
		 * \brief 根Domain
		 */
		MonoDomain* RootDomain = nullptr;
		/**
		 * \brief 应用Domain
		 */
		MonoDomain* AppDomain = nullptr;
		/**
		 * \brief 核心的程序集
		 */
		MonoAssembly* CoreAssembly = nullptr;
		/**
		 * \brief 核心程序集视图
		 */
		MonoImage* CoreAssemblyImage = nullptr;
		/**
		 * \brief 应用程序集
		 */
		MonoAssembly* AppAssembly = nullptr;
		/**
		 * \brief 应用程序集视图
		 */
		MonoImage* AppAssemblyImage = nullptr;
		/**
		 * \brief 核心程序集路径
		 */
		std::filesystem::path CoreAssemblyFilepath;
		/**
		 * \brief 应用程序集路径
		 */
		std::filesystem::path AppAssemblyFilepath;
		/**
		 * \brief ScriptObject的MonoClass定义
		 */
		ScriptClass EngineObjectClass;
		/**
		 * \brief ScriptObject的所有子类映射
		 */
		std::unordered_map<std::string, Ref<ScriptClass>> ScriptObjectClassDict;
		/**
		 * \brief ScriptObject的所有实例映射
		 */
		std::unordered_map<uint64_t, Ref<ScriptInstance>> ScriptObjectInstanceDict;
		/**
		 * \brief ScriptObject的所有实例字段映射
		 */
		std::unordered_map<uint64_t, ScriptFieldMap> ScriptObjectFieldDict;
		/**
		 * \brief 是否开启DEBUG
		 */
		bool EnableDebugging = false;
		/**
		 * \brief 当前的Scene
		 */
		Scene* SceneContext = nullptr;

		// Scope<filewatch::FileWatch<std::string>> AppAssemblyFileWatcher;
		bool AssemblyReloadPending = false;
	};
	static ScriptEngineData* s_data = nullptr;

	//static void OnAppAssemblyFileSystemEvent(const std::string& path, const filewatch::Event change_type)
	//{
	//	if (!s_Data->AssemblyReloadPending && change_type == filewatch::Event::modified)
	//	{
	//		s_Data->AssemblyReloadPending = true;

	//		Application::Get().SubmitToMainThread([]()
	//			{
	//				s_Data->AppAssemblyFileWatcher.reset();
	//				ScriptEngine::ReloadAssembly();
	//			});
	//	}
	//}

#if 1 ScriptClass

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore)
		: m_classNamespace(classNamespace), m_className(className)
	{
		m_monoClass = mono_class_from_name(isCore ? s_data->CoreAssemblyImage : s_data->AppAssemblyImage, classNamespace.c_str(), className.c_str());
	}

	MonoObject* ScriptClass::Instantiate()
	{
		return ScriptEngine::InstantiateClass(m_monoClass);
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount)
	{
		return mono_class_get_method_from_name(m_monoClass, name.c_str(), parameterCount);
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		MonoObject* exception = nullptr;
		return mono_runtime_invoke(method, instance, params, &exception);
	}

#endif

#if 1 ScriptInstance

	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, ScriptObject* object)
		: m_scriptClass(scriptClass)
	{
		m_instance = scriptClass->Instantiate();

		m_constructor = scriptClass->GetMethod(".ctor", 1);
		m_onCreateMethod = scriptClass->GetMethod("OnCreate", 0);
		m_onUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);

		// Call Entity constructor
		{
			uint64_t entityID = object->GetUnmanagedId();
			void* param = &entityID;
			m_scriptClass->InvokeMethod(m_instance, m_constructor, &param);
		}
	}

	void ScriptInstance::InvokeOnCreate()
	{
		if (m_onCreateMethod)
			m_scriptClass->InvokeMethod(m_instance, m_onCreateMethod);
	}

	void ScriptInstance::InvokeOnUpdate(float ts)
	{
		if (m_onUpdateMethod)
		{
			void* param = &ts;
			m_scriptClass->InvokeMethod(m_instance, m_onUpdateMethod, &param);
		}
	}

	bool ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer)
	{
		const auto& fields = m_scriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		const ScriptField& field = it->second;
		mono_field_get_value(m_instance, field.ClassField, buffer);
		return true;
	}

	bool ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value)
	{
		const auto& fields = m_scriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		const ScriptField& field = it->second;
		mono_field_set_value(m_instance, field.ClassField, (void*)value);
		return true;
	}

#endif

#if 1 ScriptEngine

	void ScriptEngine::Init()
	{
		s_data = new ScriptEngineData();

		// 初始化mono
		InitMono();

		// 注册方法
		ScriptRegister::RegisterFunctions();

		// 加载引擎核心程序集 路径写死
		std::string scriptCoreDllPath = "../../ScriptCore/Debug/LitchiScriptCore.dll";
		bool status = LoadCoreAssembly(scriptCoreDllPath);
		if(!status)
		{
			DEBUG_LOG_ERROR("[ScriptEngine] Could not load LitchiScriptCore assembly.");
			return;
		}

		// 加载项目的程序集
		std::filesystem::path scriptModulePath;
		status = LoadAppAssembly(scriptModulePath);
		if (!status)
		{
			DEBUG_LOG_ERROR("[ScriptEngine] Could not load app assembly.");
			return;
		}

		// 加载程序集的类型
		LoadAssemblyClasses();

		// 注册脚本
		ScriptRegister::RegisterComponents();

		s_data->EngineObjectClass = ScriptClass("LitchiEngine", "ScriptObject", true);
	}

	void ScriptEngine::Shutdown()
	{
	}

	bool ScriptEngine::LoadCoreAssembly(const std::filesystem::path& filepath)
	{
		// Create an App Domain
		s_data->AppDomain = mono_domain_create_appdomain("HazelScriptRuntime", nullptr);
		mono_domain_set(s_data->AppDomain, true);

		s_data->CoreAssemblyFilepath = filepath;
		s_data->CoreAssembly = Utils::LoadMonoAssembly(filepath, s_data->EnableDebugging);
		if (s_data->CoreAssembly == nullptr)
			return false;

		s_data->CoreAssemblyImage = mono_assembly_get_image(s_data->CoreAssembly);
		return true;
	}

	bool ScriptEngine::LoadAppAssembly(const std::filesystem::path& filepath)
	{
		s_data->AppAssemblyFilepath = filepath;
		s_data->AppAssembly = Utils::LoadMonoAssembly(filepath, s_data->EnableDebugging);
		if (s_data->AppAssembly == nullptr)
			return false;

		s_data->AppAssemblyImage = mono_assembly_get_image(s_data->AppAssembly);

		// s_Data->AppAssemblyFileWatcher = CreateScope<filewatch::FileWatch<std::string>>(filepath.string(), OnAppAssemblyFileSystemEvent);
		s_data->AssemblyReloadPending = false;
		return true;
	}

	void ScriptEngine::ReloadAssembly()
	{
		mono_domain_set(mono_get_root_domain(), false);

		mono_domain_unload(s_data->AppDomain);

		LoadCoreAssembly(s_data->CoreAssemblyFilepath);
		LoadAppAssembly(s_data->AppAssemblyFilepath);
		LoadAssemblyClasses();

		ScriptRegister::RegisterComponents();

		// Retrieve and instantiate class
		s_data->EngineObjectClass = ScriptClass("LitchiEngine", "Object", true);
	}

	void ScriptEngine::InitMono()
	{
		// 设置mono所需的运行时程序集目录, 如果不设置将使用默认的位置
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("LitchiMonoRuntime");

		// 保存rootDomain
		s_data->RootDomain = rootDomain;

		// 标记线程为domain的使用线程
		mono_thread_set_main(mono_thread_current());
	}

	void ScriptEngine::ShutdownMono()
	{
		mono_domain_set(mono_get_root_domain(), false);

		mono_domain_unload(s_data->AppDomain);
		s_data->AppDomain = nullptr;

		mono_jit_cleanup(s_data->RootDomain);
		s_data->RootDomain = nullptr;
	}

	MonoImage* ScriptEngine::GetCoreAssemblyImage()
	{
		return s_data->CoreAssemblyImage;
	}

	MonoObject* ScriptEngine::GetManagedInstance(uint64_t unmanagedId)
	{
		return s_data->ScriptObjectInstanceDict.at(unmanagedId)->GetManagedObject();
	}

	MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(s_data->AppDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	void ScriptEngine::LoadAssemblyClasses()
	{
		s_data->ScriptObjectClassDict.clear();

		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_data->AppAssemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* engineObjectClass = mono_class_from_name(s_data->CoreAssemblyImage, "LitchiEngine", "Object");

		// 收集所有Object的子类
		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(s_data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* className = mono_metadata_string_heap(s_data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);
			std::string fullName;
			if (strlen(nameSpace) != 0)
				fullName = fmt::format("{}.{}", nameSpace, className);
			else
				fullName = className;

			// 获取并检查程序集中的类型是否是LitchiEngine的子类
			MonoClass* monoClass = mono_class_from_name(s_data->AppAssemblyImage, nameSpace, className);
			if (monoClass == engineObjectClass)
				continue;
			bool isEngineObject = mono_class_is_subclass_of(monoClass, engineObjectClass, false);
			if (!isEngineObject)
				continue;

			Ref<ScriptClass> scriptClass = CreateRef<ScriptClass>(nameSpace, className);
			s_data->ScriptObjectClassDict[fullName] = scriptClass;


			// This routine is an iterator routine for retrieving the fields in a class.
			// You must pass a gpointer that points to zero and is treated as an opaque handle
			// to iterate over all of the elements. When no more values are available, the return value is NULL.

			int fieldCount = mono_class_num_fields(monoClass);
			//  DEBUG_LOG_INFO("{} has {} fields:", className, fieldCount);
			void* iterator = nullptr;
			while (MonoClassField* field = mono_class_get_fields(monoClass, &iterator))
			{
				const char* fieldName = mono_field_get_name(field);
				uint32_t flags = mono_field_get_flags(field);
				// if (flags & FIELD_ATTRIBUTE_PUBLIC) // todo
				{
					MonoType* type = mono_field_get_type(field);
					ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(type);
					// DEBUG_LOG_INFO("  {} ({})", fieldName, Utils::ScriptFieldTypeToString(fieldType));

					scriptClass->m_fields[fieldName] = { fieldType, fieldName, field };
				}
			}

		}

		auto& entityClasses = s_data->ScriptObjectClassDict;

		//mono_field_get_value()

	}

#endif
}
