
#pragma once

//= INCLUDES ========
#include <functional>
#include <variant>
//===================

/*
HOW TO USE
================================================================================
To subscribe a function to an event -> SP_SUBSCRIBE_TO_EVENT(EVENT_ID, Handler);
To fire an event                    -> SP_FIRE_EVENT(EVENT_ID);
To fire an event with data          -> SP_FIRE_EVENT_DATA(EVENT_ID, Variant);

Note: This is a blocking event system
================================================================================
*/

//= MACROS ===============================================================================================
#define SP_EVENT_HANDLER_EXPRESSION(expression)        [this](Spartan::sp_variant var)  { expression }
#define SP_EVENT_HANDLER_EXPRESSION_STATIC(expression) [](Spartan::sp_variant var)      { expression }

#define SP_EVENT_HANDLER(function)                     [this](Spartan::sp_variant var)  { function(); }
#define SP_EVENT_HANDLER_STATIC(function)              [](Spartan::sp_variant var)      { function(); }
                                                                                     
#define SP_EVENT_HANDLER_VARIANT(function)             [this](Spartan::sp_variant var)  { function(var); }
#define SP_EVENT_HANDLER_VARIANT_STATIC(function)      [](Spartan::sp_variant var)      { function(var); }
                                                       
#define SP_FIRE_EVENT(event_enum)                      Spartan::Event::Fire(event_enum)
#define SP_FIRE_EVENT_DATA(event_enum, data)           Spartan::Event::Fire(event_enum, data)
                                                       
#define SP_SUBSCRIBE_TO_EVENT(event_enum, function)    Spartan::Event::Subscribe(event_enum, function);
//========================================================================================================

namespace Spartan
{
    enum class EventType
    {
        // Engine
        EngineShutdown,                  // The engine is about to shutdown
        // Renderer                      
        RendererOnInitialized,           // The renderer has been initialized
        RendererOnFirstFrameCompleted,   // The renderer has completed the first frame
        RendererPostPresent,             // The renderer has finished presenting a frame
        RendererOnShutdown,              // The renderer is about to shutdown
        // World                         
        WorldSaveStart,                  // The world is about to be saved to a file
        WorldSavedEnd,                   // The world finished saving to file
        WorldLoadStart,                  // The world is about to be loaded from a file
        WorldLoadEnd,                    // The world finished loading from file
        WorldClear,                      // The world is about to clear everything
        WorldResolve,                    // The world is resolving
        WorldResolved,                   // The world has finished resolving
        // SDL                           
        Sdl,                             // An SDL event
        // Window                        
        WindowResized,                   // The window has been resized
        WindowFullscreen,                // The window has been set to fullscreen
        WindowFullscreenWindowedToggled, // The window has been toggled between fullscreen and windowed
    };

    class Entity;

    using sp_variant = std::variant<
        int,
        void*,
        std::vector<std::shared_ptr<Entity>>

    >;
    using subscriber = std::function<void(const sp_variant&)>;

    class SP_CLASS Event
    {
    public:
        static void Shutdown();
        static void Subscribe(const EventType event_type, subscriber&& function);
        static void Fire(const EventType event_type, sp_variant data = 0);
    };
}
