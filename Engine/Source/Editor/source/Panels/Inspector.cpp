
#include "Runtime/Core/pch.h"

#include "Editor/include/Panels/Inspector.h"

#include "Runtime/Core/Meta/Reflection/ProperyField.h"
#include "Runtime/Core/Tools/Utils/PathParser.h"
#include "Runtime/Function/Framework/Component/Animation/animator.h"
#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/Component/Gameplay/ThirdPersonController.h"
#include "Runtime/Function/Framework/Component/Light/Light.h"
#include "Runtime/Function/Framework/Component/Physcis/BoxCollider.h"
#include "Runtime/Function/Framework/Component/Physcis/CapsuleCollider.h"
#include "Runtime/Function/Framework/Component/Physcis/CharacterController.h"
#include "Runtime/Function/Framework/Component/Physcis/collider.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshFilter.h"
#include "Runtime/Function/Framework/Component/Physcis/collider.h"
#include "Runtime/Function/Framework/Component/Physcis/RigidActor.h"
#include "Runtime/Function/Framework/Component/Physcis/RigidDynamic.h"
#include "Runtime/Function/Framework/Component/Physcis/RigidStatic.h"
#include "Runtime/Function/Framework/Component/Physcis/SphereCollider.h"
#include "Runtime/Function/Framework/Component/Renderer/SkinnedMeshRenderer.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Framework/Component/UI/UIImage.h"
#include "Runtime/Function/Framework/Component/UI/UIText.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/UI/Helpers/GUIDrawer.h"
#include "Runtime/Function/UI/Plugins/DDTarget.h"
#include "Runtime/Function/UI/Settings/PanelWindowSettings.h"
#include "Runtime/Function/UI/Widgets/Buttons/Button.h"
#include "Runtime/Function/UI/Widgets/InputFields/InputFloat.h"
#include "Runtime/Function/UI/Widgets/Layout/Columns.h"
#include "Runtime/Function/UI/Widgets/Layout/Dummy.h"
#include "Runtime/Function/UI/Widgets/Layout/GroupCollapsable.h"
#include "Runtime/Function/UI/Widgets/Layout/TreeNode.h"
#include "Runtime/Function/UI/Widgets/Texts/TextColored.h"
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
	auto nameGatherer = [this] { return m_targetActor ? m_targetActor->GetName() : "%undef%"; };
	auto nameProvider = [this](const std::string& p_newName) { if (m_targetActor) m_targetActor->SetName(p_newName); };
	GUIDrawer::DrawString(headerColumns, "Name", nameGatherer, nameProvider);

	/* Active field */
	auto activeGatherer = [this] { return m_targetActor ? m_targetActor->GetActive() : false; };
	auto activeProvider = [this](bool p_active) { if (m_targetActor) m_targetActor->SetActive(p_active); };
	GUIDrawer::DrawBoolean(headerColumns, "Active", activeGatherer, activeProvider);

	/* Component select + button */
	{
		auto& componentSelectorWidget = m_inspectorHeader->CreateWidget<ComboBox>(0);
		componentSelectorWidget.lineBreak = false;
		componentSelectorWidget.choices.emplace(0, "MeshRenderer");
		componentSelectorWidget.choices.emplace(1, "MeshFilter");
		componentSelectorWidget.choices.emplace(2, "Camera");
		componentSelectorWidget.choices.emplace(3, "SkinnedMeshRenderer");
		componentSelectorWidget.choices.emplace(5, "Light");
		componentSelectorWidget.choices.emplace(8, "Animator");
		componentSelectorWidget.choices.emplace(9, "UIText");
		componentSelectorWidget.choices.emplace(10, "UIImage");
		componentSelectorWidget.choices.emplace(11, "RigidStatic");
		componentSelectorWidget.choices.emplace(12, "RigidDynamic");
		componentSelectorWidget.choices.emplace(13, "BoxCollider");
		componentSelectorWidget.choices.emplace(14, "SphereCollider");
		componentSelectorWidget.choices.emplace(15, "CapsuleCollider");
		componentSelectorWidget.choices.emplace(16, "CharacterController");
		componentSelectorWidget.choices.emplace(17, "ThirdPersonController");

		auto& addComponentButton = m_inspectorHeader->CreateWidget<Button>("Add Component", Vector2(100.f, 0.0f));
		addComponentButton.idleBackgroundColor = Color{ 0.7f, 0.5f, 0.f };
		addComponentButton.textColor = Color::White;
		addComponentButton.ClickedEvent += [&componentSelectorWidget, this]
			{
				switch (componentSelectorWidget.currentChoice)
				{
				case 0: GetTargetActor()->AddComponent<MeshRenderer>(); break;
				case 1: GetTargetActor()->AddComponent<MeshFilter>(); break;
				case 2: GetTargetActor()->AddComponent<Camera>();				break;
				case 3: GetTargetActor()->AddComponent<SkinnedMeshRenderer>();				break;
				case 5: GetTargetActor()->AddComponent<Light>();				break;
				case 8: GetTargetActor()->AddComponent<Animator>();				break;
				case 9: GetTargetActor()->AddComponent<UIText>();				break;
				case 10: GetTargetActor()->AddComponent<UIImage>();				break;
				case 11: GetTargetActor()->AddComponent<RigidStatic>();				break;
				case 12: GetTargetActor()->AddComponent<RigidDynamic>();				break;
				case 13: GetTargetActor()->AddComponent<BoxCollider>();				break;
				case 14: GetTargetActor()->AddComponent<SphereCollider>();				break;
				case 15: GetTargetActor()->AddComponent<CapsuleCollider>();				break;
				case 16: GetTargetActor()->AddComponent<CharacterController>();				break;
				case 17: GetTargetActor()->AddComponent<ThirdPersonController>();				break;
				}

				Refresh();

				componentSelectorWidget.ValueChangedEvent.Invoke(componentSelectorWidget.currentChoice);
			};

		componentSelectorWidget.ValueChangedEvent += [this, &addComponentButton](int p_value)
			{
				auto defineButtonsStates = [&addComponentButton](bool p_componentExists)
					{
						addComponentButton.disabled = p_componentExists;
						addComponentButton.idleBackgroundColor = !p_componentExists ? Color{ 0.7f, 0.5f, 0.f } : Color{ 0.1f, 0.1f, 0.1f };
					};

				if (GetTargetActor() == nullptr) { return; }
				switch (p_value)
				{
				case 0: defineButtonsStates(GetTargetActor()->GetComponent<MeshRenderer>());		return;
				case 1: defineButtonsStates(GetTargetActor()->GetComponent<MeshFilter>());				return;
				case 2: defineButtonsStates(GetTargetActor()->GetComponent<Camera>());		return;
				case 3: defineButtonsStates(GetTargetActor()->GetComponent<SkinnedMeshRenderer>());		return;
				case 5: defineButtonsStates(GetTargetActor()->GetComponent<Light>());		return;
				case 8: defineButtonsStates(GetTargetActor()->GetComponent<Animator>());		return;
				case 9: defineButtonsStates(GetTargetActor()->GetComponent<UIText>());		return;
				case 10: defineButtonsStates(GetTargetActor()->GetComponent<UIImage>());		return;
				case 11: defineButtonsStates(GetTargetActor()->GetComponent<RigidStatic>());		return;
				case 12: defineButtonsStates(GetTargetActor()->GetComponent<RigidDynamic>());		return;
				case 13: defineButtonsStates(GetTargetActor()->GetComponent<BoxCollider>());		return;
				case 14: defineButtonsStates(GetTargetActor()->GetComponent<SphereCollider>());		return;
				case 15: defineButtonsStates(GetTargetActor()->GetComponent<CapsuleCollider>());		return;
				case 16: defineButtonsStates(GetTargetActor()->GetComponent<CharacterController>());		return;
				case 17: defineButtonsStates(GetTargetActor()->GetComponent<ThirdPersonController>());		return;
				}
			};

		m_componentSelectorWidget = &componentSelectorWidget;
	}

	/* Script selector + button */
	{
		m_scriptSelectorWidget = &m_inspectorHeader->CreateWidget<InputText>("");
		m_scriptSelectorWidget->lineBreak = false;
		auto& ddTarget = m_scriptSelectorWidget->AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File");

		auto& addScriptButton = m_inspectorHeader->CreateWidget<Button>("Add Script", Vector2{ 100.f, 0.0f });
		addScriptButton.idleBackgroundColor = Color{ 0.7f, 0.5f, 0.f };
		addScriptButton.textColor = Color::White;

		// Add script button state updater
  //      const auto updateAddScriptButton = [&addScriptButton, this](const std::string& p_script)
  //      {
  //          const std::string realScriptPath = EDITOR_CONTEXT(projectScriptsPath) + p_script + ".lua";

  //          const auto targetActor = GetTargetActor();
  //          const bool isScriptValid = std::filesystem::exists(realScriptPath) && targetActor && !targetActor->GetBehaviour(p_script);

  //          addScriptButton.disabled = !isScriptValid;
  //          addScriptButton.idleBackgroundColor = isScriptValid ? Color{ 0.7f, 0.5f, 0.f } : Color{ 0.1f, 0.1f, 0.1f };
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

	m_destroyedListener = GameObject::DestroyedEvent += [this](GameObject* p_destroyed)
		{
			if (p_destroyed == m_targetActor)
				UnFocus();
		};
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
	if (!p_target) { return; }
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

bool LitchiEditor::Inspector::DrawAtomicTypeObject(WidgetContainer& p_root, const type& t, const variant& var, const string_view propertyName, Object* obj, std::vector<string> propertyPathList)
{
	/*auto& col = p_root.CreateWidget<Columns<2>>();
	col.widths[0] = 10.0f;
	auto& dumy = col.CreateWidget<Dummy>(Vector2(0.0f, 5.0f));
	auto& propertyRoot = col.CreateWidget<GroupCollapsable>(propertyName.to_string());*/

	auto& propertyRoot = p_root.CreateWidget<Columns<2>>();
	propertyRoot.widths[0] = 150.0f;
	propertyRoot.widths[1] = 300.0f;

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
		else if (t == type::get<unsigned char>() || t == type::get<uint8_t>())
		{
			auto get = [var, property_field]
				{
					return property_field.GetValue().to_uint8();
				};

			auto set = [property_field](uint8_t value)
				{
					property_field.SetValue(value);
				};

			GUIDrawer::DrawInputField4UInt8(propertyRoot, propertyName.to_string(), get, set);
		}
		else if (t == type::get<int>())
		{
			auto getInt = [var, property_field]
				{
					return property_field.GetValue().to_int32();
				};

			auto setInt = [property_field](int value)
				{
					property_field.SetValue(value);
				};

			GUIDrawer::DrawInputField4Int(propertyRoot, propertyName.to_string(), getInt, setInt);
		}
		else if (t == type::get<uint64_t>())
		{
			auto getInt = [var, property_field]
				{
					return property_field.GetValue().to_uint64();
				};

			auto setInt = [property_field](uint64_t value)
				{
					property_field.SetValue(value);
				};

			GUIDrawer::DrawInputField4UInt64(propertyRoot, propertyName.to_string(), getInt, setInt);

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
		else
		{
			std::string content = propertyName.to_string() + " " + t.get_name() + " NotSupport";
			GUIDrawer::DrawContent(propertyRoot, content);
			return false;
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
			enumeration enum_align = t.get_enumeration();
			std::vector<std::string> enumValueList;
			for (auto enumName : enum_align.get_names())
			{
				enumValueList.push_back(enumName.to_string());
			}
			GUIDrawer::DrawEnum(propertyRoot, propertyName.to_string(), enumValueList, getString, setString);

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
		auto getString = [var, property_field]
			{
				return property_field.GetValue().to_string();
			};

		auto setString = [property_field](std::string value)
			{
				property_field.SetValue(value);
			};

		GUIDrawer::DrawString(propertyRoot, propertyName.to_string(), getString, setString);
		return true;
	}

	return false;
}

void LitchiEditor::Inspector::DrawArray(WidgetContainer& p_root, const variant_sequential_view& view, const string_view propertyName, Object* obj, std::vector<string> propertyPathList)
{
	// draw size

	// draw item
	auto& propertyRoot = p_root.CreateWidget<TreeNode>(propertyName.to_string(), true, true);
	int index = 0;
	for (const auto& item : view)
	{
		propertyPathList.push_back(std::to_string(index));
		// todo: dont support item is arr
		/*if (item.is_sequential_container())
		{
			DrawArray(p_root,item.create_sequential_view(), propertyName, propertyPathList);
		}
		else*/
		{
			variant wrapped_var = item.extract_wrapped_value();
			type value_type = wrapped_var.get_type();
			if (value_type.is_arithmetic() || value_type == type::get<std::string>() || value_type.is_enumeration())
			{
				DrawAtomicTypeObject(propertyRoot, value_type, wrapped_var, std::to_string(index), obj, propertyPathList);
			}
			else
			{
				DrawInstanceInternalRecursively(propertyRoot, wrapped_var, obj, propertyPathList);
			}
		}

		propertyPathList.pop_back();
		index++;
	}

	// draw add and remove button
	PropertyField property_field(obj, propertyPathList);

	auto& arrBtnCol = p_root.CreateWidget<Columns<2>>();
	arrBtnCol.widths[0] = 50.0f;

	auto& addBtn = arrBtnCol.CreateWidget<Button>("Add");
	addBtn.ClickedEvent += [index, property_field, this] {

		property_field.SetSize(index+1);
		NeedRefresh();
		};

	auto& removeBtn = arrBtnCol.CreateWidget<Button>("Remove");
	removeBtn.ClickedEvent += [this, index, property_field]
		{
			
			if(index>0)
			{
				property_field.SetSize(index - 1);
				NeedRefresh();
			}
		};
}

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

bool LitchiEditor::Inspector::DrawProperty(WidgetContainer& p_root, const variant& var, const string_view propertyName, Object* obj, std::vector<string> propertyPathList)
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
		DrawArray(p_root, var.create_sequential_view(), propertyName, obj, propertyPathList);

	}
	/*else if (var.is_associative_container())
	{
		DrawAssociativeContainer(p_root,var.create_associative_view(), propertyName);
	}*/
	else
	{
		auto child_props = is_wrapper ? wrapped_type.get_properties() : value_type.get_properties();

		// x 垂直, y 水平
		/*auto& col = p_root.CreateWidget<Columns<2>>();
		col.widths[0] = 100.0f;
		col.widths[1] = 150.0f;
		auto& text = col.CreateWidget<Text>(propertyName.to_string());
		auto& propertyRoot = col.CreateWidget<Group>();*/

		auto& propertyRoot = p_root.CreateWidget<TreeNode>(propertyName.to_string(), true, true);
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

void LitchiEditor::Inspector::DrawInstanceInternalRecursively(WidgetContainer& p_root, const instance& inputIns, Object* obj, std::vector<string> propertyPathList)
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

		propertyPathList.push_back(name.to_string());
		if(!DrawCustomInstanceInternal(p_root, prop, prop_value,name, obj,propertyPathList))
		{
			// default 绘制
			if (!DrawProperty(p_root, prop_value, name, obj, propertyPathList))
			{
				DEBUG_LOG_ERROR("cannot serialize property:{}", name.to_string());
			}
		}
		
		propertyPathList.pop_back();
	}

}

