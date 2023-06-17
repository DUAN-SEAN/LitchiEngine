
#pragma once

#include <functional>
#include <optional>

#include "Runtime/Function/UI/Widgets/Menu/MenuList.h"

namespace LitchiRuntime
{
	class GameObject;
}

namespace LitchiEditor
{
    /**
    * Class exposing tools to generate an actor creation menu
    */
    class ActorCreationMenu
    {
    public:
        /**
        * Disabled constructor
        */
        ActorCreationMenu() = delete;

        /**
        * Generates an actor creation menu under the given MenuList item.
        * Also handles custom additionnal OnClick callback
        * @param p_menuList
        * @param p_parent
        * @param p_onItemClicked
        */
        static void GenerateActorCreationMenu(LitchiRuntime::MenuList& p_menuList, LitchiRuntime::GameObject* p_parent = nullptr, std::optional<std::function<void()>> p_onItemClicked = {});
    };
}
