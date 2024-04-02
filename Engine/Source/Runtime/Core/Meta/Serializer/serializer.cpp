
#include "Runtime/Core/pch.h"

#define RAPIDJSON_HAS_STDSTRING 1

#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>

#include "serializer.h"
#include "rttr/type"
#include "Runtime/Core/Meta/Reflection/type.h"

using std::ifstream;
using std::ios;
using std::cout;
using std::endl;
using namespace rapidjson;

namespace LitchiRuntime
{
#if 1 // Serialize

	static void to_json_recursively(const instance& obj, PrettyWriter<StringBuffer>& writer);

	/////////////////////////////////////////////////////////////////////////////////////////
	static bool write_variant(const variant& var, PrettyWriter<StringBuffer>& writer);

	static bool write_atomic_types_to_json(const type& t, const variant& var, PrettyWriter<StringBuffer>& writer)
	{
		if (t.is_arithmetic())
		{
			if (t == type::get<bool>())
				writer.Bool(var.to_bool());
			else if (t == type::get<char>())
				writer.Bool(var.to_bool());
			else if (t == type::get<int8_t>())
				writer.Int(var.to_int8());
			else if (t == type::get<int16_t>())
				writer.Int(var.to_int16());
			else if (t == type::get<int32_t>())
				writer.Int(var.to_int32());
			else if (t == type::get<int64_t>())
				writer.Int64(var.to_int64());
			else if (t == type::get<uint8_t>())
				writer.Uint(var.to_uint8());
			else if (t == type::get<uint16_t>())
				writer.Uint(var.to_uint16());
			else if (t == type::get<uint32_t>())
				writer.Uint(var.to_uint32());
			else if (t == type::get<uint64_t>())
				writer.Uint64(var.to_uint64());
			else if (t == type::get<float>())
				writer.Double(var.to_double());
			else if (t == type::get<double>())
				writer.Double(var.to_double());

			return true;
		}
		else if (t.is_enumeration())
		{
			bool ok = false;
			auto result = var.to_string(&ok);
			if (ok)
			{
				writer.String(var.to_string());
			}
			else
			{
				ok = false;
				auto value = var.to_uint64(&ok);
				if (ok)
					writer.Uint64(value);
				else
					writer.Null();
			}

			return true;
		}
		else if (t == type::get<std::string>())
		{
			writer.String(var.to_string());
			return true;
		}

		return false;
	}

	/////////////////////////////////////////////////////////////////////////////////////////

	static void write_array(const variant_sequential_view& view, PrettyWriter<StringBuffer>& writer)
	{
		writer.StartArray();
		for (const auto& item : view)
		{
			if (item.is_sequential_container())
			{
				write_array(item.create_sequential_view(), writer);
			}
			else
			{
				variant wrapped_var = item.extract_wrapped_value();
				type value_type = wrapped_var.get_type();
				if (value_type.is_arithmetic() || value_type == type::get<std::string>() || value_type.is_enumeration())
				{
					write_atomic_types_to_json(value_type, wrapped_var, writer);
				}
				else // object
				{
					to_json_recursively(wrapped_var, writer);
				}
			}
		}
		writer.EndArray();
	}


	/////////////////////////////////////////////////////////////////////////////////////////

	static void write_associative_container(const variant_associative_view& view, PrettyWriter<StringBuffer>& writer)
	{
		static const string_view key_name("key");
		static const string_view value_name("value");

		writer.StartArray();

		if (view.is_key_only_type())
		{
			for (auto& item : view)
			{
				write_variant(item.first, writer);
			}
		}
		else
		{
			for (auto& item : view)
			{
				writer.StartObject();
				writer.String(key_name.data(), static_cast<rapidjson::SizeType>(key_name.length()), false);

				write_variant(item.first, writer);

				writer.String(value_name.data(), static_cast<rapidjson::SizeType>(value_name.length()), false);

				write_variant(item.second, writer);

				writer.EndObject();
			}
		}

		writer.EndArray();
	}

	/////////////////////////////////////////////////////////////////////////////////////////

