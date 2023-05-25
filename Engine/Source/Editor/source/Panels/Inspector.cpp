
#include <map>
#include <string>


#include "Editor/include/Panels/Inspector.h"

#include "Runtime/Core/Meta/Reflection/propery_field.h"
#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/Component/Physcis/collider.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_renderer.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_filter.h"
#include "Runtime/Function/Framework/Component/Physcis/collider.h"
#include "Runtime/Function/Framework/Component/Physcis/rigid_actor.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"
#include "Runtime/Function/UI/Helpers/GUIDrawer.h"
#include "Runtime/Function/UI/Plugins/DDTarget.h"
#include "Runtime/Function/UI/Settings/PanelWindowSettings.h"
#include "Runtime/Function/UI/Widgets/Buttons/Button.h"
#include "Runtime/Function/UI/Widgets/InputFields/InputFloat.h"
#include "Runtime/Function/UI/Widgets/Layout/Columns.h"
#include "Runtime/Function/UI/Widgets/Layout/Dummy.h"
#include "Runtime/Function/UI/Widgets/Layout/GroupCollapsable.h"
#include "Runtime/Function/UI/Widgets/Visual/Separator.h"

LitchiEditor::Inspector::Inspector
(
	const std::string& p_title,
	bool p_opened,
	const LitchiRuntime::PanelWindowSettings& p_windowSettings
) : PanelWindow(p_title, p_opened, p_windowSettings)
{
	m_inspectorHeader = &CreateWidget<Group>();
	m_inspectorHeader->enabled = false;
	m_actorInfo = &CreateWidget<Group>();

	auto& headerColumns = m_inspectorHeader->CreateWidget<Columns<2>>();

	/* Name field */
	auto nameGatherer = [this] { return m_targetActor ? m_targetActor->name() : "%undef%"; };
	auto nameProvider = [this](const std::string& p_newName) { if (m_targetActor) m_targetActor->set_name(p_newName); };
	GUIDrawer::DrawString(headerColumns, "Name", nameGatherer, nameProvider);

	/* Active field */
	auto activeGatherer = [this] { return m_targetActor ? m_targetActor->active() : false; };
	auto activeProvider = [this](bool p_active) { if (m_targetActor) m_targetActor->set_active(p_active); };
	GUIDrawer::DrawBoolean(headerColumns, "Active", activeGatherer, activeProvider);

	/* Component select + button */
	{
		auto& componentSelectorWidget = m_inspectorHeader->CreateWidget<ComboBox>(0);
		componentSelectorWidget.lineBreak = false;
		componentSelectorWidget.choices.emplace(0, "MeshRenderer");
		componentSelectorWidget.choices.emplace(1, "MeshFilter");
		componentSelectorWidget.choices.emplace(2, "Camera");
		componentSelectorWidget.choices.emplace(3, "Collider");
		componentSelectorWidget.choices.emplace(4, "RigidActor");

		auto& addComponentButton = m_inspectorHeader->CreateWidget<Button>("Add Component", glm::vec2{ 100.f, 0 });
		addComponentButton.idleBackgroundColor = Color{ 0.7f, 0.5f, 0.f };
		addComponentButton.textColor = Color::White;
		addComponentButton.ClickedEvent += [&componentSelectorWidget, this]
		{
			switch (componentSelectorWidget.currentChoice)
			{
			case 0: GetTargetActor()->AddComponent<MeshRenderer>(); break;
			case 1: GetTargetActor()->AddComponent<MeshFilter>(); break;
			case 3: GetTargetActor()->AddComponent<Camera>();				break;
			case 4: GetTargetActor()->AddComponent<Collider>();				break;
			case 5: GetTargetActor()->AddComponent<RigidActor>();				break;
			}

			componentSelectorWidget.ValueChangedEvent.Invoke(componentSelectorWidget.currentChoice);
		};

		componentSelectorWidget.ValueChangedEvent += [this, &addComponentButton](int p_value)
		{
			auto defineButtonsStates = [&addComponentButton](bool p_componentExists)
			{
				addComponentButton.disabled = p_componentExists;
				addComponentButton.idleBackgroundColor = !p_componentExists ? Color{ 0.7f, 0.5f, 0.f } : Color{ 0.1f, 0.1f, 0.1f };
			};

			switch (p_value)
			{
			case 0: defineButtonsStates(GetTargetActor()->GetComponent<MeshRenderer>());		return;
			case 1: defineButtonsStates(GetTargetActor()->GetComponent<MeshFilter>());				return;
			case 2: defineButtonsStates(GetTargetActor()->GetComponent<Camera>());		return;
			case 3: defineButtonsStates(GetTargetActor()->GetComponent<Collider>());		return;
			case 4: defineButtonsStates(GetTargetActor()->GetComponent<RigidActor>());		return;
			}
		};

		m_componentSelectorWidget = &componentSelectorWidget;
	}

	/* Script selector + button */
	{
		m_scriptSelectorWidget = &m_inspectorHeader->CreateWidget<InputText>("");
		m_scriptSelectorWidget->lineBreak = false;
		auto& ddTarget = m_scriptSelectorWidget->AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File");

		auto& addScriptButton = m_inspectorHeader->CreateWidget<Button>("Add Script", glm::vec2{ 100.f, 0 });
		addScriptButton.idleBackgroundColor = Color{ 0.7f, 0.5f, 0.f };
		addScriptButton.textColor = Color::White;

		// Add script button state updater
  //      const auto updateAddScriptButton = [&addScriptButton, this](const std::string& p_script)
  //      {
  //          const std::string realScriptPath = EDITOR_CONTEXT(projectScriptsPath) + p_script + ".lua";

  //          const auto targetActor = GetTargetActor();
  //          const bool isScriptValid = std::filesystem::exists(realScriptPath) && targetActor && !targetActor->GetBehaviour(p_script);

  //          addScriptButton.disabled = !isScriptValid;
  //          addScriptButton.idleBackgroundColor = isScriptValid ? OvUI::Types::Color{ 0.7f, 0.5f, 0.f } : OvUI::Types::Color{ 0.1f, 0.1f, 0.1f };
  //      };

  //      m_scriptSelectorWidget->ContentChangedEvent += updateAddScriptButton;

		//addScriptButton.ClickedEvent += [updateAddScriptButton, this]
		//{
  //          const std::string realScriptPath = EDITOR_CONTEXT(projectScriptsPath) + m_scriptSelectorWidget->content + ".lua";

  //          // Ensure that the script is a valid one
  //          if (std::filesystem::exists(realScriptPath))
  //          {
  //              GetTargetActor()->AddBehaviour(m_scriptSelectorWidget->content);
  //              updateAddScriptButton(m_scriptSelectorWidget->content);
  //          }
		//};

  //      ddTarget.DataReceivedEvent += [updateAddScriptButton, this](std::pair<std::string, Group*> p_data)
  //      {
  //          m_scriptSelectorWidget->content = EDITOR_EXEC(GetScriptPath(p_data.first));
  //          updateAddScriptButton(m_scriptSelectorWidget->content);
  //      };
	}

	m_inspectorHeader->CreateWidget<Separator>();

	/*m_destroyedListener = GameObject::DestroyedEvent += [this](GameObject* p_destroyed)
	{
		if (&p_destroyed == m_targetActor)
			UnFocus();
	};*/
}

