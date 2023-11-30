
#include "GUIDrawer.h"

#include <array>

#include "Runtime/Core/Global/ServiceLocator.h"
#include "Runtime/Core/Tools/Utils/PathParser.h"
#include "Runtime/Function/Renderer/Resources/Texture.h"
#include "Runtime/Function/UI/Plugins/DDTarget.h"
#include "Runtime/Function/UI/Widgets/Buttons/Button.h"
#include "Runtime/Function/UI/Widgets/Buttons/ButtonSmall.h"
#include "Runtime/Function/UI/Widgets/Drags/DragMultipleScalars.h"
#include "Runtime/Function/UI/Widgets/InputFields/InputDouble.h"
#include "Runtime/Function/UI/Widgets/InputFields/InputFloat.h"
#include "Runtime/Function/UI/Widgets/InputFields/InputInt.h"
#include "Runtime/Function/UI/Widgets/InputFields/InputInt32.h"
#include "Runtime/Function/UI/Widgets/InputFields/InputText.h"
#include "Runtime/Function/UI/Widgets/Layout/Columns.h"
#include "Runtime/Function/UI/Widgets/Layout/Group.h"
#include "Runtime/Function/UI/Widgets/Menu/MenuItem.h"
#include "Runtime/Function/UI/Widgets/Menu/MenuList.h"
#include "Runtime/Function/UI/Widgets/Selection/CheckBox.h"
#include "Runtime/Function/UI/Widgets/Selection/ColorEdit.h"
#include "Runtime/Function/UI/Widgets/Texts/TextColored.h"
#include "Runtime/Resource/MaterialManager.h"
#include "Runtime/Resource/ModelManager.h"
#include "Runtime/Resource/ShaderManager.h"
#include "Runtime/Resource/TextureManager.h"


using namespace LitchiRuntime;
const Color LitchiRuntime::GUIDrawer::TitleColor = { 0.85f, 0.65f, 0.0f };
const Color LitchiRuntime::GUIDrawer::ClearButtonColor = { 0.5f, 0.0f, 0.0f };
const float LitchiRuntime::GUIDrawer::_MIN_FLOAT = -999999999.f;
const float LitchiRuntime::GUIDrawer::_MAX_FLOAT = +999999999.f;
RHI_Texture* LitchiRuntime::GUIDrawer::__EMPTY_TEXTURE = nullptr;

void LitchiRuntime::GUIDrawer::ProvideEmptyTexture(RHI_Texture& p_emptyTexture)
{
	__EMPTY_TEXTURE = &p_emptyTexture;
}

void LitchiRuntime::GUIDrawer::CreateTitle(WidgetContainer& p_root, const std::string & p_name)
{
	p_root.CreateWidget<TextColored>(p_name, TitleColor);
}

void LitchiRuntime::GUIDrawer::DrawBoolean(WidgetContainer & p_root, const std::string & p_name, bool & p_data)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<CheckBox>();
	auto& dispatcher = widget.AddPlugin<DataDispatcher<bool>>();
	dispatcher.RegisterReference(reinterpret_cast<bool&>(p_data));
}

void LitchiRuntime::GUIDrawer::DrawVec2(WidgetContainer & p_root, const std::string & p_name, Vector2 & p_data, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<DragMultipleScalars<float, 2>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 2>>>();
	dispatcher.RegisterReference(reinterpret_cast<std::array<float, 2>&>(p_data));
}

void LitchiRuntime::GUIDrawer::DrawVec3(WidgetContainer & p_root, const std::string & p_name, Vector3 & p_data, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<DragMultipleScalars<float, 3>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 3>>>();
	dispatcher.RegisterReference(reinterpret_cast<std::array<float, 3>&>(p_data));
}

void LitchiRuntime::GUIDrawer::DrawVec4(WidgetContainer & p_root, const std::string & p_name, Vector4& p_data, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<DragMultipleScalars<float, 4>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 4>>>();
	dispatcher.RegisterReference(reinterpret_cast<std::array<float, 4>&>(p_data));
}

void LitchiRuntime::GUIDrawer::DrawQuat(WidgetContainer & p_root, const std::string & p_name, Quaternion & p_data, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<DragMultipleScalars<float, 4>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 4>>>();
	dispatcher.RegisterReference(reinterpret_cast<std::array<float, 4>&>(p_data));
}

