﻿
#pragma once

#include "Runtime/Core/Log/debug.h"

#include <filesystem>
#include <string>
#include <map>
#include <unordered_map>
#include <stdint.h>

template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoReflectionType MonoReflectionType;
}

namespace LitchiRuntime
{
	enum class ScriptFieldType
	{
		None = 0,
		Float, Double,
		Bool, Char, Byte, Short, Int, Long,
		UByte, UShort, UInt, ULong,
		Vector2, Vector3, Vector4
	};

	struct ScriptField
	{
		ScriptFieldType Type;
		std::string Name;

		MonoClassField* ClassField;
	};

	/**
	 * \brief 脚本字段的实例, 并且包含了数据
	 */
	struct ScriptFieldInstance
	{
		ScriptField Field;

		ScriptFieldInstance()
		{
			memset(m_Buffer, 0, sizeof(m_Buffer));
		}

		template<typename T>
		T GetValue()
		{
			static_assert(sizeof(T) <= 16, "Type too large!");
			return *(T*)m_Buffer;
		}

		template<typename T>
		void SetValue(T value)
		{
			static_assert(sizeof(T) <= 16, "Type too large!");
			memcpy(m_Buffer, &value, sizeof(T));
		}
	private:
		uint8_t m_Buffer[16];

		friend class ScriptEngine;
		friend class ScriptInstance;
	};

	using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;

