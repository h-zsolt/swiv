#pragma once

#include "IFunctionHandler.h"
#include "MemberFunctionHandler.h"
#include "GlobalFunctionHandler.h"

#include <list>
#include <memory>
#include <typeindex>
#include <unordered_map>

// Used to unsubscribe a function from the Event Bus
struct CallbackID
{
    const void* m_handle = nullptr;
    const char* m_id = nullptr;
};

class EventBus
{
private: // TypeDefs //
    using HandlerList = std::list<std::unique_ptr<IFunctionHandler>>;

public: // Constructor & Destructor //
    EventBus() = default;
    ~EventBus() = default;

public:

//------------------------------------------------------------------------------
// Envoke Event Function - public
//------------------------------------------------------------------------------

    /** @brief Loops through list of functions subscribed to event of type 'EventType'
    * 
    * @param event -> template<EventType*> 
    * @return void
    */
    template<typename EventType>
    void envoke(EventType* event)
    {
        auto&& handlers = m_subscribers.at(typeid(EventType).name());

        if (handlers == nullptr)
        {
            return;
        }

        for (auto& handler : *handlers)
        {
            if (handler != nullptr)
            {
                handler->execute(event);
            }
        }
    }

//------------------------------------------------------------------------------
// Event Subscribe Functions - public
//------------------------------------------------------------------------------

	
    /** @brief Subscribe a member function to the Event Bus 
    * 
    * @details Adds a member function to the list of functions to be called when
    *          an event of type 'EventType' is envoked.
    * 
    * Returns an EventHandle used to unsubscribe the function from the Event Bus
    * 
    * @param inst -> class instance
    * @param fnc -> function pointer
    * @return EventHandle
    */
    template<class T, class EventType>
    CallbackID subscribe(T* inst, void (T::* fnc)(EventType*))
    {
        assert(std::is_class<EventType>::value && "EventType parameter of functions is not a Struct or Class");

        using Handler = MemberFunctionHandler<T, EventType>;

        auto&& id       = typeid(EventType).name();
        auto&& handlers = m_subscribers[id];

        if (handlers == nullptr)
        {
            handlers = std::make_unique<HandlerList>();
        }


        for (auto& h : *handlers)
        {
            const auto&& n = static_cast<Handler*>(h.get());
            if (n->function() == fnc && n->instance() == inst)
            {
                return { h.get(), id };
            }

        }

        handlers->emplace_back(std::make_unique<Handler>(inst, fnc));

        return { handlers->back().get(), id };
    }

    /** @brief Subscribe a global or lambda function to the Event Bus
    * 
    * @details Adds a global or lambda function to the list of functions to be called when
    *          an event of type 'EventType' is envoked.
    * 
    * @note Lambda function syntax = event_bus->subscribe<EventType>([](EventType*){});
    * 
    * Returns an EventHandle used to unsubscribe from the EventBus.
    * 
    * @param fnc -> function pointer
    * @return EventHandle
    */
    template<class EventType>
    CallbackID subscribe(void(*fnc)(EventType*))
    {
        assert(std::is_class<EventType>::value && "EventType parameter of functions is not a Struct or Class");

        using Handler = GlobalFunctionHandler<EventType>;

        auto&& id = typeid(EventType).name();
        auto&& handlers = m_subscribers[id];

        if (handlers == nullptr)
        {
            handlers = std::make_unique<HandlerList>();
        }

        for (auto& h : *handlers)
        {
            const auto&& n = static_cast<Handler*>(h.get());
            if (n->function() == fnc)
            {
                return { h.get(), id };
            }
        }

        handlers->emplace_back(std::make_unique<Handler>(fnc));

        return { handlers->back().get(), id };
    }

//------------------------------------------------------------------------------
// Event Unsubscribe Functions - public
//------------------------------------------------------------------------------

    /** @brief Unsubscribes function from the Event Bus
    * 
    * @details Remove the function associated with the EventHandle from the
    * EventBus.
    * 
    * @note Returns true if function unsubscribed from Event Bus
    *       Returns false if function does not exist in Event Bus
    * 
    * @param handle -> EventHandle
    * @return bool
    */
    bool unsubscribe(const CallbackID& handle)
    {

      auto&& s = m_subscribers[handle.m_id];

      if (handle.m_handle == nullptr || s == nullptr)
      {
        return false;
      }

      for (auto itr = s->begin(); itr != s->end(); ++itr)
      {
        if (itr->get() == handle.m_handle)
        {
          s->erase(itr);
          return true;
        }
      }

        return false;
    }

    // Clears all lists of subscribers
//    void reset()
//    {
//        m_subscribers.clear();
//    }

private:
    std::unordered_map<const char*, std::unique_ptr<HandlerList>> m_subscribers = {};
};