bool LitchiEditor::Inspector::DrawCustomInstanceInternal(WidgetContainer& p_root, property prop, variant prop_value , const string_view name,Object* obj, std::vector<std::string> propertyPathList)
{
	if (prop.get_type() == type::get_by_name("Vector2"))
	{
		// 绘制rotation
		PropertyField property_field(obj, propertyPathList);
		auto getVec = [prop_value, property_field]
			{
				auto vec = property_field.GetValue().get_value<Vector2>();
				return vec;
			};
		auto setVec = [property_field](Vector2 vec)
			{
				bool result = property_field.SetValue(vec);
				if (!result)
				{
					DEBUG_LOG_ERROR("Vector2 Write Fail!");
				}
			};

		GUIDrawer::DrawVec2(p_root, name.to_string(), getVec, setVec);
	}
	else if (prop.get_type() == type::get_by_name("Vector3"))
	{
		// 绘制rotation
		PropertyField property_field(obj, propertyPathList);
		auto getVec = [prop_value, property_field]
			{
				auto vec = property_field.GetValue().get_value<Vector3>();
				return vec;
			};
		auto setVec = [property_field](Vector3 vec)
			{
				bool result = property_field.SetValue(vec);
				if (!result)
				{
					DEBUG_LOG_ERROR("Vector3 Write Fail!");
				}
			};

		GUIDrawer::DrawVec3(p_root, name.to_string(), getVec, setVec);
	}
	else if (prop.get_type() == type::get_by_name("Vector4"))
	{
		// 绘制rotation
		PropertyField property_field(obj, propertyPathList);
		auto getVec = [prop_value, property_field]
			{
				auto vec = property_field.GetValue().get_value<Vector4>();
				return vec;
			};
		auto setVec = [property_field](Vector4 vec)
			{
				bool result = property_field.SetValue(vec);
				if (!result)
				{
					DEBUG_LOG_ERROR("Vector4 Write Fail!");
				}
			};

		GUIDrawer::DrawVec4(p_root, name.to_string(), getVec, setVec);
	}
	else if (prop.get_type() == type::get_by_name("Color"))
	{
		// 绘制rotation
		PropertyField property_field(obj, propertyPathList);
		auto getVec = [prop_value, property_field]
			{
				auto vec = property_field.GetValue().get_value<Color>();
				return vec;
			};
		auto setVec = [property_field](Color vec)
			{
				bool result = property_field.SetValue(vec);
				if (!result)
				{
					DEBUG_LOG_ERROR("Color Write Fail!");
				}
			};

		GUIDrawer::DrawColor(p_root, name.to_string(), getVec, setVec);
	}
	//else if(prop.get_type() == type::get_by_name("AnimationClipInfo"))
	//{
	//	// 绘制名字
	//	// 绘制clip
	//	// 绘制动画枚举
	//}
	// property 特殊绘制
	else if (prop.get_metadata("QuatToEuler"))
	{
		// 绘制rotation
		PropertyField property_field(obj, propertyPathList);
		auto getVec3 = [prop_value, property_field]
			{
				auto localRotation = property_field.GetValue().get_value<Quaternion>();
				auto localRotation4Euler = localRotation.ToEulerAngles();
				auto localRotation4DegreesEuler = Vector3((localRotation4Euler.x), (localRotation4Euler.y), (localRotation4Euler.z));
				return localRotation4DegreesEuler;
			};
		auto setVec3 = [property_field](Vector3 eulerVec3)
			{
				// 万向节(x,y,z)(pitch, yaw, roll)
				//-180 <Yaw<= 180  -90<= Pitch<= 90  -180 <Roll<= 180 if (Pitch == -90 || Pitch == 90) Roll = 0
				eulerVec3.x = std::max(std::min(eulerVec3.x, 180.0f), -180.0f);
				eulerVec3.y = std::max(std::min(eulerVec3.y, 180.0f), -180.0f);
				eulerVec3.z = std::max(std::min(eulerVec3.z, 180.0f), -180.0f);
				// 处理奇点问题
				if (eulerVec3.y == -90.0f || eulerVec3.y == 90.0f)
				{
					eulerVec3.z = 0.0f;
				}

				//bool result = property_field.SetValue(Quaternion(glm::radians(eulerVec3)));
				bool result = property_field.SetValue(Quaternion::FromEulerAngles((eulerVec3.x), (eulerVec3.y), (eulerVec3.z)));
				if (!result)
				{
					DEBUG_LOG_ERROR("QuatToEuler Write Fail!");
				}
			};

		GUIDrawer::DrawVec3(p_root, name.to_string(), getVec3, setVec3);

	}
	else if (prop.get_metadata("AssetPath"))
	{
		auto assetType = prop.get_metadata("AssetType").get_value<PathParser::EFileType>();
		PropertyField property_field(obj, propertyPathList);
		auto path = property_field.GetValue().get_value<std::string>();
		auto& col  = p_root.CreateWidget<Columns<2>>();
		col.widths[0] = 125.0f;
		col.CreateWidget<TextColored>(name.to_string(), GUIDrawer::TitleColor);
		if (path.empty())
		{
			path = "Empty";
		}
		auto& widget = col.CreateWidget<Text>(path);
		widget.AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [assetType, &widget, property_field](auto p_receivedData)
			{
				auto& newPath = p_receivedData.first;
				if (PathParser::GetFileType(newPath) == assetType)
				{
					if (property_field.SetValue(newPath))
					{
						widget.content = newPath;
					}
				}

			};
	}
	else
	{
		return false;
	}

	return true;
}

void LitchiEditor::Inspector::OnDraw()
{
	PanelWindow::OnDraw();

	if(m_needRefresh)
	{
		Refresh();
		ResetNeedRefresh();
	}
}

void LitchiEditor::Inspector::DrawInstance(WidgetContainer& p_root, rttr::instance ins, Object* obj)
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
			if (p_component->GetGameObject()->RemoveComponent(p_component))
			{
				m_componentSelectorWidget->ValueChangedEvent.Invoke(m_componentSelectorWidget->currentChoice);
			}
		};
	auto& columns = header.CreateWidget<Columns<2>>();
	columns.widths[0] = 200;
	columns.widths[1] = 200;

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
