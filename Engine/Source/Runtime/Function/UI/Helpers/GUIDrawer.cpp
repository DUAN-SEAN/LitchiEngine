
#include "GUIDrawer.h"

#include <array>

#include "Runtime/Function/Renderer/texture2d.h"
#include "Runtime/Function/UI/Plugins/DDTarget.h"
#include "Runtime/Function/UI/Widgets/Drags/DragMultipleScalars.h"
#include "Runtime/Function/UI/Widgets/InputFields/InputText.h"
#include "Runtime/Function/UI/Widgets/Layout/Group.h"
#include "Runtime/Function/UI/Widgets/Selection/CheckBox.h"
#include "Runtime/Function/UI/Widgets/Selection/ColorEdit.h"
#include "Runtime/Function/UI/Widgets/Texts/TextColored.h"


using namespace LitchiRuntime;
const Color LitchiRuntime::GUIDrawer::TitleColor = { 0.85f, 0.65f, 0.0f };
const Color LitchiRuntime::GUIDrawer::ClearButtonColor = { 0.5f, 0.0f, 0.0f };
const float LitchiRuntime::GUIDrawer::_MIN_FLOAT = -999999999.f;
const float LitchiRuntime::GUIDrawer::_MAX_FLOAT = +999999999.f;
Texture2D* LitchiRuntime::GUIDrawer::__EMPTY_TEXTURE = nullptr;

void LitchiRuntime::GUIDrawer::ProvideEmptyTexture(Texture2D& p_emptyTexture)
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

void LitchiRuntime::GUIDrawer::DrawVec2(WidgetContainer & p_root, const std::string & p_name, glm::vec2 & p_data, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<DragMultipleScalars<float, 2>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 2>>>();
	dispatcher.RegisterReference(reinterpret_cast<std::array<float, 2>&>(p_data));
}

void LitchiRuntime::GUIDrawer::DrawVec3(WidgetContainer & p_root, const std::string & p_name, glm::vec3 & p_data, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<DragMultipleScalars<float, 3>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 3>>>();
	dispatcher.RegisterReference(reinterpret_cast<std::array<float, 3>&>(p_data));
}

void LitchiRuntime::GUIDrawer::DrawVec4(WidgetContainer & p_root, const std::string & p_name, glm::vec4& p_data, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<DragMultipleScalars<float, 4>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 4>>>();
	dispatcher.RegisterReference(reinterpret_cast<std::array<float, 4>&>(p_data));
}

void LitchiRuntime::GUIDrawer::DrawQuat(WidgetContainer & p_root, const std::string & p_name, glm::quat & p_data, float p_step, float p_min, float p_max)
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
//
//void LitchiRuntime::GUIDrawer::DrawColor(WidgetContainer & p_root, const std::string & p_name, Color & p_color, bool p_hasAlpha)
//{
//	CreateTitle(p_root, p_name);
//	auto& widget = p_root.CreateWidget<ColorEdit>(p_hasAlpha);
//	auto& dispatcher = widget.AddPlugin<DataDispatcher<Color>>();
//	dispatcher.RegisterReference(p_color);
//}
//
//Text& LitchiRuntime::GUIDrawer::DrawMesh(WidgetContainer & p_root, const std::string & p_name, OvRendering::Resources::Model *& p_data, OvTools::Eventing::Event<>* p_updateNotifier)
//{
//	CreateTitle(p_root, p_name);
//
//	std::string displayedText = (p_data ? p_data->path : std::string("Empty"));
//	auto& rightSide = p_root.CreateWidget<Group>();
//
//	auto& widget = rightSide.CreateWidget<Text>(displayedText);
//
//	widget.AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
//	{
//		if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == OvTools::Utils::PathParser::EFileType::MODEL)
//		{
//			if (auto resource = OVSERVICE(OvCore::ResourceManagement::ModelManager).GetResource(p_receivedData.first); resource)
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
//	auto& resetButton = rightSide.CreateWidget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
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
//OvUI::Widgets::Visual::Image& LitchiRuntime::GUIDrawer::DrawTexture(WidgetContainer & p_root, const std::string & p_name, OvRendering::Resources::Texture *& p_data, OvTools::Eventing::Event<>* p_updateNotifier)
//{
//	CreateTitle(p_root, p_name);
//
//	std::string displayedText = (p_data ? p_data->path : std::string("Empty"));
//	auto& rightSide = p_root.CreateWidget<Group>();
//
//	auto& widget = rightSide.CreateWidget<OvUI::Widgets::Visual::Image>(p_data ? p_data->id : (__EMPTY_TEXTURE ? __EMPTY_TEXTURE->id : 0), glm::vec2{ 75, 75 });
//
//	widget.AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
//	{
//		if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == OvTools::Utils::PathParser::EFileType::TEXTURE)
//		{
//			if (auto resource = OVSERVICE(OvCore::ResourceManagement::TextureManager).GetResource(p_receivedData.first); resource)
//			{
//				p_data = resource;
//				widget.textureID.id = resource->id;
//				if (p_updateNotifier)
//					p_updateNotifier->Invoke();
//			}
//		}
//	};
//
//	widget.lineBreak = false;
//
//	auto& resetButton = rightSide.CreateWidget<OvUI::Widgets::Buttons::Button>("Clear");
//	resetButton.idleBackgroundColor = ClearButtonColor;
//	resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
//	{
//		p_data = nullptr;
//		widget.textureID.id = (__EMPTY_TEXTURE ? __EMPTY_TEXTURE->id : 0);
//		if (p_updateNotifier)
//			p_updateNotifier->Invoke();
//	};
//
//	return widget;
//}
//
//Text& LitchiRuntime::GUIDrawer::DrawShader(WidgetContainer & p_root, const std::string & p_name, OvRendering::Resources::Shader *& p_data, OvTools::Eventing::Event<>* p_updateNotifier)
//{
//	CreateTitle(p_root, p_name);
//
//	std::string displayedText = (p_data ? p_data->path : std::string("Empty"));
//	auto& rightSide = p_root.CreateWidget<Group>();
//
//	auto& widget = rightSide.CreateWidget<Text>(displayedText);
//
//	widget.AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
//	{
//		if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == OvTools::Utils::PathParser::EFileType::SHADER)
//		{
//			if (auto resource = OVSERVICE(OvCore::ResourceManagement::ShaderManager).GetResource(p_receivedData.first); resource)
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
//	auto& resetButton = rightSide.CreateWidget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
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
//Text& LitchiRuntime::GUIDrawer::DrawMaterial(WidgetContainer & p_root, const std::string & p_name, OvCore::Resources::Material *& p_data, OvTools::Eventing::Event<>* p_updateNotifier)
//{
//	CreateTitle(p_root, p_name);
//
//	std::string displayedText = (p_data ? p_data->path : std::string("Empty"));
//	auto& rightSide = p_root.CreateWidget<Group>();
//
//	auto& widget = rightSide.CreateWidget<Text>(displayedText);
//
//	widget.AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
//	{
//		if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == OvTools::Utils::PathParser::EFileType::MATERIAL)
//		{
//			if (auto resource = OVSERVICE(OvCore::ResourceManagement::MaterialManager).GetResource(p_receivedData.first); resource)
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
//	auto& resetButton = rightSide.CreateWidget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
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
//Text& LitchiRuntime::GUIDrawer::DrawSound(WidgetContainer& p_root, const std::string& p_name, OvAudio::Resources::Sound*& p_data, OvTools::Eventing::Event<>* p_updateNotifier)
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
//		if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == OvTools::Utils::PathParser::EFileType::SOUND)
//		{
//			if (auto resource = OVSERVICE(OvCore::ResourceManagement::SoundManager).GetResource(p_receivedData.first); resource)
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
//	auto & resetButton = rightSide.CreateWidget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
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
//Text& LitchiRuntime::GUIDrawer::DrawAsset(WidgetContainer& p_root, const std::string& p_name, std::string& p_data, OvTools::Eventing::Event<>* p_updateNotifier)
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
//    auto& resetButton = rightSide.CreateWidget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
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

