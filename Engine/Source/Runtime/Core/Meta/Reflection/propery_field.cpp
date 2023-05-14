#include "propery_field.h"
#include "object.h"
#include "Runtime/Function/Framework/Component/Base/component.h"
LitchiRuntime::PropertyField::PropertyField(Component* root, const std::vector<std::string>& propertyNameList)
{
	m_root = root;
	m_propertyNameList = propertyNameList;
}

LitchiRuntime::PropertyField::~PropertyField()
{
	m_root = nullptr;
}

void WriteChild(rttr::instance intInput,const std::vector<std::string>& propertyNameList,int propIndex, rttr::variant value)
{
	instance instance = intInput.get_type().get_raw_type().is_wrapper() ? intInput.get_wrapped_instance() : intInput;
	bool isValid = false;
	auto propName = propertyNameList[propIndex];
	auto prop = instance.get_derived_type().get_property(propName);

	auto childIns = prop.get_value(instance);

	if(propIndex == propertyNameList.size() - 1)
	{
		auto temp = childIns.to_float();
		auto temp2 = value.to_float();
		isValid = value.convert(prop.get_type());
		isValid = prop.set_value(instance, value);
	}
	else
	{
		WriteChild(childIns, propertyNameList, ++propIndex, value);
		isValid = prop.set_value(instance, childIns);
	}
}

bool LitchiRuntime::PropertyField::SetValue(rttr::variant value) const
{
	rttr::instance instance = *m_root;

	int propIndex = 0;
	WriteChild(instance, m_propertyNameList, propIndex, value);

	return false;
}
