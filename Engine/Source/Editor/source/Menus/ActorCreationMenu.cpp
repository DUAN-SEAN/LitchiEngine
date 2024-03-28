
#include "Runtime/Core/pch.h"

#include <functional>
#include "Editor/include/Menus/ActorCreationMenu.h"

#include "Editor/include/Core/EditorActions.h"
#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Core/Global/ServiceLocator.h"
#include "Runtime/Function/Framework/Component/Light/Light.h"
#include "Runtime/Function/Framework/Component/UI/UICanvas.h"
#include "Runtime/Function/Framework/Component/UI/UIImage.h"
#include "Runtime/Function/Framework/Component/UI/UIText.h"

std::function<void()> Combine(std::function<void()> p_a, std::optional<std::function<void()>> p_b)
{
    if (p_b.has_value())
    {
        return [=]()
        {
            p_a();
            p_b.value()();
        };
    }

    return p_a;
}

template<class T>
std::function<void()> ActorWithComponentCreationHandler(LitchiRuntime::GameObject* p_parent, std::optional<std::function<void()>> p_onItemClicked)
{
    return Combine(EDITOR_BIND(CreateMonoComponentActor<T>, true, p_parent), p_onItemClicked);
}

template<class T>
std::function<void()> UIActorWithComponentCreationHandler(LitchiRuntime::GameObject* p_parent, std::optional<std::function<void()>> p_onItemClicked)
{
    return Combine(EDITOR_BIND(CreateUIActor<T>, true, p_parent), p_onItemClicked);
}

std::function<void()> ActorWithModelComponentCreationHandler(LitchiRuntime::GameObject* p_parent, const std::string& p_modelName, std::optional<std::function<void()>> p_onItemClicked)
{
    return Combine(EDITOR_BIND(CreateActorWithModel, ":Models/" + p_modelName + ".fbx", true, p_parent, p_modelName), p_onItemClicked);
}

