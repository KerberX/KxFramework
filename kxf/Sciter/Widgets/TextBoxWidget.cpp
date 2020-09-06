#include "stdafx.h"
#include "TextBoxWidget.h"

namespace
{
	constexpr std::wstring_view g_Stylesheet =
		#include "TextBoxWidget.css"
		;

	namespace Attribute
	{
		constexpr auto Label = "Label";
	}
}

namespace kxf::Sciter
{
	void TextBoxWidget::OnAttached()
	{
		Element thisNode = GetElement();

		m_LabelArea = Element::Create("LabelArea");
		m_EditArea = Element::Create("EditArea");
		if (thisNode.AppendChild(m_LabelArea) && thisNode.AppendChild(m_EditArea))
		{
			SetLabel(thisNode.GetAttribute(Attribute::Label));
			thisNode.RemoveAttribute(Attribute::Label);

			m_EditAreaEvtHandler.Bind(FocusEvent::EvtSetFocus, [&](FocusEvent& event)
			{
				m_LabelArea.SetStyleAttribute("transform", "translate(0, 0) scale(0.9)");
			});
			m_EditAreaEvtHandler.Bind(FocusEvent::EvtKillFocus, [&](FocusEvent& event)
			{
				if (!m_EditArea.HasValue())
				{
					m_LabelArea.SetStyleAttribute("transform", "translate(0, 1.5em) scale(1)");
				}
			});
			m_EditArea.AttachEventHandler(m_EditAreaEvtHandler);

			Widget::OnAttached();
		}
	}

	String TextBoxWidget::GetValue() const
	{
		return m_EditArea.GetValue().GetString();
	}
	void TextBoxWidget::SetValue(StringView value)
	{
		m_EditArea.SetValue(value);
	}

	String TextBoxWidget::GetLabel() const
	{
		return m_LabelArea.GetValue().GetString();
	}
	void TextBoxWidget::SetLabel(const String& label)
	{
		m_LabelArea.SetValue(label);
	}
}

namespace kxf::Sciter
{
	String TextBoxWidgetFactory::GetWidgetStylesheet() const
	{
		return String::FromView(g_Stylesheet);
	}
}
