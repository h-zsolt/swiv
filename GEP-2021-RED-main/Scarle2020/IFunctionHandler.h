#pragma once

#include "IEvent.h"

class IFunctionHandler
{
public:
	virtual ~IFunctionHandler() = default;

	void execute(IEvent* event) { call(event); }

private:
	virtual void call(IEvent* event) = 0;
};