void LitchiRuntime::GUIDrawer::DrawVec2(WidgetContainer & p_root, const std::string & p_name, std::function<glm::vec2(void)> p_gatherer, std::function<void(glm::vec2)> p_provider, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<DragMultipleScalars<float, 2>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 2>>>();
	
	dispatcher.RegisterGatherer([p_gatherer]()
	{
		glm::vec2 value = p_gatherer();
		return reinterpret_cast<const std::array<float, 2>&>(value);
	});

	dispatcher.RegisterProvider([p_provider](std::array<float, 2> p_value)
	{
		p_provider(reinterpret_cast<glm::vec2&>(p_value));
	});
}

void LitchiRuntime::GUIDrawer::DrawVec3(WidgetContainer & p_root, const std::string & p_name, std::function<glm::vec3(void)> p_gatherer, std::function<void(glm::vec3)> p_provider, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<DragMultipleScalars<float, 3>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 3>>>();

	dispatcher.RegisterGatherer([p_gatherer]()
	{
		glm::vec3 value = p_gatherer();
		return reinterpret_cast<const std::array<float, 3>&>(value);
	});

	dispatcher.RegisterProvider([p_provider](std::array<float, 3> p_value)
	{
		p_provider(reinterpret_cast<glm::vec3&>(p_value));
	});
}

void LitchiRuntime::GUIDrawer::DrawVec4(WidgetContainer & p_root, const std::string & p_name, std::function<glm::vec4(void)> p_gatherer, std::function<void(glm::vec4)> p_provider, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<DragMultipleScalars<float, 4>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 4>>>();
	
	dispatcher.RegisterGatherer([p_gatherer]()
	{
		glm::vec4 value = p_gatherer();
		return reinterpret_cast<const std::array<float, 4>&>(value);
	});

	dispatcher.RegisterProvider([p_provider](std::array<float, 4> p_value)
	{
		p_provider(reinterpret_cast<glm::vec4&>(p_value));
	});
}

void LitchiRuntime::GUIDrawer::DrawQuat(WidgetContainer & p_root, const std::string & p_name, std::function<glm::quat(void)> p_gatherer, std::function<void(glm::quat)> p_provider, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<DragMultipleScalars<float, 4>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 4>>>();
	
	dispatcher.RegisterGatherer([p_gatherer]()
	{
		glm::quat value = p_gatherer();
		return reinterpret_cast<const std::array<float, 4>&>(value);
	});

	dispatcher.RegisterProvider([&dispatcher, p_provider](std::array<float, 4> p_value)
	{
		p_provider(glm::normalize(reinterpret_cast<glm::quat&>(p_value)));
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
