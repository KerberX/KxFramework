#pragma once
#include "kxf/Common.hpp"
#include "kxf/General/String.h"
#include "kxf/General/DateTime.h"

namespace kxf
{
	class URI;
	class URL;

	enum class NetworkHostType
	{
		None = -1,

		RegName,
		IPvUnknown,
		IPv4,
		IPv6,
	};

	enum class URIFlag: uint32_t
	{
		None = 0,
		Strict = 1 << 0
	};
	KxFlagSet_Declare(URIFlag);

	enum class URLStatus
	{
		Success = 0,
		Unknown,
		Syntax,
		NoProtocol,
		NoHost,
		NoPath,
		ConnectionError,
		ProtocolError
	};
}

namespace kxf::Network
{
	KX_API bool IsInternetAvailable() noexcept;
	KX_API String LookupIP(const URI& uri, NetworkHostType ip);
}
