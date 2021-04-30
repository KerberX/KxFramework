#include "KxfPCH.h"
#include "IWebResponse.h"
#include "WebRequestHeader.h"
#include "kxf/IO/NullStream.h"
#include "kxf/IO/StreamReaderWriter.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/Serialization/JSON.h"
#include "kxf/Serialization/XML.h"

namespace
{
	kxf::NullWebResponse g_NullWebResponse;
}

namespace kxf
{
	String IWebResponse::GetAsString() const
	{
		if (auto stream = GetStream())
		{
			IO::InputStreamReader reader(*stream);
			return reader.ReadStringUTF8(stream->GetSize().ToBytes());
		}
		return {};
	}
	JSONDocument IWebResponse::GetAsJSON() const
	{
		if (auto stream = GetStream())
		{
			return *stream;
		}
		return {};
	}
	XMLDocument IWebResponse::GetAsXML() const
	{
		if (auto stream = GetStream())
		{
			return *stream;
		}
		return {};
	}
}

namespace kxf
{
	IWebResponse& NullWebResponse::Get()
	{
		return g_NullWebResponse;
	}

	Enumerator<WebRequestHeader> NullWebResponse::EnumHeaders() const
	{
		return {};
	}
	Enumerator<String> NullWebResponse::EnumCookies() const
	{
		return {};
	}

	FSPath NullWebResponse::GetSuggestedFilePath() const
	{
		return {};
	}
	std::unique_ptr<IInputStream> NullWebResponse::GetStream() const
	{
		return nullptr;
	}
}
