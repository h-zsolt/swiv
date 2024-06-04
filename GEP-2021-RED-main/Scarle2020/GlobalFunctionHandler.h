#pragma once

#include "IEvent.h"
#include "IFunctionHandler.h"

template<class EventType>
class GlobalFunctionHandler : public IFunctionHandler
{
public:
	using GlobalFunction = void (*)(EventType*);

public:
	explicit GlobalFunctionHandler(GlobalFunction fnc) : m_function(fnc) {}
	~GlobalFunctionHandler() { m_function = nullptr; }

	void call(IEvent* event) override { (*m_function)(static_cast<EventType*>(event)); }
	const GlobalFunction function() const { return m_function; }

private:
	GlobalFunction m_function = nullptr;
};