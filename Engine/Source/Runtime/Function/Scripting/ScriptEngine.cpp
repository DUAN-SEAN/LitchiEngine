#include "ScriptEngine.h"

#include <string>

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/mono-debug.h"
#include "mono/metadata/threads.h"
#include "mono/metadata/debug-helpers.h"

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
		///**
		// * \brief 应用Domain
		// */
		//MonoDomain* AppDomain = nullptr;
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
		ScriptClass EngineClass4ScriptObject;
		ScriptClass EngineClass4Component;
		ScriptClass EngineClass4ScriptComponent;
		/**
		 * \brief Scene的MonoClass定义
		 */
		ScriptClass EngineClass4Scene;
		/**
		 * \brief GameObject的MonoClass定义
		 */
		ScriptClass EngineClass4GameObject;
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

		// Scope<filewatch::FileWatch<std::string>> AppAssemblyFileWatcher;
		bool AssemblyReloadPending = false;

		/**
		 * \brief Id分配工厂
		 */
		uint64_t m_unmanagedIdFactory;
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
		auto result = mono_runtime_invoke(method, instance, params, &exception);
		if(exception != nullptr)
		{
			mono_unhandled_exception(exception);
			DEBUG_LOG_ERROR("InvokeMethod Fail Exception");

			return nullptr;
		}
		return result;
	}

#endif

#if 1 ScriptInstance

	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, uint64_t unmanagedId)
		: m_scriptClass(scriptClass),m_unmanagedId(unmanagedId)
	{
		m_managedObject = scriptClass->Instantiate();

		//m_constructor = scriptClass->GetMethod(".ctor", 1);
		//m_onCreateMethod = scriptClass->GetMethod("OnCreate", 0);
		//m_onUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);

		//// Call Entity constructor
		//{
		//	uint64_t entityID = unmanagedId;
		//	void* param = &entityID;
		//	m_scriptClass->InvokeMethod(m_instance, m_constructor, &param);
		//}
	}

	void ScriptInstance::InvokeOnCreate()
	{
		if (m_onCreateMethod)
			m_scriptClass->InvokeMethod(m_managedObject, m_onCreateMethod);
	}

	void ScriptInstance::InvokeOnUpdate(float ts)
	{
		if (m_onUpdateMethod)
		{
			void* param = &ts;
			m_scriptClass->InvokeMethod(m_managedObject, m_onUpdateMethod, &param);
		}
	}

	void ScriptInstance::Invoke(std::string methodName, void* param, int paramCount)
	{
		// 获取方法
		auto method  = m_scriptClass->GetMethod(methodName, paramCount);
		if(method == nullptr)
		{
			DEBUG_LOG_ERROR("Method Not Found, methodName:{}", methodName);
			return;
		}

		// 调用 resultObject could maybe none
		auto resultObject =  m_scriptClass->InvokeMethod(m_managedObject, method, &param);
	}

	void ScriptInstance::InvokeBaseClass(Ref<ScriptClass> baseScriptClass, std::string methodName, void* param, int paramCount)
	{
		// 获取方法
		auto method = baseScriptClass->GetMethod(methodName, paramCount);
		if (method == nullptr)
		{
			DEBUG_LOG_ERROR("Method Not Found, methodName:{}", methodName);
			return;
		}

		// 调用 resultObject could maybe none
		auto resultObject = m_scriptClass->InvokeMethod(m_managedObject, method, &param);
	}

	uint64_t ScriptInstance::GetUnmanagedId()
	{
		return m_unmanagedId;
	}

	bool ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer)
	{
		const auto& fields = m_scriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		const ScriptField& field = it->second;
		mono_field_get_value(m_managedObject, field.ClassField, buffer);
		return true;
	}

	bool ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value)
	{
		const auto& fields = m_scriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		const ScriptField& field = it->second;
		mono_field_set_value(m_managedObject, field.ClassField, (void*)value);
		return true;
	}

#endif