LitchiEditor::Inspector::~Inspector()
{
	// GameObject::DestroyedEvent -= m_destroyedListener;

	UnFocus();
}

void LitchiEditor::Inspector::FocusActor(GameObject* p_target)
{
	if (m_targetActor)
		UnFocus();

	m_actorInfo->RemoveAllWidgets();

	m_targetActor = p_target;

	/*m_componentAddedListener = m_targetActor->ComponentAddedEvent += [this] (auto& useless) { EDITOR_EXEC(DelayAction([this] { Refresh(); })); };
	m_behaviourAddedListener =		m_targetActor->BehaviourAddedEvent += [this](auto& useless) { EDITOR_EXEC(DelayAction([this] { Refresh(); })); };
	m_componentRemovedListener =	m_targetActor->ComponentRemovedEvent += [this](auto& useless) { EDITOR_EXEC(DelayAction([this] { Refresh(); })); };
	m_behaviourRemovedListener =	m_targetActor->BehaviourRemovedEvent += [this](auto& useless) { EDITOR_EXEC(DelayAction([this] { Refresh(); })); };*/

	m_inspectorHeader->enabled = true;

	CreateActorInspector(p_target);

	// Force component and script selectors to trigger their ChangedEvent to update button states
	m_componentSelectorWidget->ValueChangedEvent.Invoke(m_componentSelectorWidget->currentChoice);
	m_scriptSelectorWidget->ContentChangedEvent.Invoke(m_scriptSelectorWidget->content);

	// EDITOR_EVENT(ActorSelectedEvent).Invoke(*m_targetActor);
}

