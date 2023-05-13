#include "propery_field.h"

LitchiRuntime::PropertyField::PropertyField(rttr::type type,std::vector<std::string> propertyNameList)
{
    m_type = type;
    m_propertyNameList = propertyNameList;
}

LitchiRuntime::PropertyField::~PropertyField()
{
}

bool LitchiRuntime::PropertyField::SetValue(rttr::instance instance,rttr::variant value)
{
    return false;
}
