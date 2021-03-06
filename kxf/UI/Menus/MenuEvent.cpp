#include "KxfPCH.h"
#include "MenuEvent.h"
#include "Menu.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(MenuEvent, wxNotifyEvent);

	MenuEvent::MenuEvent(const EventID& type, Menu* menu, MenuItem* menuItem)
		:wxNotifyEvent(type.AsInt(), menuItem ? menuItem->GetId() : wxID_NONE), m_Menu(menu), m_Item(menuItem)
	{
		Allow();
	}

	wxWindow* MenuEvent::GetInvokingWindow() const
	{
		return m_Menu ? m_Menu->GetWindow() : nullptr;
	}
	String MenuEvent::GetHelpString() const
	{
		if (m_Item)
		{
			return m_Item->GetHelp();
		}
		return m_HelpString;
	}
}