void LitchiEditor::Inspector::UnFocus()
{
	if (m_targetActor)
	{
		/*m_targetActor->ComponentAddedEvent		-= m_componentAddedListener;
		m_targetActor->ComponentRemovedEvent	-= m_componentRemovedListener;
		m_targetActor->BehaviourAddedEvent		-= m_behaviourAddedListener;
		m_targetActor->BehaviourRemovedEvent	-= m_behaviourRemovedListener;*/
	}

	SoftUnFocus();
}

void LitchiEditor::Inspector::SoftUnFocus()
{
	if (m_targetActor)
	{
		// EDITOR_EVENT(ActorUnselectedEvent).Invoke(*m_targetActor);
		m_inspectorHeader->enabled = false;
		m_targetActor = nullptr;
		m_actorInfo->RemoveAllWidgets();
	}
}

GameObject* LitchiEditor::Inspector::GetTargetActor() const
{
	return m_targetActor;
}

void LitchiEditor::Inspector::CreateActorInspector(GameObject* p_target)
{
	auto& components = p_target->GetComponents();

	for (auto component : components)
	{
		auto name = component->get_type().get_name().to_string();
		DrawComponent(name, component);
	}
		

	/*auto& behaviours = p_target.GetBehaviours();

	for (auto&[name, behaviour] : behaviours)
		DrawBehaviour(behaviour);*/
}

static void DrawInstanceInternalRecursively(WidgetContainer& p_root, const instance& inputIns, Object* obj, std::vector<string> propertyPathList);

static bool DrawProperty(WidgetContainer& p_root, const variant& var, const string_view propertyName, Object* obj, std::vector<string> propertyPathList);