void LitchiEditor::ActorCreationMenu::GenerateActorCreationMenu(LitchiRuntime::MenuList& p_menuList, LitchiRuntime::GameObject* p_parent, std::optional<std::function<void()>> p_onItemClicked)
{
    p_menuList.CreateWidget<LitchiRuntime::MenuItem>("Create Empty").ClickedEvent += Combine(EDITOR_BIND(CreateEmptyActor, true, p_parent, "",false), p_onItemClicked);

    auto& primitives = p_menuList.CreateWidget<LitchiRuntime::MenuList>("Primitives");
    auto& physicals = p_menuList.CreateWidget<LitchiRuntime::MenuList>("Physicals");
    auto& lights = p_menuList.CreateWidget<LitchiRuntime::MenuList>("Lights");
    auto& audio = p_menuList.CreateWidget<LitchiRuntime::MenuList>("Audio");
    auto& ui = p_menuList.CreateWidget<LitchiRuntime::MenuList>("UI");
    auto& others = p_menuList.CreateWidget<LitchiRuntime::MenuList>("Others");

    primitives.CreateWidget<LitchiRuntime::MenuItem>("Cube").ClickedEvent              += ActorWithModelComponentCreationHandler(p_parent, "Cube", p_onItemClicked);
    primitives.CreateWidget<LitchiRuntime::MenuItem>("Sphere").ClickedEvent            += ActorWithModelComponentCreationHandler(p_parent, "Sphere", p_onItemClicked);
    primitives.CreateWidget<LitchiRuntime::MenuItem>("Cone").ClickedEvent              += ActorWithModelComponentCreationHandler(p_parent, "Cone", p_onItemClicked);
    primitives.CreateWidget<LitchiRuntime::MenuItem>("Cylinder").ClickedEvent          += ActorWithModelComponentCreationHandler(p_parent, "Cylinder", p_onItemClicked);
    primitives.CreateWidget<LitchiRuntime::MenuItem>("Plane").ClickedEvent             += ActorWithModelComponentCreationHandler(p_parent, "Plane", p_onItemClicked);
    primitives.CreateWidget<LitchiRuntime::MenuItem>("Gear").ClickedEvent              += ActorWithModelComponentCreationHandler(p_parent, "Gear", p_onItemClicked);
    primitives.CreateWidget<LitchiRuntime::MenuItem>("Helix").ClickedEvent             += ActorWithModelComponentCreationHandler(p_parent, "Helix", p_onItemClicked);
    primitives.CreateWidget<LitchiRuntime::MenuItem>("Pipe").ClickedEvent              += ActorWithModelComponentCreationHandler(p_parent, "Pipe", p_onItemClicked);
    primitives.CreateWidget<LitchiRuntime::MenuItem>("Pyramid").ClickedEvent           += ActorWithModelComponentCreationHandler(p_parent, "Pyramid", p_onItemClicked);
   /* primitives.CreateWidget<LitchiRuntime::MenuList>("Torus").ClickedEvent             += ActorWithModelComponentCreationHandler(p_parent, "Torus", p_onItemClicked);
    physicals.CreateWidget<LitchiRuntime::MenuList>("Physical Box").ClickedEvent       += ActorWithComponentCreationHandler<CPhysicalBox>(p_parent, p_onItemClicked);
    physicals.CreateWidget<LitchiRuntime::MenuList>("Physical Sphere").ClickedEvent    += ActorWithComponentCreationHandler<CPhysicalSphere>(p_parent, p_onItemClicked);
    physicals.CreateWidget<LitchiRuntime::MenuList>("Physical Capsule").ClickedEvent   += ActorWithComponentCreationHandler<CPhysicalCapsule>(p_parent, p_onItemClicked);
    lights.CreateWidget<LitchiRuntime::MenuList>("Point").ClickedEvent                 += ActorWithComponentCreationHandler<CPointLight>(p_parent, p_onItemClicked);*/
    lights.CreateWidget<LitchiRuntime::MenuItem>("Light").ClickedEvent           += ActorWithComponentCreationHandler<LitchiRuntime::Light>(p_parent, p_onItemClicked);


    ui.CreateWidget<LitchiRuntime::MenuItem>("Canvas").ClickedEvent += ActorWithComponentCreationHandler<LitchiRuntime::UICanvas>(p_parent, p_onItemClicked);
    ui.CreateWidget<LitchiRuntime::MenuItem>("Text").ClickedEvent += UIActorWithComponentCreationHandler<LitchiRuntime::UIText>(p_parent, p_onItemClicked);
    ui.CreateWidget<LitchiRuntime::MenuItem>("Image").ClickedEvent += UIActorWithComponentCreationHandler<LitchiRuntime::UIImage>(p_parent, p_onItemClicked);

  /*  lights.CreateWidget<LitchiRuntime::MenuList>("Spot").ClickedEvent                  += ActorWithComponentCreationHandler<CSpotLight>(p_parent, p_onItemClicked);
    lights.CreateWidget<LitchiRuntime::MenuList>("Ambient Box").ClickedEvent           += ActorWithComponentCreationHandler<CAmbientBoxLight>(p_parent, p_onItemClicked);
    lights.CreateWidget<LitchiRuntime::MenuList>("Ambient Sphere").ClickedEvent        += ActorWithComponentCreationHandler<CAmbientSphereLight>(p_parent, p_onItemClicked);
    audio.CreateWidget<LitchiRuntime::MenuList>("Audio Source").ClickedEvent           += ActorWithComponentCreationHandler<CAudioSource>(p_parent, p_onItemClicked);
    audio.CreateWidget<LitchiRuntime::MenuList>("Audio Listener").ClickedEvent         += ActorWithComponentCreationHandler<CAudioListener>(p_parent, p_onItemClicked);*/
    others.CreateWidget<LitchiRuntime::MenuItem>("Camera").ClickedEvent                += ActorWithComponentCreationHandler<LitchiRuntime::Camera>(p_parent, p_onItemClicked);
}