#if 1 ScriptEngine

	void ScriptEngine::Init(std::string dataPath)
	{
		s_data = new ScriptEngineData();

#if true DEBUG
		s_data->EnableDebugging = true;
#endif


		// 初始化mono
		std::string monoDllPath = dataPath + "Assets/Mono";
		InitMono(monoDllPath);

		// 加载引擎核心程序集 路径写死
		std::string scriptCoreDllPath = dataPath + "ScriptCore/Debug/LitchiScriptCore.dll";
		// std::string scriptCoreDllPath = dataPath + "ScriptCore/Debug/Hazel-ScriptCore.dll";
		bool status = LoadCoreAssembly(scriptCoreDllPath);
		if (!status)
		{
			DEBUG_LOG_ERROR("[ScriptEngine] Could not load LitchiScriptCore assembly.");
			return;
		}

		//// 加载项目的程序集
		//std::filesystem::path scriptModulePath;
		//status = LoadAppAssembly(scriptModulePath);
		//if (!status)
		//{
		//	DEBUG_LOG_ERROR("[ScriptEngine] Could not load app assembly.");
		//	return;
		//}

		// 加载程序集的类型
		LoadAssemblyClasses();

		// 注册方法
		ScriptRegister::RegisterFunctions();
		// 注册脚本
		ScriptRegister::RegisterComponents();

		s_data->EngineClass4ScriptObject = ScriptClass("LitchiEngine", "ScriptObject", true);
		s_data->EngineClass4Component = ScriptClass("LitchiEngine", "Component", true);
		s_data->EngineClass4ScriptComponent = ScriptClass("LitchiEngine", "ScriptComponent", true);
		s_data->EngineClass4Scene = ScriptClass("LitchiEngine", "Scene", true);
		s_data->EngineClass4GameObject = ScriptClass("LitchiEngine", "GameObject", true);

		//// 测试代码
		//MonoClass* monoClass = mono_class_from_name(s_data->CoreAssemblyImage, "LitchiEngine", "SceneManager");
		//MonoMethod* monoMethod = mono_class_get_method_from_name(monoClass, "LoadSceneFromEngine", 1);
		//auto sceneClass = s_data->ScriptObjectClassDict["LitchiEngine.Scene"];
		//auto scene = sceneClass->Instantiate();

		//void* param = &scene;
		//MonoObject* exception = nullptr;
		//mono_runtime_invoke(monoMethod, nullptr, &param, &exception);
	}

	void ScriptEngine::Shutdown()
	{
	}

	bool ScriptEngine::LoadCoreAssembly(const std::filesystem::path& filepath)
	{
		// Create an App Domain
		/*s_data->AppDomain = mono_domain_create_appdomain("LitchiScriptRuntime", nullptr);
		mono_domain_set(s_data->AppDomain, true);*/

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

		// mono_domain_unload(s_data->AppDomain);

		LoadCoreAssembly(s_data->CoreAssemblyFilepath);
		LoadAppAssembly(s_data->AppAssemblyFilepath);
		LoadAssemblyClasses();

		ScriptRegister::RegisterComponents();

		// Retrieve and instantiate class
		s_data->EngineClass4ScriptObject = ScriptClass("LitchiEngine", "ScriptObject", true);
		s_data->EngineClass4Component = ScriptClass("LitchiEngine", "Component", true);
		s_data->EngineClass4ScriptComponent = ScriptClass("LitchiEngine", "ScriptComponent", true);
		s_data->EngineClass4Scene = ScriptClass("LitchiEngine", "Scene", true);
		s_data->EngineClass4GameObject = ScriptClass("LitchiEngine", "GameObject", true);
	}

	void ScriptEngine::InitMono(std::string monoDllPath)
	{
		// 设置mono所需的运行时程序集目录, 如果不设置将使用默认的位置
		//mono_set_assemblies_path("mono/lib");
		mono_set_assemblies_path(monoDllPath.c_str());

		if (s_data->EnableDebugging)
		{
			const char* argv[2] = {
				"--debugger-agent=transport=dt_socket,address=127.0.0.1:2550,server=y,suspend=n,loglevel=3,logfile=MonoDebugger.log",
				"--soft-breakpoints"
			};

			mono_jit_parse_options(2, (char**)argv);
			mono_debug_init(MONO_DEBUG_FORMAT_MONO);
		}

		MonoDomain* rootDomain = mono_jit_init("LitchiMonoRuntime");

		// 保存rootDomain
		s_data->RootDomain = rootDomain;
		if (s_data->EnableDebugging)
			mono_debug_domain_create(s_data->RootDomain);

		// 标记线程为domain的使用线程
		mono_thread_set_main(mono_thread_current());
	}

	void ScriptEngine::ShutdownMono()
	{
		mono_domain_set(mono_get_root_domain(), false);

		/*mono_domain_unload(s_data->AppDomain);
		s_data->AppDomain = nullptr;*/

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

	uint64_t ScriptEngine::CreateScene()
	{
		// 通过脚本名获取脚本类型定义
		auto scriptInstance = ScriptEngine::CreateScriptInstance(CreateRef<ScriptClass>(s_data->EngineClass4Scene));
		if (scriptInstance == nullptr)
		{
			return 0;
		}

		uint64_t id = scriptInstance->GetUnmanagedId();
		return id;
	}

	uint64_t ScriptEngine::CreateGameObject(uint64_t sceneUnmanagedId)
	{
		// 通过脚本名获取脚本类型定义
		auto scriptClass = s_data->EngineClass4GameObject;
		

		auto scriptInstance = ScriptEngine::CreateScriptInstance(CreateRef<ScriptClass>(scriptClass));
		if (scriptInstance == nullptr)
		{
			return 0;
		}

		uint64_t id = scriptInstance->GetUnmanagedId();
		scriptInstance->Invoke("SetSceneUnmanagedIdFromEngine", &sceneUnmanagedId, 1);
		return id;
	}

	uint64_t ScriptEngine::CreateComponent(uint64_t gameObjectUnmanagedId, const std::string& componentTypeName)
	{
		auto fullName = fmt::format("{}.{}", "LitchiEngine", componentTypeName);
		auto scriptClass = s_data->ScriptObjectClassDict[fullName];
		if (scriptClass == nullptr)
		{
			return 0;
		}

		auto scriptInstance = ScriptEngine::CreateScriptInstance(scriptClass);
		if (scriptInstance == nullptr)
		{
			return 0;
		}

		uint64_t id = scriptInstance->GetUnmanagedId();
		scriptInstance->InvokeBaseClass(CreateRef<ScriptClass>(s_data->EngineClass4Component),"SetGameObjectUnmanagedIdFromEngine", &gameObjectUnmanagedId, 1);
	/*	auto mdesc = mono_method_desc_new(":SetGameObjectUnmanagedIdFromEngine()", false);
		auto vtmethod = mono_method_desc_search_in_class(mdesc, scriptClass->m_monoClass);*/

		return id;
	}

	uint64_t ScriptEngine::CreateScriptComponent(uint64_t gameObjectUnmanagedId, const std::string& scriptName)
	{
		// 通过脚本名获取脚本类型定义
		auto scriptClass = s_data->ScriptObjectClassDict[scriptName];
		if (scriptClass == nullptr)
		{
			return 0;
		}

		auto scriptInstance = ScriptEngine::CreateScriptInstance(scriptClass);
		if(scriptInstance == nullptr)
		{
			return 0;
		}

		uint64_t id = scriptInstance->GetUnmanagedId();
		scriptInstance->InvokeBaseClass(CreateRef<ScriptClass>(s_data->EngineClass4Component), "SetGameObjectUnmanagedIdFromEngine", &gameObjectUnmanagedId, 1);

		return id;
	}

	MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(s_data->RootDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	void ScriptEngine::LoadAssemblyClasses()
	{
		s_data->ScriptObjectClassDict.clear();

		// 找到引擎的脚本基类Class
		MonoClass* engineObjectClass = mono_class_from_name(s_data->CoreAssemblyImage, "LitchiEngine", "ScriptObject");
		if(engineObjectClass == nullptr)
		{
			DEBUG_LOG_ERROR("engineObjectClass Not Found");
			return;
		}

		// 加载CoreAssembly 收集所有ScriptObject的子类
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_data->CoreAssemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(s_data->CoreAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* className = mono_metadata_string_heap(s_data->CoreAssemblyImage, cols[MONO_TYPEDEF_NAME]);
			std::string fullName;
			if (strlen(nameSpace) != 0)
				fullName = fmt::format("{}.{}", nameSpace, className);
			else
				fullName = className;

			// 获取并检查程序集中的类型是否是LitchiEngine的子类
			MonoClass* monoClass = mono_class_from_name(s_data->CoreAssemblyImage, nameSpace, className);
			if (monoClass == engineObjectClass)
				continue;
			bool isEngineObject = mono_class_is_subclass_of(monoClass, engineObjectClass, false);
			if (!isEngineObject)
				continue;

			// 将Class添加到静态字典中
			Ref<ScriptClass> scriptClass = CreateRef<ScriptClass>(nameSpace, className, true);
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

		// 加载AppAssembly 收集所有ScriptObject的子类
		//typeDefinitionsTable = mono_image_get_table_info(s_data->AppAssemblyImage, MONO_TABLE_TYPEDEF);
		//numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		//for (int32_t i = 0; i < numTypes; i++)
		//{
		//	uint32_t cols[MONO_TYPEDEF_SIZE];
		//	mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

		//	const char* nameSpace = mono_metadata_string_heap(s_data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
		//	const char* className = mono_metadata_string_heap(s_data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);
		//	std::string fullName;
		//	if (strlen(nameSpace) != 0)
		//		fullName = fmt::format("{}.{}", nameSpace, className);
		//	else
		//		fullName = className;

		//	// 获取并检查程序集中的类型是否是LitchiEngine的子类
		//	MonoClass* monoClass = mono_class_from_name(s_data->AppAssemblyImage, nameSpace, className);
		//	if (monoClass == engineObjectClass)
		//		continue;
		//	bool isEngineObject = mono_class_is_subclass_of(monoClass, engineObjectClass, false);
		//	if (!isEngineObject)
		//		continue;

		//	// 将Class添加到静态字典中
		//	Ref<ScriptClass> scriptClass = CreateRef<ScriptClass>(nameSpace, className);
		//	s_data->ScriptObjectClassDict[fullName] = scriptClass;


		//	// This routine is an iterator routine for retrieving the fields in a class.
		//	// You must pass a gpointer that points to zero and is treated as an opaque handle
		//	// to iterate over all of the elements. When no more values are available, the return value is NULL.

		//	int fieldCount = mono_class_num_fields(monoClass);
		//	//  DEBUG_LOG_INFO("{} has {} fields:", className, fieldCount);
		//	void* iterator = nullptr;
		//	while (MonoClassField* field = mono_class_get_fields(monoClass, &iterator))
		//	{
		//		const char* fieldName = mono_field_get_name(field);
		//		uint32_t flags = mono_field_get_flags(field);
		//		// if (flags & FIELD_ATTRIBUTE_PUBLIC) // todo
		//		{
		//			MonoType* type = mono_field_get_type(field);
		//			ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(type);
		//			// DEBUG_LOG_INFO("  {} ({})", fieldName, Utils::ScriptFieldTypeToString(fieldType));

		//			scriptClass->m_fields[fieldName] = { fieldType, fieldName, field };
		//		}
		//	}

		//}

		// 打印
		// auto& entityClasses = s_data->ScriptObjectClassDict;
		//mono_field_get_value()
	}


	Ref<ScriptInstance> ScriptEngine::CreateScriptInstance(Ref<ScriptClass> scriptClass)
	{
		auto id = ++s_data->m_unmanagedIdFactory;

		// 通过脚本类型定义和id构建脚本实例
		Ref<ScriptInstance> scriptInstance = CreateRef<ScriptInstance>(scriptClass, id);
		scriptInstance->InvokeBaseClass(CreateRef<ScriptClass>(s_data->EngineClass4ScriptObject),"SetUnmanagedIdFromEngine", &id, 1);

		// 添加实例映射
		s_data->ScriptObjectInstanceDict[id] = scriptInstance;

		return scriptInstance;
	}
#endif
}