static bool DrawAtomicTypeObject(WidgetContainer& p_root, const type& t, const variant& var, const string_view propertyName, Object* obj, std::vector<string> propertyPathList)
{
	/*auto& col = p_root.CreateWidget<Columns<2>>();
	col.widths[0] = 10.0f;
	auto& dumy = col.CreateWidget<Dummy>(glm::vec2(0.0f, 5.0f));
	auto& propertyRoot = col.CreateWidget<GroupCollapsable>(propertyName.to_string());*/

	auto& propertyRoot = p_root;

	PropertyField property_field(obj, propertyPathList);
	if (t.is_arithmetic())
	{
		if (t == type::get<bool>() || t == type::get<char>())
		{
			auto getBool = [var, property_field]
			{
				return property_field.GetValue().to_bool();
			};

			auto setBool = [property_field](bool value)
			{
				property_field.SetValue(value);
			};

			GUIDrawer::DrawBoolean(propertyRoot, propertyName.to_string(), getBool, setBool);
		}
		/*else if (t == type::get<int8_t>())
		{
			writer.Int(var.to_int8());
		}
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
			writer.Uint64(var.to_uint64());*/
		else if (t == type::get<float>())
		{
			auto getFloat = [var, property_field]
			{
				return property_field.GetValue().to_float();
			};

			auto setFloat = [property_field](float value)
			{
				property_field.SetValue(value);
			};
			
			GUIDrawer::DrawInputField4Float(propertyRoot, propertyName.to_string(), getFloat, setFloat);
		}
		else if (t == type::get<double>())
		{
			auto getDouble = [var, property_field]
			{
				return property_field.GetValue().to_double();
			};

			auto setDouble = [property_field](double value)
			{
				property_field.SetValue(value);
			};
			
			GUIDrawer::DrawInputField4Double(propertyRoot, propertyName.to_string(), getDouble, setDouble);
		}

		return true;
	}
	else if (t.is_enumeration())
	{
		bool ok = false;
		auto result = var.to_string(&ok);
		if (ok)
		{
			auto getString = [var, property_field]
			{
				return property_field.GetValue().to_string();
			};

			auto setString = [property_field](std::string value)
			{
				property_field.SetValue(value);
			};

			GUIDrawer::DrawString(propertyRoot, propertyName.to_string(), getString, setString);
		}
		else
		{
			/*ok = false;
			auto value = var.to_uint64(&ok);
			if (ok)
				writer.Uint64(value);
			else
				writer.Null();*/
		}

		return true;
	}
	else if (t == type::get<std::string>())
	{
		auto getString = [var,property_field]
		{
			return property_field.GetValue().to_string();
		};

		auto setString = [property_field](std::string value)
		{
			property_field.SetValue(value);
		};

		GUIDrawer::DrawString(p_root, propertyName.to_string(), getString, setString);
		return true;
	}

	return false;
}

//static void DrawArray(WidgetContainer& p_root, const variant_sequential_view& view, const string_view propertyName)
//{
//	for (const auto& item : view)
//	{
//		if (item.is_sequential_container())
//		{
//			DrawArray(item.create_sequential_view(), propertyName);
//		}
//		else
//		{
//			variant wrapped_var = item.extract_wrapped_value();
//			type value_type = wrapped_var.get_type();
//			if (value_type.is_arithmetic() || value_type == type::get<std::string>() || value_type.is_enumeration())
//			{
//				DrawAtomicTypeObject(value_type, wrapped_var, propertyName);
//			}
//			else // object
//			{
//				DrawInstanceInternalRecursively(wrapped_var, propertyName);
//			}
//		}
//	}
//}

//
//static void DrawAssociativeContainer(WidgetContainer& p_root, const variant_associative_view& view, const string_view propertyName)
//{
//	static const string_view key_name("key");
//	static const string_view value_name("value");
//
//	writer.StartArray();
//
//	if (view.is_key_only_type())
//	{
//		for (auto& item : view)
//		{
//			DrawProperty(item.first, writer);
//		}
//	}
//	else
//	{
//		for (auto& item : view)
//		{
//			writer.StartObject();
//			writer.String(key_name.data(), static_cast<rapidjson::SizeType>(key_name.length()), false);
//
//			DrawProperty(item.first, writer);
//
//			writer.String(value_name.data(), static_cast<rapidjson::SizeType>(value_name.length()), false);
//
//			DrawProperty(item.second, writer);
//
//			writer.EndObject();
//		}
//	}
//
//	writer.EndArray();
//}

