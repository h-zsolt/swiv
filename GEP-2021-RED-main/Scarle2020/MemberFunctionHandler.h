#pragma once

#include "IEvent.h"
#include "IFunctionHandler.h"

template<class T, class EventType>
class MemberFunctionHandler : public IFunctionHandler
{
public:
    using MemberFunction = void (T::*)(EventType*);

public:
    MemberFunctionHandler(T* _instance, MemberFunction function) : m_instance(_instance), m_function(function) {}

    void call(IEvent* event) override { (m_instance->*m_function)(static_cast<EventType*>(event)); }
    const T* instance() const { return m_instance; }
    const MemberFunction function() const { return m_function; }

private:
    T* m_instance = { nullptr };
    MemberFunction m_function = { nullptr };
};