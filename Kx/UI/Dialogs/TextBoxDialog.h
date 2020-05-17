#pragma once
#include "Kx/UI/Common.h"
#include "StdDialog.h"
#include "Kx/UI/Controls/TextBox.h"
#include "Kx/UI/Controls/StyledTextBox.h"

namespace KxFramework::UI
{
	enum class TextBoxDialogStyle
	{
		None = 0,

		Multiline = 1 << 0,
		ReadOnly = 1 << 1,
		Password = 1 << 2,
		Styled = 1 << 3
	};
}
namespace KxFramework::EnumClass
{
	Kx_EnumClass_AllowEverything(UI::TextBoxDialogStyle);
}

namespace KxFramework::UI
{
	class KX_API TextBoxDialog: public StdDialog
	{
		public:
			static constexpr TextBoxDialogStyle DefaultStyle = EnumClass::Combine<TextBoxDialogStyle>(StdDialog::DefaultStyle);
			static constexpr int DefaultWidth = 300;
			static constexpr int DefaultMLWidth = 450;
			static constexpr int DefaultMLHeight = 200;

		private:
			wxControl* m_View = nullptr;
			TextBoxDialogStyle m_Options = TextBoxDialogStyle::None;

		private:
			wxOrientation GetViewLabelSizerOrientation() const override;
			wxOrientation GetWindowResizeSide() const override;
			bool IsEnterAllowed(wxKeyEvent& event, wxWindowID* idOut = nullptr) const override;

			bool IsStyledTextBox() const
			{
				return m_Options & TextBoxDialogStyle::Styled;
			}

		public:
			TextBoxDialog() = default;
			TextBoxDialog(wxWindow* parent,
						  wxWindowID id,
						  const String& caption,
						  const wxPoint& pos = wxDefaultPosition,
						  const wxSize& size = wxDefaultSize,
						  StdButton buttons = DefaultButtons,
						  TextBoxDialogStyle style = DefaultStyle
			)
			{
				Create(parent, id, caption, pos, size, buttons, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& caption,
						const wxPoint& pos = wxDefaultPosition,
						const wxSize& size = wxDefaultSize,
						StdButton buttons = DefaultButtons,
						TextBoxDialogStyle style = DefaultStyle
			);

		public:
			wxWindow* GetDialogMainCtrl() const override
			{
				return m_View;
			}
			int GetViewSizerProportion() const override
			{
				return IsMultiLine() ? 1 : 0;
			}
		
			TextBox* GetTextBox() const
			{
				if (!IsStyledTextBox())
				{
					return static_cast<TextBox*>(m_View);
				}
				return nullptr;
			}
			StyledTextBox* GetStyledTextBox() const
			{
				if (IsStyledTextBox())
				{
					return static_cast<StyledTextBox*>(m_View);
				}
				return nullptr;
			}

			String GetValue() const
			{
				if (GetTextBox())
				{
					return GetTextBox()->GetValue();
				}
				else if (GetStyledTextBox())
				{
					return GetStyledTextBox()->GetValue();
				}
				return {};
			}
			void SetValue(const String& value)
			{
				if (GetTextBox())
				{
					GetTextBox()->SetValue(value);
				}
				else if (GetStyledTextBox())
				{
					return GetStyledTextBox()->SetValue(value);
				}
			}
		
			bool IsMultiLine() const
			{
				if (GetTextBox())
				{
					return GetTextBox()->IsMultiLine();
				}
				else if (GetStyledTextBox())
				{
					return GetStyledTextBox()->IsMultiline();
				}
				return false;
			}
			void SetMultiLine(bool value)
			{
				if (GetStyledTextBox())
				{
					return GetStyledTextBox()->SetMultiline(value);
				}
			}
		
			bool IsEditable() const
			{
				if (GetTextBox())
				{
					return GetTextBox()->IsEditable();
				}
				else if (GetStyledTextBox())
				{
					return GetStyledTextBox()->IsEditable();
				}
				return false;
			}
			void SetEditable(bool value)
			{
				if (GetTextBox())
				{
					GetTextBox()->SetEditable(value);
				}
				else if (GetStyledTextBox())
				{
					return GetStyledTextBox()->SetEditable(value);
				}
			}
		
			bool SetHint(const String& value)
			{
				if (GetTextBox())
				{
					return GetTextBox()->SetHint(value);
				}
				else if (GetStyledTextBox())
				{
					return GetStyledTextBox()->SetHint(value);
				}
				return false;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(TextBoxDialog);
	};
}
