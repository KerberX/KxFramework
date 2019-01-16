#pragma once
#include "KxFramework/KxFramework.h"

namespace Kx::DataView2
{
	enum class CtrlStyle: uint32_t
	{
		None = 0,
		SingleSelection = None,
		MultipleSelection = 1 << 0,
		NoHeader = 1 << 1,
		HorizontalRules = 1 << 2,
		VerticalRules = 1 << 3,
		AlternatingRowColors = 1 << 4,
		VariableRowHeight = 1 << 5,

		DefaultStyle = SingleSelection|VerticalRules
	};
	constexpr inline CtrlStyle operator|(CtrlStyle v1, CtrlStyle v2)
	{
		return static_cast<CtrlStyle>((uint32_t)v1 | (uint32_t)v2);
	}
	constexpr inline bool operator&(CtrlStyle v1, CtrlStyle v2)
	{
		return (uint32_t)v1 & (uint32_t)v2;
	}

	enum class ColumnStyle: uint32_t
	{
		None = 0,
		Sort = 1 << 0,
		Move = 1 << 1,
		Size = 1 << 2,

		DefaultStyle = Move|Size,
	};
	constexpr inline ColumnStyle operator|(ColumnStyle v1, ColumnStyle v2)
	{
		return static_cast<ColumnStyle>((uint32_t)v1 | (uint32_t)v2);
	}
	constexpr inline bool operator&(ColumnStyle v1, ColumnStyle v2)
	{
		return (uint32_t)v1 & (uint32_t)v2;
	}

	class ColumnWidth
	{
		public:
			enum Value: int
			{
				Default = -1,
				AutoSize = -2,
			};

		private:
			int m_Value = Value::AutoSize;

		public:
			ColumnWidth(int value = Value::AutoSize) noexcept
				:m_Value(value)
			{
			}

		public:
			bool IsSpecialValue() const noexcept
			{
				return IsDefault() || IsAutoSize();
			}
			bool IsDefault() const noexcept
			{
				return m_Value == Value::Default;
			}
			bool IsAutoSize() const noexcept
			{
				return m_Value == Value::AutoSize;
			}

			operator int() const noexcept
			{
				return m_Value;
			}
	};

	enum class UniformHeight: int
	{
		Default,
		ListView,
		Explorer
	};
}