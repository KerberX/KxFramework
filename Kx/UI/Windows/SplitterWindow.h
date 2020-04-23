#pragma once
#include "Kx/UI/Common.h"
#include <wx/splitter.h>

namespace KxFramework::UI
{
	enum class SplitterWindowStyle
	{
		None = 0,

		LiveUpdate = wxSP_LIVE_UPDATE,
		AllowUnsplit = wxSP_PERMIT_UNSPLIT,
		SashNone = wxSP_NOSASH,
		Sash3D = wxSP_3DSASH,
		Border3D = wxSP_3DBORDER,
		NoXPTheme = wxSP_NO_XP_THEME,
	};
}
namespace KxFramework::EnumClass
{
	Kx_EnumClass_AllowEverything(UI::SplitterWindowStyle);
}

namespace KxFramework::UI
{
	class KX_API SplitterWindow: public wxSplitterWindow
	{
		public:
			static constexpr SplitterWindowStyle DefaultStyle = SplitterWindowStyle::LiveUpdate|SplitterWindowStyle::Sash3D;

		private:
			Color m_SashColor;
			int m_InitialPosition = 0;

		private:
			void OnDoubleClickSash(int x, int y) override;
			void OnDrawSash(wxPaintEvent& event);

		public:
			SplitterWindow() = default;
			SplitterWindow(wxWindow* parent,
						   wxWindowID id = wxID_ANY,
						   SplitterWindowStyle style = DefaultStyle
			)
			{
				Create(parent, id, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id = wxID_ANY,
						SplitterWindowStyle style = DefaultStyle
			);

		public:
			bool ShouldInheritColours() const override
			{
				return true;
			}
			bool SplitHorizontally(wxWindow* window1, wxWindow* window2, int sashPosition = 0) override;
			bool SplitVertically(wxWindow* window1, wxWindow* window2, int sashPosition = 0) override;

			int GetInitialPosition() const
			{
				return m_InitialPosition;
			}
			void SetInitialPosition(int pos);

			Color GetSashColor() const
			{
				return m_SashColor;
			}
			void SetSashColor(const Color& color);

		public:
			wxDECLARE_DYNAMIC_CLASS(SplitterWindow);
	};
}