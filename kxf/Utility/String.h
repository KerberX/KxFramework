#pragma once
#include "kxf/Common.hpp"
#include "kxf/General/String.h"
#include <unordered_map>
#include <unordered_set>

namespace kxf::Utility
{
	struct StringEqualToNoCase final
	{
		template<class T>
		bool operator()(T&& left, T&& right) const noexcept
		{
			return String::Compare(std::forward<T>(left), std::forward<T>(right), StringOpFlag::IgnoreCase) == 0;
		}
	};

	struct StringHashNoCase final
	{
		// From Boost
		template<class T>
		static void hash_combine(size_t& seed, const T& v) noexcept
		{
			std::hash<T> hasher;
			seed ^= hasher(v) + static_cast<size_t>(0x9e3779b9u) + (seed << 6) + (seed >> 2);
		}

		template<class T>
		size_t operator()(T&& value) const noexcept
		{
			size_t hashValue = 0;
			for (const auto& c: value)
			{
				hash_combine(hashValue, String::ToLower(c).GetValue());
			}
			return hashValue;
		}
	};

	template<class TKey, class TValue>
	using UnorderedMapNoCase = std::unordered_map<TKey, TValue, StringHashNoCase, StringEqualToNoCase>;
	
	template<class TValue>
	using UnorderedSetNoCase = std::unordered_set<TValue, StringHashNoCase, StringEqualToNoCase>;
}

namespace kxf::Utility
{
	KX_API std::optional<bool> ParseBool(const String& value);
}