static bool DrawProperty(WidgetContainer& p_root, const variant& var, const string_view propertyName, Object* obj, std::vector<string> propertyPathList)
{
	auto value_type = var.get_type();
	auto wrapped_type = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;
	bool is_wrapper = wrapped_type != value_type;

	if (DrawAtomicTypeObject(p_root, is_wrapper ? wrapped_type : value_type,
		is_wrapper ? var.extract_wrapped_value() : var, propertyName, obj, propertyPathList))
	{
	}
	else if (var.is_sequential_container())
	{
		// DrawArray(p_root, var.create_sequential_view(), propertyName);
	}
	/*else if (var.is_associative_container())
	{
		DrawAssociativeContainer(p_root,var.create_associative_view(), propertyName);
	}*/
	else
	{
		auto child_props = is_wrapper ? wrapped_type.get_properties() : value_type.get_properties();

		// x 垂直, y 水平
		auto& col = p_root.CreateWidget<Columns<2>>();
		col.widths[0] = 75.0f;
		auto& text = col.CreateWidget<Text>(propertyName.to_string());
		auto& propertyRoot = col.CreateWidget<Group>();
		if (!child_props.empty())
		{
			DrawInstanceInternalRecursively(propertyRoot, var, obj, propertyPathList);
		}
		else
		{
			/*bool ok = false;
			auto text = var.to_string(&ok);
			if (!ok)
			{
				writer.String(text);
				return false;
			}

			writer.String(text);*/
		}
	}

	return true;
}

static void DrawInstanceInternalRecursively(WidgetContainer& p_root, const instance& inputIns, Object* obj, std::vector<string> propertyPathList)
{
	instance ins = inputIns.get_type().get_raw_type().is_wrapper() ? inputIns.get_wrapped_instance() : inputIns;

	auto prop_list = ins.get_derived_type().get_properties();

	for (auto prop : prop_list)
	{
		if (prop.get_metadata("NO_SERIALIZE"))
			continue;

		variant prop_value = prop.get_value(ins);
		if (!prop_value)
			continue; // cannot serialize, because we cannot retrieve the value

		const auto name = prop.get_name();

		// auto propertyObj = prop.get_value(obj);
		// writer.String(name.data(), static_cast<rapidjson::SizeType>(name.length()), false);

		propertyPathList.push_back(name.to_string());
		if (!DrawProperty(p_root, prop_value, name, obj, propertyPathList))
		{
			DEBUG_LOG_ERROR("cannot serialize property:{}", name.to_string());
		}
		propertyPathList.pop_back();
	}

}

void DrawInstance(WidgetContainer& p_root, rttr::instance ins, Object* obj)
{
	if (!ins.is_valid())
		return;

	std::vector<string> propertyPathList;
	DrawInstanceInternalRecursively(p_root, ins, obj, propertyPathList);
}

void LitchiEditor::Inspector::DrawComponent(std::string name, Component* p_component)
{
	// 反射读取component的所有字段
	auto& header = m_actorInfo->CreateWidget<GroupCollapsable>(name);
	header.closable = !dynamic_cast<Transform*>(p_component);
	header.CloseEvent += [this, &header, p_component]
	{
		if (p_component->game_object()->RemoveComponent(p_component))
			m_componentSelectorWidget->ValueChangedEvent.Invoke(m_componentSelectorWidget->currentChoice);
	};
	auto& columns = header.CreateWidget<Columns<2>>();
	columns.widths[0] = 200;

	DrawInstance(header, *p_component, p_component);
}

//void LitchiEditor::Inspector::DrawBehaviour(OvCore::ECS::Components::Behaviour & p_behaviour)
//{
//	if (auto inspectorItem = dynamic_cast<OvCore::API::IInspectorItem*>(&p_behaviour); inspectorItem)
//	{
//		auto& header = m_actorInfo->CreateWidget<GroupCollapsable>(p_behaviour.name);
//		header.closable = true;
//		header.CloseEvent += [this, &header, &p_behaviour]
//		{
//			p_behaviour.owner.RemoveBehaviour(p_behaviour);
//		};
//
//		auto& columns = header.CreateWidget<Columns<2>>();
//		columns.widths[0] = 200;
//		inspectorItem->OnInspector(columns);
//	}
//}

void LitchiEditor::Inspector::Refresh()
{
	if (m_targetActor)
	{
		m_actorInfo->RemoveAllWidgets();
		CreateActorInspector(m_targetActor);
	}
}
