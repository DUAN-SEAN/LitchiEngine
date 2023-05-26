#include "propery_field.h"
#include "object.h"
#include "Runtime/Function/Framework/Component/Base/component.h"
LitchiRuntime::PropertyField::PropertyField(Object* root, const std::vector<std::string>& propertyNameList)
{
	m_root = root;
	m_propertyNameList = propertyNameList;
}

LitchiRuntime::PropertyField::~PropertyField()
{
	m_root = nullptr;
}

void WriteChild(rttr::instance intInput, const std::vector<std::string>& propertyNameList, int propIndex, rttr::variant value);

void WriteArray(variant_sequential_view& view, const std::vector<std::string>& propertyNameList, int propIndex, rttr::variant value)
{
	bool isValid = false;

	// 解析字符串为元素索引
	int itemIndex = std::stoi(propertyNameList[propIndex]);

	// 获取索引所指定的Item
	auto itemVar = view.get_value(itemIndex);

	// 获取元素类型
	const auto arrayItemType = view.get_rank_type(itemIndex);

	// 检查是否已经到达根,只可能是基础类型,因此直接向数组中写入
	if (itemIndex == propertyNameList.size() - 1)
	{
		isValid = value.convert(arrayItemType);
		isValid = view.set_value(itemIndex, value);
	}
	else
	{
		// 否则先判断数组中元素是否依旧是数组类型
		if (arrayItemType.is_sequential_container())
		{
			// 创建元素的视图
			auto childView = itemVar.create_sequential_view();
			WriteArray(childView, propertyNameList, ++propIndex, value);
		}
		// 如果不是数组类型,而是复合Object类型,则嵌套写入
		else
		{
			variant var_tmp = view.get_value(itemIndex);
			variant wrapped_var = var_tmp.extract_wrapped_value();
			WriteChild(wrapped_var, propertyNameList, ++propIndex, value);
			isValid = view.set_value(itemIndex, wrapped_var);

		}
	}
}

void WriteChild(rttr::instance intInput, const std::vector<std::string>& propertyNameList, int propIndex, rttr::variant value)
{
	bool isValid = false;

	instance instance = intInput.get_type().get_raw_type().is_wrapper() ? intInput.get_wrapped_instance() : intInput;

	auto propName = propertyNameList[propIndex]; // 获取属性名
	auto insType = instance.get_derived_type(); // 获取实例的真实类型
	auto prop = insType.get_property(propName); // 通过真实类型和属性名获取属性
	auto propType = prop.get_type(); // 获取属性的类型
	auto childIns = prop.get_value(instance); // 获取属性的值

	// 检查是否已经到达根,只可能是基础类型,因此直接向数组中写入
	if (propIndex == propertyNameList.size() - 1)
	{
		isValid = value.convert(prop.get_type());
		isValid = prop.set_value(instance, value);
	}
	else
	{
		// 否则先判断数组中元素是否依旧是数组类型
		if (propType.is_sequential_container())
		{
			auto view = childIns.create_sequential_view();
			WriteArray(view, propertyNameList, ++propIndex, value);
		}
		// 如果不是数组类型,而是复合Object类型,则嵌套写入
		else
		{
			WriteChild(childIns, propertyNameList, ++propIndex, value);
			isValid = prop.set_value(instance, childIns);
		}
	}


}

bool LitchiRuntime::PropertyField::SetValue(rttr::variant value) const
{
	rttr::instance instance = *m_root;

	int propIndex = 0;
	WriteChild(instance, m_propertyNameList, propIndex, value);

	// 调用资源加载完毕接口,重新刷新资源
	m_root->PostResourceModify();

	return true;
}


variant ReadChild(const instance& intInput, const std::vector<std::string>& propertyNameList, int propIndex);

variant ReadArray(const variant_sequential_view& view, const std::vector<std::string>& propertyNameList, int propIndex)
{
	bool isValid = false;

	// 解析字符串为元素索引
	int itemIndex = std::stoi(propertyNameList[propIndex]);

	// 获取索引所指定的Item
	auto itemVar = view.get_value(itemIndex);

	// 获取元素类型
	const auto arrayItemType = view.get_rank_type(itemIndex);

	// 检查是否已经到达根,只可能是基础类型,因此直接向数组中写入
	if (itemIndex == propertyNameList.size() - 1)
	{
		return view.get_value(itemIndex);
	}
	else
	{
		// 否则先判断数组中元素是否依旧是数组类型
		if (arrayItemType.is_sequential_container())
		{
			// 创建元素的视图
			auto childView = itemVar.create_sequential_view();
			return ReadArray(childView, propertyNameList, ++propIndex);
		}
		// 如果不是数组类型,而是复合Object类型,则嵌套写入
		else
		{
			variant var_tmp = view.get_value(itemIndex);
			variant wrapped_var = var_tmp.extract_wrapped_value();
			return ReadChild(wrapped_var, propertyNameList, ++propIndex);
		}
	}
}

variant ReadChild(const instance& intInput, const std::vector<std::string>& propertyNameList, int propIndex)
{
	bool isValid = false;

	instance instance = intInput.get_type().get_raw_type().is_wrapper() ? intInput.get_wrapped_instance() : intInput;

	auto propName = propertyNameList[propIndex]; // 获取属性名
	auto insType = instance.get_derived_type(); // 获取实例的真实类型
	auto prop = insType.get_property(propName); // 通过真实类型和属性名获取属性
	auto propType = prop.get_type(); // 获取属性的类型
	auto childIns = prop.get_value(instance); // 获取属性的值

	// 检查是否已经到达根,只可能是基础类型,因此直接向数组中写入
	if (propIndex == propertyNameList.size() - 1)
	{
		return prop.get_value(instance);
	}
	else
	{
		// 否则先判断数组中元素是否依旧是数组类型
		if (propType.is_sequential_container())
		{
			auto view = childIns.create_sequential_view();
			return ReadArray(view, propertyNameList, ++propIndex);
		}
		// 如果不是数组类型,而是复合Object类型,则嵌套写入
		else
		{
			return ReadChild(childIns, propertyNameList, ++propIndex);
		}
	}



}

rttr::variant LitchiRuntime::PropertyField::GetValue() const
{
	rttr::instance instance = *m_root;

	int propIndex = 0;
	return ReadChild(instance, m_propertyNameList, propIndex);
}
