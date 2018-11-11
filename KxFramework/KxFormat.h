/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include <string>
#include <string_view>

namespace
{
	constexpr const wxChar DefaultFillChar = wxS(' ');
	constexpr const wxChar DefaultFloatFormat = wxS('f');

	template<class T> inline constexpr bool IsCharType = std::is_same_v<T, char> || std::is_same_v<T, wchar_t>;
	template<class T> inline constexpr bool IsCharPointer =
		std::is_same_v<T, char*> ||
		std::is_same_v<T, wchar_t*> ||
		std::is_same_v<T, const char*> ||
		std::is_same_v<T, const wchar_t*>;

	template<class T> inline constexpr bool FmtString =
		IsCharType<T> ||
		IsCharPointer<T> ||
		std::is_same_v<T, bool> ||
		std::is_same_v<T, std::string_view> ||
		std::is_same_v<T, std::wstring_view> ||
		std::is_constructible_v<wxString, T>;

	template<class T> inline constexpr bool FmtInteger = (std::is_integral_v<T> || std::is_enum_v<T>) && !(IsCharType<T> || IsCharPointer<T> || std::is_pointer_v<T> || std::is_same_v<T, bool>);
	template<class T> inline constexpr bool FmtFloat = std::is_floating_point_v<T>;
	template<class T> inline constexpr bool FmtPointer = std::is_pointer_v<T> && !IsCharPointer<T>;
}

class KxFormat
{
	private:
		wxString m_String;
		size_t m_CurrentArgument = 1;
		bool m_UpperCase = false;

	private:
		bool FindAndReplace(const wxString& string, const std::wstring_view& index, size_t startAt = 0);
		void FindAndReplace(const wxString& string, size_t index, size_t startAt = 0);
		void FindCurrentAndReplace(const wxString& string);

	private:
		// Strings
		KxFormat& argString(const wxString& a, int fieldWidth = 0, const wxUniChar& fillChar = DefaultFillChar);
		KxFormat& argChar(const wxUniChar& a, int fieldWidth = 0, const wxUniChar& fillChar = DefaultFillChar);

		// Integers
		KxFormat& argInt(int8_t a, int fieldWidth = 0, int base = 10, const wxUniChar& fillChar = DefaultFillChar);
		KxFormat& argInt(uint8_t a, int fieldWidth = 0, int base = 10, const wxUniChar& fillChar = DefaultFillChar);

		KxFormat& argInt(int16_t a, int fieldWidth = 0, int base = 10, const wxUniChar& fillChar = DefaultFillChar);
		KxFormat& argInt(uint16_t a, int fieldWidth = 0, int base = 10, const wxUniChar& fillChar = DefaultFillChar);

		KxFormat& argInt(int32_t a, int fieldWidth = 0, int base = 10, const wxUniChar& fillChar = DefaultFillChar);
		KxFormat& argInt(uint32_t a, int fieldWidth = 0, int base = 10, const wxUniChar& fillChar = DefaultFillChar);

		KxFormat& argInt(int64_t a, int fieldWidth = 0, int base = 10, const wxUniChar& fillChar = DefaultFillChar);
		KxFormat& argInt(uint64_t a, int fieldWidth = 0, int base = 10, const wxUniChar& fillChar = DefaultFillChar);

		KxFormat& argPointer(const void* a, bool add0x = false, int fieldWidth = 0, const wxUniChar& fillChar = DefaultFillChar);
		KxFormat& argBool(bool a, int fieldWidth = 0, const wxUniChar& fillChar = DefaultFillChar);

		// Floats
		KxFormat& argDouble(double a, int precision = -1, int fieldWidth = 0, const wxUniChar& format = DefaultFloatFormat, const wxUniChar& fillChar = DefaultFillChar);

	public:
		KxFormat(const wxString& format)
			:m_String(format)
		{
		}

	public:
		wxString ToString() const
		{
			return m_String;
		}
		operator wxString() const
		{
			return m_String;
		}

		// Switches
		KxFormat& UpperCase(bool value = true)
		{
			m_UpperCase = value;
			return *this;
		}
		KxFormat& LowerCase(bool value = true)
		{
			m_UpperCase = !value;
			return *this;
		}

	public:
		template<class T> typename std::enable_if<FmtString<T>, KxFormat&>::type
		arg(const T& a, int fieldWidth = 0, const wxUniChar& fillChar = DefaultFillChar)
		{
			if constexpr(std::is_same_v<T, bool>)
			{
				return argBool(a, fieldWidth, fillChar);
			}
			else if constexpr(std::is_constructible_v<wxUniChar, T>)
			{
				return argChar(a, fieldWidth, fillChar);
			}
			else if constexpr(std::is_constructible_v<wxString, T>)
			{
				return argString(a, fieldWidth, fillChar);
			}
			else if constexpr(std::is_same_v<T, std::string_view> || std::is_same_v<T, std::wstring_view>)
			{
				return argString(wxString(a.data(), a.size()), fieldWidth, fillChar);
			}
			else
			{
				static_assert(false, "KxFormat: unsupported type for string formatting");
			}
			return *this;
		}
		
		template<class T> typename std::enable_if<FmtInteger<T>, KxFormat&>::type
		arg(const T& a, int fieldWidth = 0, int base = 10, const wxUniChar& fillChar = DefaultFillChar)
		{
			if constexpr(std::is_integral_v<T>)
			{
				return argInt(a, fieldWidth, base, fillChar);
			}
			else if constexpr(std::is_enum_v<T>)
			{
				return argInt(static_cast<std::underlying_type_t<T>>(a), fieldWidth, base, fillChar);
			}
			else
			{
				static_assert(false, "KxFormat: unsupported type for integer formatting");
			}
			return *this;
		}

		template<class T> typename std::enable_if<FmtFloat<T>, KxFormat&>::type
		arg(const T& a, int precision = -1, int fieldWidth = 0, const wxUniChar& format = DefaultFloatFormat, const wxUniChar& fillChar = DefaultFillChar)
		{
			if constexpr(std::is_floating_point_v<T>)
			{
				return argDouble(a, precision, fieldWidth, format, fillChar);
			}
			else
			{
				static_assert(false, "KxFormat: unsupported type for floating-point formatting");
			}
			return *this;
		}

		template<class T> typename std::enable_if<FmtPointer<T>, KxFormat&>::type
		arg(const T& a, bool add0x = false, int fieldWidth = 0, const wxUniChar& fillChar = DefaultFillChar)
		{
			if constexpr(std::is_pointer_v<T>)
			{
				return argPointer(a, add0x, fieldWidth, fillChar);
			}
			else
			{
				static_assert(false, "KxFormat: unsupported type for pointer formatting");
			}
			return *this;
		}
};
