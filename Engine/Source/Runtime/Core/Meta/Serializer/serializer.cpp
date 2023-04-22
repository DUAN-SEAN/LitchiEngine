#include "serializer.h"

#include "Runtime/Core/App/application.h"
#include "Runtime/Function/Renderer/material.h"

using std::ifstream;
using std::ios;
using std::cout;
using std::endl;

void SerializerManager::Initialize(SerializerManager* instance)
{
	// instance_ = instance;

	Material* t = new Material();

    //解析xml
	std::ifstream input_file_stream(Application::GetDataPath() + "material/materialTemplete.mat", ios::in | ios::binary);

    rapidxml::file<> xml_file(input_file_stream);
    rapidxml::xml_document<> document;
    document.parse<0>(xml_file.data());

    //根节点
    rapidxml::xml_node<>* material_node = document.first_node("root");
    if (material_node == nullptr) {
        return;
    }

    auto rootTypeAttr = material_node->first_attribute("type");
    DEBUG_LOG_INFO(rootTypeAttr->value());
    rttr::type rootType = rttr::type::get_by_name(rootTypeAttr->value());

    auto rootObject = rootType.create();
    auto rootPropertyArr  = rootType.get_properties();
    for (auto rootProperty:rootPropertyArr)
    {
        auto propertyName = rootProperty.get_name().to_string();

        auto propertyNode  = material_node->first_node(propertyName.c_str());
        auto propertyNodeTypeName = std::string( propertyNode->first_attribute("type")->value());
        auto propertyNodeSubTypeAttr = propertyNode->first_attribute("subType");
        char* propertySubName = nullptr;
        if(propertyNodeSubTypeAttr != nullptr)
        {
        	propertySubName = propertyNode->first_attribute("subType")->value();
        }

        {
			if (propertyNodeTypeName == std::string("std::string"))
			{
                rootType.set_property_value(propertyName, rootObject, propertyNode->first_attribute("value")->value());
			}
            else if(propertyNodeTypeName == std::string("std::vector<>"))
            {
                if (propertySubName == std::string("std::string"))
                {
                    std::vector<std::string> stringArr;
                    // todo 填充数据

                    // 写入object中
                    rootType.set_property_value(propertyName, rootObject, stringArr);
                }else
                {
                    rttr::type subType = rttr::type::get_by_name(propertySubName);

                    if(subType.is_valid())
                    {
                        // 构建子对象
                        auto subObject = subType.create();

                        auto subPropertyArr = rootType.get_properties();
                        for (auto subProperty:subPropertyArr)
                        {
	                        
                        }

                        // todo 如何将子对象添加到vector中
                        auto  propertyValue =  rootType.get_property_value(propertyName, rootObject);
                        auto propertyType = rootProperty.get_type();
                        auto methods = propertyType.get_methods();
                    	// propertyValue->
                    }
                }
            }

            if(rootProperty.is_valid())
            {
	            
            }
            
        }
       
    }

    material_node->first_node("");


	delete t;

}