void LitchiRuntime::GUIDrawer::DrawString(WidgetContainer & p_root, const std::string & p_name, std::string & p_data)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<InputText>("");
	auto& dispatcher = widget.AddPlugin<DataDispatcher<std::string>>();
	dispatcher.RegisterReference(p_data);
}

void LitchiRuntime::GUIDrawer::DrawColor(WidgetContainer & p_root, const std::string & p_name, Color & p_color, bool p_hasAlpha)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<ColorEdit>(p_hasAlpha);
	auto& dispatcher = widget.AddPlugin<DataDispatcher<Color>>();
	dispatcher.RegisterReference(p_color);
}

Text& LitchiRuntime::GUIDrawer::DrawMesh(WidgetContainer & p_root, const std::string & p_name, LitchiRuntime::Model *& p_data, Event<>* p_updateNotifier)
{
	// TODO:
	CreateTitle(p_root, p_name);

	std::string displayedText = (p_data ? p_data->path : std::string("Empty"));
	auto& rightSide = p_root.CreateWidget<Group>();

	auto& widget = rightSide.CreateWidget<Text>(displayedText);
	/*
	widget.AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
	{
		if (PathParser::GetFileType(p_receivedData.first) == PathParser::EFileType::MODEL)
		{
			if (auto resource = OVSERVICE(LitchiRuntime::ModelManager).GetResource(p_receivedData.first); resource)
			{
				p_data = resource;
				widget.content = p_receivedData.first;
				if (p_updateNotifier)
					p_updateNotifier->Invoke();
			}
		}
	};

	widget.lineBreak = false;

	auto& resetButton = rightSide.CreateWidget<ButtonSmall>("Clear");
	resetButton.idleBackgroundColor = ClearButtonColor;
	resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
	{
		p_data = nullptr;
		widget.content = "Empty";
		if (p_updateNotifier)
			p_updateNotifier->Invoke();
	};*/

	return widget;
}

Image& LitchiRuntime::GUIDrawer::DrawTexture(WidgetContainer & p_root, const std::string & p_name, LitchiRuntime::RHI_Texture*& p_data, Event<>* p_updateNotifier)
{
	CreateTitle(p_root, p_name);

	std::string displayedText = (p_data ? p_data->GetResourceFilePath() : std::string("Empty"));
	auto& rightSide = p_root.CreateWidget<Group>();

	auto& widget = rightSide.CreateWidget<Image>(p_data ? p_data : (__EMPTY_TEXTURE ), Vector2{ 75, 75 });

	// TODO:
	/*widget.AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
	{
		if (PathParser::GetFileType(p_receivedData.first) == PathParser::EFileType::TEXTURE)
		{
			if (auto resource = OVSERVICE(LitchiRuntime::TextureManager).GetResource(p_receivedData.first); resource)
			{
				p_data = resource;
				widget.textureID.id = resource->id;
				if (p_updateNotifier)
					p_updateNotifier->Invoke();
			}
		}
	};

	widget.lineBreak = false;

	auto& resetButton = rightSide.CreateWidget<Button>("Clear");
	resetButton.idleBackgroundColor = ClearButtonColor;
	resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
	{
		p_data = nullptr;
		widget.textureID.id = (__EMPTY_TEXTURE ? __EMPTY_TEXTURE->id : 0);
		if (p_updateNotifier)
			p_updateNotifier->Invoke();
	};*/

	

	return widget;
}

Text& LitchiRuntime::GUIDrawer::DrawShader(WidgetContainer & p_root, const std::string & p_name, MaterialShader*& p_data, Event<>* p_updateNotifier)
{
	CreateTitle(p_root, p_name);

	std::string displayedText = (p_data ? p_data->m_shaderPath: std::string("Empty"));
	auto& rightSide = p_root.CreateWidget<Group>();

	auto& widget = rightSide.CreateWidget<Text>(displayedText);

	//widget.AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
	//{
	//	if (PathParser::GetFileType(p_receivedData.first) == PathParser::EFileType::SHADER)
	//	{
	//		if (auto resource = OVSERVICE(LitchiRuntime::ShaderManager).GetResource(p_receivedData.first); resource)
	//		{
	//			p_data = resource;
	//			widget.content = p_receivedData.first;
	//			if (p_updateNotifier)
	//				p_updateNotifier->Invoke();
	//		}
	//	}
	//};

	//widget.lineBreak = false;

	//auto& resetButton = rightSide.CreateWidget<ButtonSmall>("Clear");
	//resetButton.idleBackgroundColor = ClearButtonColor;
	//resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
	//{
	//	p_data = nullptr;
	//	widget.content = "Empty";
	//	if (p_updateNotifier)
	//		p_updateNotifier->Invoke();
	//};

	return widget;
}

