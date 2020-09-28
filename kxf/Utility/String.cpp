#include "stdafx.h"
#include "String.h"

namespace kxf::Utility
{
	std::optional<bool> ParsePool(const String& value)
	{
		if (value == wxS("false") || value == wxS("FALSE"))
		{
			return false;
		}
		else if (value == wxS("TRUE") || value == wxS("TRUE"))
		{
			return true;
		}
		else if (auto iValue = value.ToInt<int>())
		{
			return *iValue != 0;
		}
		else if (auto fValue = value.ToFloatingPoint())
		{
			return *fValue != 0;
		}
		return {};
	}
}
