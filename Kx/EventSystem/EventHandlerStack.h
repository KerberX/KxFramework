#pragma once
#include "EvtHandler.h"

class KX_API KxEvtHandlerStack
{
	public:
		enum class Order
		{
			LastToFirst,
			FirstToLast,
		};

	private:
		wxEvtHandler* m_Base = nullptr;
		wxEvtHandler* m_Top = nullptr;

	public:
		KxEvtHandlerStack(wxEvtHandler& first)
			:m_Base(&first), m_Top(&first)
		{
		}
		KxEvtHandlerStack(const KxEvtHandlerStack&) = delete;

	public:
		bool Push(wxEvtHandler& evtHandler);
		bool Remove(wxEvtHandler& evtHandler);
		wxEvtHandler* Pop();

		wxEvtHandler* GetBase() const
		{
			return m_Base;
		}
		wxEvtHandler* GetTop() const
		{
			return m_Top;
		}
		bool HasChainedItems() const
		{
			return m_Base != m_Top;
		}
		size_t GetCount() const
		{
			size_t count = 0;
			ForEachItem(Order::LastToFirst, [&count](wxEvtHandler& chainItem)
			{
				count++;
				return true;
			});
			return count;
		}

		template<class TFunc>
		wxEvtHandler* ForEachItem(Order order, TFunc&& func, bool chainedItemsOnly = false) const
		{
			auto TestItem = [&](wxEvtHandler* item)
			{
				return item && (!chainedItemsOnly || item != m_Base);
			};

			switch (order)
			{
				case Order::FirstToLast:
				{
					for (wxEvtHandler* item = m_Base; TestItem(); item = item->GetPreviousHandler())
					{
						if (!func(*item))
						{
							return item;
						}
					}
				}
				case Order::LastToFirst:
				{
					for (wxEvtHandler* item = m_Top; TestItem(); item = item->GetNextHandler())
					{
						if (!func(*item))
						{
							return item;
						}
					}
				}
			};
			return nullptr;
		}

	public:
		KxEvtHandlerStack& operator=(const KxEvtHandlerStack&) = delete;
};