Text& LitchiRuntime::GUIDrawer::DrawMaterial(WidgetContainer & p_root, const std::string & p_name, Material *& p_data, Event<>* p_updateNotifier)
{
	CreateTitle(p_root, p_name);

	std::string displayedText = (p_data ? p_data->GetResourceFilePath() : std::string("Empty"));
	auto& rightSide = p_root.CreateWidget<Group>();

	auto& widget = rightSide.CreateWidget<Text>(displayedText);

	// todo:
	/*widget.AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
	{
		if (PathParser::GetFileType(p_receivedData.first) == PathParser::EFileType::MATERIAL)
		{
			if (auto resource = OVSERVICE(LitchiRuntime::MaterialManager).GetResource(p_receivedData.first); resource)
			{
				p_data = resource;
				widget.content = p_receivedData.first;
				if (p_updateNotifier)
					p_updateNotifier->Invoke();
			}
		}
	};*/

	widget.lineBreak = false;

	auto& resetButton = rightSide.CreateWidget<ButtonSmall>("Clear");
	resetButton.idleBackgroundColor = ClearButtonColor;
	resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
	{
		p_data = nullptr;
		widget.content = "Empty";
		if (p_updateNotifier)
			p_updateNotifier->Invoke();
	};

	return widget;
}

//Text& LitchiRuntime::GUIDrawer::DrawSound(WidgetContainer& p_root, const std::string& p_name, OvAudio::LitchiRuntime::Sound*& p_data, Event<>* p_updateNotifier)
//{
//	CreateTitle(p_root, p_name);
//
//	std::string displayedText = (p_data ? p_data->path : std::string("Empty"));
//	auto & rightSide = p_root.CreateWidget<Group>();
//
//	auto & widget = rightSide.CreateWidget<Text>(displayedText);
//
//	widget.AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
//	{
//		if (PathParser::GetFileType(p_receivedData.first) == PathParser::EFileType::SOUND)
//		{
//			if (auto resource = OVSERVICE(LitchiRuntime::SoundManager).GetResource(p_receivedData.first); resource)
//			{
//				p_data = resource;
//				widget.content = p_receivedData.first;
//				if (p_updateNotifier)
//					p_updateNotifier->Invoke();
//			}
//		}
//	};
//
//	widget.lineBreak = false;
//
//	auto & resetButton = rightSide.CreateWidget<ButtonSmall>("Clear");
//	resetButton.idleBackgroundColor = ClearButtonColor;
//	resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
//	{
//		p_data = nullptr;
//		widget.content = "Empty";
//		if (p_updateNotifier)
//			p_updateNotifier->Invoke();
//	};
//
//	return widget;
//}
//
//Text& LitchiRuntime::GUIDrawer::DrawAsset(WidgetContainer& p_root, const std::string& p_name, std::string& p_data, Event<>* p_updateNotifier)
//{
//    CreateTitle(p_root, p_name);
//
//    const std::string displayedText = (p_data.empty() ? std::string("Empty") : p_data);
//    auto& rightSide = p_root.CreateWidget<Group>();
//
//    auto& widget = rightSide.CreateWidget<Text>(displayedText);
//
//    widget.AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
//    {
//        p_data = p_receivedData.first;
//        widget.content = p_receivedData.first;
//        if (p_updateNotifier)
//            p_updateNotifier->Invoke();
//    };
//
//    widget.lineBreak = false;
//
//    auto& resetButton = rightSide.CreateWidget<ButtonSmall>("Clear");
//    resetButton.idleBackgroundColor = ClearButtonColor;
//    resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
//    {
//        p_data = "";
//        widget.content = "Empty";
//        if (p_updateNotifier)
//            p_updateNotifier->Invoke();
//    };
//
//    return widget;
//}

