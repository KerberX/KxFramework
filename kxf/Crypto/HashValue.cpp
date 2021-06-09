#include "KxfPCH.h"
#include "HashValue.h"
#include "kxf/General/String.h"
#include "kxf/General/Format.h"

namespace kxf::Crypto::Private
{
	String HashValueToString(std::span<const uint8_t> data)
	{
		String result;
		result.reserve(data.size() * 2);

		for (uint8_t x: data)
		{
			result += Format("{:02x}", x);
		};
		return result;
	}
}
