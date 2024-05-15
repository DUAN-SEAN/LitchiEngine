
#pragma once

#include <string>
#include <unordered_map>


namespace LitchiRuntime
{
	/**
	 * @brief The IniFile class represents a file .ini that stores a set of attributes/values that can get read and written
	*/
	class IniFile final
	{
	public:
		using AttributePair = std::pair<std::string, std::string>;
		using AttributeMap = std::unordered_map<std::string, std::string>;

		IniFile() = default;
		/**
		 * @brief Create an IniFile by parsing the given file path and extracting key/values pairs for future usage
		 * @param p_filePath 
		*/
		IniFile(const std::string& p_filePath);

		IniFile(AttributeMap& data);

		/**
		 * @brief Overwrite the content of the current data by reloading the file
		*/
		void Reload();

		/**
		 * @brief Rewrite the entiere .ini file with the current values. This operation is destructive and can't be undone.
		 * Any comment or line break in your .ini file will get destroyed
		*/
		void Rewrite() const;

		/**
		 * @brief Return the value attached to the given key
		 * If the key doesn't exist, a default value is returned (0, false, "NULL")
		 * @tparam T 
		 * @param p_key 
		 * @return 
		*/
		template<typename T>
		T Get(const std::string& p_key);

		/**
		 * @brief Return the value attached to the given key
		 * If the key doesn't exist, the specified value is returned
		 * @tparam T 
		 * @param p_key 
		 * @param p_default 
		 * @return 
		*/
		template<typename T>
		T GetOrDefault(const std::string& p_key, T p_default);

		/**
		 * @brief Set a new value to the given key (Not applied to the real file untill Rewrite() or Save() is called)
		 * @tparam T 
		 * @param p_key 
		 * @param p_value 
		 * @return 
		*/
		template<typename T>
		bool Set(const std::string& p_key, const T& p_value);

		/**
		 * @brief Add a new key/value to the IniFile object (Not applied to the real file untill Rewrite() or Save() is called)
		 * @tparam T 
		 * @param p_key 
		 * @param p_value 
		 * @return 
		*/
		template<typename T>
		bool Add(const std::string& p_key, const T& p_value);

		/**
		 * @brief Remove an key/value pair identified by the given key (Not applied to the real file untill Rewrite() or Save() is called)
		 * @param p_key 
		 * @return 
		*/
		bool Remove(const std::string& p_key);

		/**
		 * @brief Remove all key/value pairs (Not applied to the real file untill Rewrite() or Save() is called)
		*/
		void RemoveAll();

		/**
		 * @brief Verify if the given key exists
		 * @param p_key 
		 * @return 
		*/
		bool IsKeyExisting(const std::string& p_key) const;

		/**
		 * @brief Get the content stored in the ini file as a vector of strings (Each string correspond to an attribute pair : Attribute=Value
		 * @return 
		*/
		std::vector<std::string> GetFormattedContent() const;

		AttributeMap	GetData() { return m_data; }

	private:

		/**
		 * @brief Register pair from key and value
		 * @param p_key 
		 * @param p_value 
		*/
		void RegisterPair(const std::string& p_key, const std::string& p_value);

		/**
		 * @brief Register pair from attributePair
		 * @param p_pair 
		*/
		void RegisterPair(const AttributePair& p_pair);

		/**
		 * @brief Load iniFile
		*/
		void Load();

		AttributePair	ExtractKeyAndValue(const std::string& p_attributeLine)	const;
		bool			IsValidLine(const std::string& p_attributeLine)	const;
		bool			StringToBoolean(const std::string& p_value)			const;

	private:
		std::string		m_filePath;
		AttributeMap	m_data;
	};
}

#include "IniFile.inl"