void LitchiRuntime::GUIDrawer::DrawBoolean(WidgetContainer & p_root, const std::string & p_name, std::function<bool(void)> p_gatherer, std::function<void(bool)> p_provider)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<CheckBox>();
	auto& dispatcher = widget.AddPlugin<DataDispatcher<bool>>();

	dispatcher.RegisterGatherer([p_gatherer]()
	{
		bool value = p_gatherer();
		return reinterpret_cast<bool&>(value);
	});

	dispatcher.RegisterProvider([p_provider](bool p_value)
	{
		p_provider(reinterpret_cast<bool&>(p_value));
	});
}

void LitchiRuntime::GUIDrawer::DrawVec2(WidgetContainer & p_root, const std::string & p_name, std::function<Vector2(void)> p_gatherer, std::function<void(Vector2)> p_provider, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<DragMultipleScalars<float, 2>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 2>>>();
	
	dispatcher.RegisterGatherer([p_gatherer]()
	{
		Vector2 value = p_gatherer();
		return reinterpret_cast<const std::array<float, 2>&>(value);
	});

	dispatcher.RegisterProvider([p_provider](std::array<float, 2> p_value)
	{
		p_provider(reinterpret_cast<Vector2&>(p_value));
	});
}

void LitchiRuntime::GUIDrawer::DrawVec3(WidgetContainer & p_root, const std::string & p_name, std::function<Vector3(void)> p_gatherer, std::function<void(Vector3)> p_provider, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<DragMultipleScalars<float, 3>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 3>>>();

	dispatcher.RegisterGatherer([p_gatherer]()
	{
		Vector3 value = p_gatherer();
		return reinterpret_cast<const std::array<float, 3>&>(value);
	});

	dispatcher.RegisterProvider([p_provider](std::array<float, 3> p_value)
	{
		p_provider(reinterpret_cast<Vector3&>(p_value));
	});
}

void LitchiRuntime::GUIDrawer::DrawVec4(WidgetContainer & p_root, const std::string & p_name, std::function<Vector4(void)> p_gatherer, std::function<void(Vector4)> p_provider, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<DragMultipleScalars<float, 4>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 4>>>();
	
	dispatcher.RegisterGatherer([p_gatherer]()
	{
		Vector4 value = p_gatherer();
		return reinterpret_cast<const std::array<float, 4>&>(value);
	});

	dispatcher.RegisterProvider([p_provider](std::array<float, 4> p_value)
	{
		p_provider(reinterpret_cast<Vector4&>(p_value));
	});
}

void LitchiRuntime::GUIDrawer::DrawQuat(WidgetContainer & p_root, const std::string & p_name, std::function<Quaternion(void)> p_gatherer, std::function<void(Quaternion)> p_provider, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<DragMultipleScalars<float, 4>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 4>>>();
	
	dispatcher.RegisterGatherer([p_gatherer]()
	{
		Quaternion value = p_gatherer();
		return reinterpret_cast<const std::array<float, 4>&>(value);
	});

	dispatcher.RegisterProvider([&dispatcher, p_provider](std::array<float, 4> p_value)
	{
		auto& rotation = reinterpret_cast<Quaternion&>(p_value);
		rotation.Normalize();
		p_provider(rotation);
	});
}

void LitchiRuntime::GUIDrawer::DrawDDString(WidgetContainer& p_root, const std::string& p_name,
	std::function<std::string()> p_gatherer, std::function<void(std::string)> p_provider,
	const std::string& p_identifier)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<InputText>("");
	auto& dispatcher = widget.AddPlugin<DataDispatcher<std::string>>();
	dispatcher.RegisterGatherer(p_gatherer);
	dispatcher.RegisterProvider(p_provider);

	auto& ddTarget = widget.AddPlugin<DDTarget<std::pair<std::string, Group*>>>(p_identifier);
	ddTarget.DataReceivedEvent += [&widget, &dispatcher](std::pair<std::string, Group*> p_data)
	{
		widget.content = p_data.first;
		dispatcher.NotifyChange();
	};
}

