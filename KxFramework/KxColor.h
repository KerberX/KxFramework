#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxColor: public wxColour
{
	public:
		enum class C2S
		{
			Name = 0,
			CSS,
			HTML,
			LuaSyntax,
		};
		enum class C2SAlpha
		{
			None = 0,
			Auto,
			Always,
		};

	public:
		inline static const int ALPHA_OPAQUE_PERCENT = 100;
		inline static const double SRGB_WeightR = 0.2126;
		inline static const double SRGB_WeightG = 0.7152;
		inline static const double SRGB_WeightB = 0.0722;

	public:
		static KxColor FromRGBA(uint32_t color)
		{
			return KxColor().SetRGBA(color);
		}
		static KxColor FromARGB(uint32_t color)
		{
			return KxColor().SetARGB(color);
		}
		static KxColor FromCOLORREF(uint32_t color)
		{
			return KxColor().SetCOLORREF(color);
		}

		// Range: [1, 21]
		static double ContrastRatio(const KxColor& lighterColor, const KxColor& darkerColor);

		// Range: [0, 1]
		static double ColorDifference(const KxColor& c1, const KxColor& c2);

	private:
		static double HUE2RGB(double p, double q, double t);
		static uint8_t GetAlphaCOLORREF(uint32_t rgb);
		static uint32_t MakeCOLORREF(uint8_t R, uint8_t G, uint8_t B, uint8_t A);

	public:
		KxColor()
			:wxColour()
		{
		}
		KxColor(uint8_t R, uint8_t G, uint8_t B, uint8_t A = wxALPHA_OPAQUE)
			:wxColour(R, G, B, A)
		{
		}
		KxColor(uint32_t rgb)
			:wxColour(rgb)
		{
		}
		
		KxColor(const wchar_t* name)
			:wxColour(name)
		{
		}
		KxColor(const char* name)
			:wxColour(name)
		{
		}
		KxColor(const wxString& name)
			:wxColour(name)
		{
		}
		
		KxColor(const KxColor& other);
		KxColor(const wxColour& other);

	public:
		bool IsOk() const override;
		bool IsAlphaOpaque() const;

		KxColor Clone() const;
		wxString GetAsString(long mode = wxC2S_HTML_SYNTAX) const override;
		wxString ToString(C2S mode = C2S::LuaSyntax, C2SAlpha alpha = C2SAlpha::None) const;
		
		KxColor& AlphaBlend(const KxColor& source);
		KxColor& MakeMono(bool isOn = true);
		KxColor& MakeGray(double weightR, double weightG, double weightB);
		KxColor& MakeGraySRGB()
		{
			return MakeGray(SRGB_WeightR, SRGB_WeightG, SRGB_WeightB);
		}
		KxColor& MakeGray();
		KxColor& ChangeLightness(int alpha);
		KxColor& RotateHue(double angle);
		KxColor& Invert();
		KxColor& Negate();

		KxColor AlphaBlend(const KxColor& source) const
		{
			return Clone().AlphaBlend(source);
		}
		KxColor MakeMono(bool isOn = true) const
		{
			return Clone().MakeMono(isOn);
		}
		KxColor MakeGray(double weightR, double weightG, double weightB) const
		{
			return Clone().MakeGray(weightR, weightG, weightB);
		}
		KxColor MakeGraySRGB() const
		{
			return Clone().MakeGraySRGB();
		}
		KxColor MakeGray() const
		{
			return Clone().MakeGray();
		}
		KxColor ChangeLightness(int alpha) const
		{
			return Clone().ChangeLightness(alpha);
		}
		KxColor RotateHue(double angle) const
		{
			return Clone().RotateHue(angle);
		}
		KxColor Invert() const
		{
			return Clone().Invert();
		}
		KxColor Negate() const
		{
			return Clone().Negate();
		}
		KxColor GetContrastColor(const KxColor& lighterColor, const KxColor& darkerColor) const;
		KxColor GetContrastColor(const wxWindow* window) const;

		// Range: [0, 1]
		double GetRelativeLuminance() const;

		// Range: [0, 1]
		double GetBrightness() const;

		uint8_t GetR() const
		{
			return Red();
		}
		uint8_t GetG() const
		{
			return Green();
		}
		uint8_t GetB() const
		{
			return Blue();
		}
		uint8_t GetA() const
		{
			return Alpha();
		}
		
		KxColor& SetR(uint8_t value)
		{
			Set(value, GetG(), GetB(), GetA());
			return *this;
		}
		KxColor& SetG(uint8_t value)
		{
			Set(GetR(), value, GetB(), GetA());
			return *this;
		}
		KxColor& SetB(uint8_t value)
		{
			Set(GetR(), GetB(), value, GetA());
			return *this;
		}
		KxColor& SetA(uint8_t value)
		{
			Set(GetR(), GetG(), GetB(), value);
			return *this;
		}

		uint32_t GetRGBA() const;
		KxColor& SetRGBA(uint32_t color);
		uint32_t GetARGB() const;
		KxColor& SetARGB(uint32_t color);
		uint32_t GetCOLORREF() const;
		KxColor& SetCOLORREF(uint32_t color);
		
		void GetHSV(double& H, double& S, double& V, double* A = nullptr) const;
		void GetHSV(int& H, int& S, int& V, int* A = nullptr) const;
		KxColor& SetHSV(double H, double S, double V, double A = ALPHA_OPAQUE_PERCENT);
		
		void GetHSB(double& H, double& S, double& B, double* A = nullptr) const;
		void GetHSB(int& H, int& S, int& B, int* A = nullptr) const;
		KxColor& SetHSB(double H, double S, double B, double A = ALPHA_OPAQUE_PERCENT);
};

// Standard colors names
/*
AQUAMARINE
BLACK
BLUE
BLUE VIOLET
BROWN
CADET BLUE
CORAL
CORNFLOWER BLUE
CYAN
DARK GREY
DARK GREEN
DARK OLIVE GREEN
DARK ORCHID
DARK SLATE BLUE
DARK SLATE GREY
DARK TURQUOISE
DIM GREY	FIREBRICK
FOREST GREEN
GOLD
GOLDENROD
GREY
GREEN
GREEN YELLOW
INDIAN RED
KHAKI
LIGHT BLUE
LIGHT GREY
LIGHT STEEL BLUE
LIME GREEN
MAGENTA
MAROON
MEDIUM AQUAMARINE
MEDIUM BLUE	MEDIUM FOREST GREEN
MEDIUM GOLDENROD
MEDIUM ORCHID
MEDIUM SEA GREEN
MEDIUM SLATE BLUE
MEDIUM SPRING GREEN
MEDIUM TURQUOISE
MEDIUM VIOLET RED
MIDNIGHT BLUE
NAVY
ORANGE
ORANGE RED
ORCHID
PALE GREEN
PINK
PLUM
PURPLE	RED
SALMON
SEA GREEN
SIENNA
SKY BLUE
SLATE BLUE
SPRING GREEN
STEEL BLUE
TAN
THISTLE
TURQUOISE
VIOLET
VIOLET RED
WHEAT
WHITE
YELLOW
YELLOW GREEN
*/
