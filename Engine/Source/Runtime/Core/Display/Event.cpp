
//= INCLUDES ======
#include "Runtime/Core/pch.h"
#include "Event.h"
//=================

//= NAMESPACES =====
using namespace std;
//==================

namespace Spartan
{
    namespace
    {
        static array<vector<subscriber>, 16> event_subscribers;
    }

    void Event::Shutdown()
    {
        for (vector<subscriber>& subscribers : event_subscribers)
        {
            subscribers.clear();
        }
    }

    void Event::Subscribe(const EventType event_type, subscriber&& function)
    {
        event_subscribers[static_cast<uint32_t>(event_type)].push_back(forward<subscriber>(function));
    }

    void Event::Fire(const EventType event_type, sp_variant data /*= 0*/)
    {
        for (const auto& subscriber : event_subscribers[static_cast<uint32_t>(event_type)])
        {
            subscriber(data);
        }
    }
}
