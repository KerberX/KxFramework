#pragma once
#include "Common.h"
#include "kxf/General/Version.h"
#include "kxf/General/String.h"
#include <wx/zstream.h>

namespace kxf
{
	enum class ZLibHeaderType
	{
		None = wxZLIB_NO_HEADER,
		Auto = wxZLIB_AUTO,
		ZLib = wxZLIB_ZLIB,
		GZip = wxZLIB_GZIP,
	};
}

namespace kxf::Compression::ZLib
{
	KX_API String GetLibraryName();
	KX_API Version GetLibraryVersion();
}

namespace kxf
{
	class KX_API ZLibInputStream: public wxZlibInputStream
	{
		public:
			ZLibInputStream(wxInputStream& stream, ZLibHeaderType header = ZLibHeaderType::Auto)
				:wxZlibInputStream(stream, ToInt(header))
			{
			}
			ZLibInputStream(std::unique_ptr<wxInputStream> stream, ZLibHeaderType header = ZLibHeaderType::Auto)
				:wxZlibInputStream(stream.release(), ToInt(header))
			{
			}
	};

	class KX_API ZLibOutputStream: public wxZlibOutputStream
	{
		public:
			ZLibOutputStream(wxOutputStream& stream, ZLibHeaderType header = ZLibHeaderType::Auto)
				:wxZlibOutputStream(stream, ToInt(header))
			{
			}
			ZLibOutputStream(std::unique_ptr<wxOutputStream> stream, ZLibHeaderType header = ZLibHeaderType::Auto)
				:wxZlibOutputStream(stream.release(), ToInt(header))
			{
			}
	};
}