	/**
	 * \brief 脚本类句柄 主要用于定义脚本组件(ScriptObject)
	 */
	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore = false);

		MonoObject* Instantiate();
		MonoMethod* GetMethod(const std::string& name, int parameterCount);
		/**
		 * \brief 调用方法
		 * \param instance 脚本运行时对象
		 * \param method 方法句柄
		 * \param params 参数列表
		 * \return
		 */
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

		/**
		 * \brief 获取所有方法
		 * \return 方法列表
		 */
		const std::map<std::string, ScriptField>& GetFields() const { return m_fields; }
	private:
		std::string m_classNamespace;
		std::string m_className;

		std::map<std::string, ScriptField> m_fields;

		MonoClass* m_monoClass = nullptr;

		friend class ScriptEngine;
	};

	/**
	 * \brief 脚本实例
	 */
	class ScriptInstance
	{
	public:
		ScriptInstance(Ref<ScriptClass> scriptClass, uint64_t unmanagedId);

		void InvokeOnCreate();
		void InvokeOnUpdate(float ts);


		void Invoke(std::string methodName, void* param, int paramCount = 1);
		void InvokeBaseClass(Ref<ScriptClass> baseScriptClass, std::string methodName, void* param, int paramCount = 1);
		Ref<ScriptClass> GetScriptClass() { return m_scriptClass; }
		uint64_t GetUnmanagedId();

		template<typename T>
		T GetFieldValue(const std::string& name)
		{
			static_assert(sizeof(T) <= 16, "Type too large!");

			bool success = GetFieldValueInternal(name, s_FieldValueBuffer);
			if (!success)
				return T();

			return *(T*)s_FieldValueBuffer;
		}

		template<typename T>
		void SetFieldValue(const std::string& name, T value)
		{
			static_assert(sizeof(T) <= 16, "Type too large!");

			SetFieldValueInternal(name, &value);
		}

		MonoObject* GetManagedObject() { return m_managedObject; }
	private:
		bool GetFieldValueInternal(const std::string& name, void* buffer);
		bool SetFieldValueInternal(const std::string& name, const void* value);
	private:
		Ref<ScriptClass> m_scriptClass;
		uint64_t m_unmanagedId;

		MonoObject* m_managedObject = nullptr;
		MonoMethod* m_constructor = nullptr;
		MonoMethod* m_onCreateMethod = nullptr;
		MonoMethod* m_onUpdateMethod = nullptr;

		inline static char s_FieldValueBuffer[16];

		friend class ScriptEngine;
		friend struct ScriptFieldInstance;
	};

	class ScriptEngine
	{
	public:
		/**
		 * \brief Entry Init
		 */
		static void Init(std::string dataPath);
		static void Shutdown();

		static bool LoadCoreAssembly(const std::filesystem::path& filepath);
		static bool LoadAppAssembly(const std::filesystem::path& filepath);

		static void ReloadAssembly();

		static MonoImage* GetCoreAssemblyImage();

		/**
		 * \brief 获取托管对象实例
		 * \param unmanagedId 非托管id
		 * \return
		 */
		static MonoObject* GetManagedInstance(uint64_t unmanagedId);

		static uint64_t  CreateScene();
		static uint64_t  CreateGameObject(uint64_t sceneUnmanagedId);
		static uint64_t CreateComponent(uint64_t gameObjectUnmanagedId, const std::string& componentTypeName);
		static uint64_t CreateScriptComponent(uint64_t gameObjectUnmanagedId, const std::string& scriptName);
		static void InvokeMethod(uint64_t unmanagedId, std::string methodName, void* param);

	private:
		static void InitMono(std::string monoDllPath);
		static void ShutdownMono();

		static MonoObject* InstantiateClass(MonoClass* monoClass);
		static void LoadAssemblyClasses();

		static Ref<ScriptInstance> CreateScriptInstance(Ref<ScriptClass> scriptClass);

		friend class ScriptClass;
	};

	namespace Utils {

		inline const char* ScriptFieldTypeToString(ScriptFieldType fieldType)
		{
			switch (fieldType)
			{
			case ScriptFieldType::None:    return "None";
			case ScriptFieldType::Float:   return "Float";
			case ScriptFieldType::Double:  return "Double";
			case ScriptFieldType::Bool:    return "Bool";
			case ScriptFieldType::Char:    return "Char";
			case ScriptFieldType::Byte:    return "Byte";
			case ScriptFieldType::Short:   return "Short";
			case ScriptFieldType::Int:     return "Int";
			case ScriptFieldType::Long:    return "Long";
			case ScriptFieldType::UByte:   return "UByte";
			case ScriptFieldType::UShort:  return "UShort";
			case ScriptFieldType::UInt:    return "UInt";
			case ScriptFieldType::ULong:   return "ULong";
			case ScriptFieldType::Vector2: return "Vector2";
			case ScriptFieldType::Vector3: return "Vector3";
			case ScriptFieldType::Vector4: return "Vector4";
			}
			DEBUG_LOG_ERROR("Unknown ScriptFieldType");
			return "None";
		}

		inline ScriptFieldType ScriptFieldTypeFromString(std::string_view fieldType)
		{
			if (fieldType == "None")    return ScriptFieldType::None;
			if (fieldType == "Float")   return ScriptFieldType::Float;
			if (fieldType == "Double")  return ScriptFieldType::Double;
			if (fieldType == "Bool")    return ScriptFieldType::Bool;
			if (fieldType == "Char")    return ScriptFieldType::Char;
			if (fieldType == "Byte")    return ScriptFieldType::Byte;
			if (fieldType == "Short")   return ScriptFieldType::Short;
			if (fieldType == "Int")     return ScriptFieldType::Int;
			if (fieldType == "Long")    return ScriptFieldType::Long;
			if (fieldType == "UByte")   return ScriptFieldType::UByte;
			if (fieldType == "UShort")  return ScriptFieldType::UShort;
			if (fieldType == "UInt")    return ScriptFieldType::UInt;
			if (fieldType == "ULong")   return ScriptFieldType::ULong;
			if (fieldType == "Vector2") return ScriptFieldType::Vector2;
			if (fieldType == "Vector3") return ScriptFieldType::Vector3;
			if (fieldType == "Vector4") return ScriptFieldType::Vector4;

			DEBUG_LOG_ERROR("Unknown ScriptFieldType");
			return ScriptFieldType::None;
		}

	}
}