void LitchiRuntime::GUIDrawer::DrawString(WidgetContainer & p_root, const std::string & p_name, std::function<std::string(void)> p_gatherer, std::function<void(std::string)> p_provider)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<InputText>("");
	auto& dispatcher = widget.AddPlugin<DataDispatcher<std::string>>();
	dispatcher.RegisterGatherer(p_gatherer);
	dispatcher.RegisterProvider(p_provider);
}

void LitchiRuntime::GUIDrawer::DrawColor(WidgetContainer & p_root, const std::string & p_name, std::function<Color(void)> p_gatherer, std::function<void(Color)> p_provider, bool p_hasAlpha)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<ColorEdit>(p_hasAlpha);
	auto& dispatcher = widget.AddPlugin<DataDispatcher<Color>>();
	dispatcher.RegisterGatherer(p_gatherer);
	dispatcher.RegisterProvider(p_provider);
}
void LitchiRuntime::GUIDrawer::DrawInputField4Int(WidgetContainer& p_root, const std::string& p_name, std::function<int(void)> p_gatherer, std::function<void(int)> p_provider)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<InputInt>();
	auto& dispatcher = widget.AddPlugin<DataDispatcher<int>>();
	dispatcher.RegisterGatherer(p_gatherer);
	dispatcher.RegisterProvider(p_provider);
}

void LitchiRuntime::GUIDrawer::DrawInputField4Int32(WidgetContainer& p_root, const std::string& p_name, std::function<int32_t(void)> p_gatherer, std::function<void(int32_t)> p_provider)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<InputInt32>();
	auto& dispatcher = widget.AddPlugin<DataDispatcher<int32_t>>();
	dispatcher.RegisterGatherer(p_gatherer);
	dispatcher.RegisterProvider(p_provider);
}

void LitchiRuntime::GUIDrawer::DrawInputField4UInt64(WidgetContainer& p_root, const std::string& p_name, std::function<uint64_t(void)> p_gatherer, std::function<void(uint64_t)> p_provider)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<InputInt32>();
	auto& dispatcher = widget.AddPlugin<DataDispatcher<int32_t>>();
	dispatcher.RegisterGatherer(p_gatherer);
	dispatcher.RegisterProvider(p_provider);
}

void LitchiRuntime::GUIDrawer::DrawInputField4Float(WidgetContainer& p_root, const std::string& p_name, std::function<float(void)> p_gatherer, std::function<void(float)> p_provider)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<InputFloat>();
	auto& dispatcher = widget.AddPlugin<DataDispatcher<float>>();
	dispatcher.RegisterGatherer(p_gatherer);
	dispatcher.RegisterProvider(p_provider);
}

void LitchiRuntime::GUIDrawer::DrawInputField4Double(WidgetContainer& p_root, const std::string& p_name, std::function<double(void)> p_gatherer, std::function<void(double)> p_provider)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<InputDouble>();
	auto& dispatcher = widget.AddPlugin<DataDispatcher<double>>();
	dispatcher.RegisterGatherer(p_gatherer);
	dispatcher.RegisterProvider(p_provider);
}

void LitchiRuntime::GUIDrawer::DrawEnum(WidgetContainer& p_root, const std::string& p_name, std::vector<std::string> enumValueList, std::function<std::string(void)> p_gatherer, std::function<void(std::string)> p_provider)
{
	CreateTitle(p_root, p_name);
	auto currName = p_gatherer();
	auto& widget = p_root.CreateWidget<MenuList>(currName);
	for (auto enumName : enumValueList)
	{
		bool isSelect = enumName == currName;
		auto& menuItem = widget.CreateWidget<MenuItem>(enumName,"",false,isSelect);
	/*	auto& dispatcher = menuItem.AddPlugin<DataDispatcher<std::string>>();
		dispatcher.RegisterGatherer(p_gatherer);
		dispatcher.RegisterProvider(p_provider);*/
		menuItem.ClickedEvent += [&widget,p_provider, enumName]
		{
			widget.name = enumName;
			p_provider(enumName);
		};
		//auto& dispatcher = widget.AddPlugin<DataDispatcher<std::string>>();
		//dispatcher.RegisterGatherer(p_gatherer);
		//dispatcher.RegisterProvider(p_provider);
	}

}
