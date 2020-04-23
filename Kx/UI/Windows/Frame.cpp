#include "stdafx.h"
#include "Frame.h"

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(Frame, wxFrame);

	bool Frame::Create(wxWindow* parent,
					   wxWindowID id,
					   const String& title,
					   const wxPoint& pos,
					   const wxSize& size,
					   FrameStyle style
	)
	{
		if (wxFrame::Create(parent, id, title, pos, size, ToInt(style)))
		{
			SetStatusBarPane(-1);
			SetDefaultBackgroundColor();
			Center(wxBOTH);

			return true;
		}
		return false;
	}
}