	static bool write_variant(const variant& var, PrettyWriter<StringBuffer>& writer)
	{
		auto value_type = var.get_type();
		auto wrapped_type = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;
		bool is_wrapper = wrapped_type != value_type;

		if (write_atomic_types_to_json(is_wrapper ? wrapped_type : value_type,
			is_wrapper ? var.extract_wrapped_value() : var, writer))
		{
		}
		else if (var.is_sequential_container())
		{
			write_array(var.create_sequential_view(), writer);
		}
		else if (var.is_associative_container())
		{
			write_associative_container(var.create_associative_view(), writer);
		}
		else
		{
			auto child_props = is_wrapper ? wrapped_type.get_properties() : value_type.get_properties();
			if (!child_props.empty())
			{
				to_json_recursively(var, writer);
			}
			else
			{
				bool ok = false;
				auto text = var.to_string(&ok);
				if (!ok)
				{
					writer.String(text);
					return false;
				}

				writer.String(text);
			}
		}

		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////

	static void to_json_recursively(const instance& obj2, PrettyWriter<StringBuffer>& writer)
	{
		// todo 需要检查obj2是否是指针类型, 如果是指针类型则需要获取特性,如果特性打了多态
		// todo 如果打了多态, 则需要将多态的类型信息注入到json中

		writer.StartObject();
		instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;

		auto objType = obj.get_type();
		if (objType.is_pointer())
		{
			objType = objType.get_raw_type();
		}
		const std::string key = "Type";
		if (objType.get_metadata("Polymorphic").to_bool() == true)
		{
			writer.String(key.c_str(), key.length(), false);
			auto objDerivedType = obj.get_derived_type();
			if (objDerivedType.is_pointer())
			{
				objDerivedType = objDerivedType.get_raw_type();
			}
			std::string typeName = objDerivedType.get_name().data();
			writer.String(typeName.c_str(), typeName.length(), false);
		}

		auto prop_list = obj.get_derived_type().get_properties();
		for (auto prop : prop_list)
		{
			if (prop.get_metadata("NO_SERIALIZE"))
				continue;

			variant prop_value = prop.get_value(obj);
			if (!prop_value)
				continue; // cannot serialize, because we cannot retrieve the value

			const auto name = prop.get_name();
			writer.String(name.data(), static_cast<rapidjson::SizeType>(name.length()), false);
			if (!write_variant(prop_value, writer))
			{
				std::cerr << "cannot serialize property: " << name << std::endl;
			}
		}

		writer.EndObject();
	}

#endif

#if 1 // Deserialize


	/////////////////////////////////////////////////////////////////////////////////////////

	void fromjson_recursively(instance obj, Value& json_object);

	/////////////////////////////////////////////////////////////////////////////////////////

	variant extract_basic_types(Value& json_value)
	{
		switch (json_value.GetType())
		{
		case kStringType:
		{
			return std::string(json_value.GetString());
			break;
		}
		case kNullType:     break;
		case kFalseType:
		case kTrueType:
		{
			return json_value.GetBool();
			break;
		}
		case kNumberType:
		{
			if (json_value.IsInt())
				return json_value.GetInt();
			else if (json_value.IsDouble())
				return json_value.GetDouble();
			else if (json_value.IsUint())
				return json_value.GetUint();
			else if (json_value.IsInt64())
				return json_value.GetInt64();
			else if (json_value.IsUint64())
				return json_value.GetUint64();
			break;
		}
		// we handle only the basic types here
		case kObjectType:
		case kArrayType: return variant();
		}

		return variant();
	}


	/////////////////////////////////////////////////////////////////////////////////////////

	static void write_array_recursively(variant_sequential_view& view, Value& json_array_value)
	{
		bool result = false;
		view.set_size(json_array_value.Size());
		for (SizeType i = 0; i < json_array_value.Size(); ++i)
		{
			auto& json_index_value = json_array_value[i];
			if (json_index_value.IsArray())
			{
				auto sub_array_view = view.get_value(i).create_sequential_view();
				write_array_recursively(sub_array_view, json_index_value);
			}
			else if (json_index_value.IsObject())
			{
				variant var_tmp = view.get_value(i);
				variant wrapped_var = var_tmp.extract_wrapped_value();

				auto rawType2 = wrapped_var.get_type().get_raw_type();

				const std::string key = "Type";
				if (rawType2.get_metadata("Polymorphic"))
				{
					Value::MemberIterator ret = json_index_value.FindMember(key);
					auto realTypeName = std::string(ret->value.GetString(), ret->value.GetStringLength());
					rawType2 = (rttr::type::get_by_name(realTypeName));
					wrapped_var = rawType2.create();
					auto isPointer = wrapped_var.get_type().is_pointer();
				}

				fromjson_recursively(wrapped_var, json_index_value);
				result = view.set_value(i, wrapped_var);
				if (!result)
				{
					DEBUG_LOG_ERROR("write_array_recursively WriteObject False");
				}
			}
			else
			{
				const type array_type = view.get_rank_type(i);
				variant extracted_value = extract_basic_types(json_index_value);
				if (extracted_value.convert(array_type))
					view.set_value(i, extracted_value);
				else
				{
					DEBUG_LOG_ERROR("write_array_recursively Write Basic False");
				}
			}
		}
	}

	variant extract_value(Value::MemberIterator& itr, const type& t)
	{
		auto& json_value = itr->value;
		variant extracted_value = extract_basic_types(json_value);
		const bool could_convert = extracted_value.convert(t);
		if (!could_convert)
		{
			if (json_value.IsObject())
			{
				constructor ctor = t.get_constructor();
				for (auto& item : t.get_constructors())
				{
					if (item.get_instantiated_type() == t)
						ctor = item;
				}
				extracted_value = ctor.invoke();
				fromjson_recursively(extracted_value, json_value);
			}
		}

		return extracted_value;
	}

	static void write_associative_view_recursively(variant_associative_view& view, Value& json_array_value)
	{
		for (SizeType i = 0; i < json_array_value.Size(); ++i)
		{
			auto& json_index_value = json_array_value[i];
			if (json_index_value.IsObject()) // a key-value associative view
			{
				Value::MemberIterator key_itr = json_index_value.FindMember("key");
				Value::MemberIterator value_itr = json_index_value.FindMember("value");

				if (key_itr != json_index_value.MemberEnd() &&
					value_itr != json_index_value.MemberEnd())
				{
					auto key_var = extract_value(key_itr, view.get_key_type());

					auto value_var = extract_value(value_itr, view.get_value_type());
					if (key_var && value_var)
					{
						view.insert(key_var, value_var);
					}
				}
			}
			else // a key-only associative view
			{
				variant extracted_value = extract_basic_types(json_index_value);
				if (extracted_value && extracted_value.convert(view.get_key_type()))
					view.insert(extracted_value);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////

	void fromjson_recursively(instance obj2, Value& json_object)
	{
		// todo 需要检查obj2是否是指针类型, 如果是指针类型则需要获取特性,如果特性打了多态
		// todo 如果打了多态, 则需要从json中获取多态信息


		auto rawType = obj2.get_type().get_raw_type();
		auto isWrapper = rawType.is_wrapper();
		instance obj = isWrapper ? obj2.get_wrapped_instance() : obj2;

		const auto prop_list = obj.get_type().get_raw_type().get_properties();

		for (auto prop : prop_list)
		{
			Value::MemberIterator ret = json_object.FindMember(prop.get_name().data());
			if (ret == json_object.MemberEnd())
				continue;
			type value_t = prop.get_type();


			auto rawType2 = value_t.get_raw_type();

			const std::string key = "Type";
			if (rawType2.get_metadata("Polymorphic"))
			{
				Value::MemberIterator ret = json_object.FindMember(key);
				auto realTypeName = std::string(ret->value.GetString(), ret->value.GetStringLength());
				value_t = rttr::type::get_by_name(realTypeName);
			}

			auto& json_value = ret->value;
			switch (json_value.GetType())
			{
			case kArrayType:
			{
				variant var;
				if (value_t.is_sequential_container())
				{
					var = prop.get_value(obj);
					auto view = var.create_sequential_view();
					write_array_recursively(view, json_value);
				}
				else if (value_t.is_associative_container())
				{
					var = prop.get_value(obj);
					auto associative_view = var.create_associative_view();
					write_associative_view_recursively(associative_view, json_value);
				}

				prop.set_value(obj, var);
				break;
			}
			case kObjectType:
			{
				variant var = prop.get_value(obj);

				fromjson_recursively(var, json_value);
				prop.set_value(obj, var);
				break;
			}
			default:
			{
				const type value_t2 = prop.get_type();
				variant extracted_value = extract_basic_types(json_value);
				if (extracted_value.convert(value_t2)) // REMARK: CONVERSION WORKS ONLY WITH "const type", check whether this is correct or not!
					prop.set_value(obj, extracted_value);
			}
			}
		}
	}

#endif 

	std::string Serializer::SerializeToJson(rttr::instance obj)
	{
		if (!obj.is_valid())
			return std::string();

		StringBuffer sb;
		PrettyWriter<StringBuffer> writer(sb);

		to_json_recursively(obj, writer);

		return sb.GetString();
	}

	bool Serializer::DeserializeFromJson(const std::string& json, rttr::instance obj)
	{
		Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.

		// "normal" parsing, decode strings to new buffers. Can use other input stream via ParseStream().
		if (document.Parse(json.c_str()).HasParseError())
			return false;

		fromjson_recursively(obj, document);

		return true;
	}
}


