#pragma once
#include "../Event.h"

class KxNotifyEvent: public wxNotifyEvent
{
	public:
		KxNotifyEvent() = default;

	public:
		KxNotifyEvent* Clone() const override
		{
			return new KxNotifyEvent(*this);
		}
};
