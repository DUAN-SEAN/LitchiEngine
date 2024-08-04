
#pragma once
#include <string>

#include "Runtime/Core/Meta/Reflection/Object.h"
#include "Runtime/Function/UI/Panels/PanelWindow.h"
#include "Runtime/Function/UI/Settings/PanelWindowSettings.h"
#include "Runtime/Function/UI/Widgets/InputFields/InputText.h"
#include "Runtime/Function/UI/Widgets/Layout/Group.h"
#include "Runtime/Function/UI/Widgets/Selection/ComboBox.h"

namespace LitchiRuntime
{
	class Component;
	class GameObject;
}
using namespace LitchiRuntime;
namespace LitchiEditor
{
	class Inspector : public LitchiRuntime::PanelWindow
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		Inspector
		(
			const std::string& p_title,
			bool p_opened,
			const LitchiRuntime::PanelWindowSettings& p_windowSettings
		);

		/**
		* Destructor
		*/
		~Inspector();

		/**
		* Focus the given actor
		* @param p_target
		*/
		void FocusActor(GameObject* p_target);

		/**
		* Unfocus the currently targeted actor
		*/
		void UnFocus();

		/**
		* Unfocus the currently targeted actor without removing listeners attached to this actor
		*/
		void SoftUnFocus();

		/**
		* Returns the currently selected actor
		*/
		GameObject* GetTargetActor() const;

		/**
		* Create the actor inspector for the given actor
		*/
		void CreateActorInspector(GameObject* p_target);

		/**
		* Draw the given component in inspector
		*/
		void DrawComponent(std::string name,Component* p_component);

		///**
		//* Draw the given behaviour in inspector
		//*/
		//void DrawBehaviour(OvCore::ECS::Components::Behaviour& p_behaviour);

		/**
		* Refresh the inspector
		*/
		void Refresh();

	private:

		void OnDraw() override;

		void DrawInstance(WidgetContainer& p_root, rttr::instance ins, Object* obj);
		void DrawInstanceInternalRecursively(WidgetContainer& p_root, const rttr::instance& inputIns, Object* obj, std::vector<std::string> propertyPathList);
		void DrawArray(WidgetContainer& p_root, const rttr::variant_sequential_view& view, const rttr::string_view propertyName, Object* obj, std::vector<std::string> propertyPathList);
		bool DrawProperty(WidgetContainer& p_root, const rttr::variant& var, const rttr::string_view propertyName, Object* obj, std::vector<std::string> propertyPathList);

		bool DrawAtomicTypeObject(WidgetContainer& p_root, const rttr::type& t, const rttr::variant& var, const rttr::string_view propertyName, Object* obj, std::vector<std::string> propertyPathList);

		void NeedRefresh() { m_needRefresh = true; }
		void ResetNeedRefresh() { m_needRefresh = false; }
	private:
		GameObject* m_targetActor = nullptr;
		Group* m_actorInfo;
		Group* m_inspectorHeader;
		ComboBox* m_componentSelectorWidget;
        InputText* m_scriptSelectorWidget;

		uint64_t m_componentAddedListener	= 0;
		uint64_t m_componentRemovedListener = 0;
		uint64_t m_behaviourAddedListener	= 0;
		uint64_t m_behaviourRemovedListener = 0;
		uint64_t m_destroyedListener		= 0;

		bool m_needRefresh;
	};
}
