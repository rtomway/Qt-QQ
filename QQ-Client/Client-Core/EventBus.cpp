#include "EventBus.h"

EventBus* EventBus::instance()
{
	static EventBus ins;
	return &ins; 
}
