#include "stdafx.h"
#include "ShellFileType.h"
#include "ShellFileTypeManager.h"
#include "Registry.h"
#include "Kx/General/RegEx.h"
#include <wx/regex.h>

namespace KxFramework
{
	FSPath ShellFileType::GetOpenExecutable() const
	{
		String openCommand = GetOpenCommand(NullString);
		if (RegEx regEx(u8R"(\"(.+?)\")", RegExFlag::IgnoreCase); regEx.Matches(openCommand))
		{
			return regEx.GetMatch(openCommand, 1);
		}
		return {};
	}

	bool ShellFileType::IsURLProtocol(const FSPath& extension) const
	{
		if (m_FileType && extension)
		{
			for (const String& ext: GetAllExtensions())
			{
				if (ext == extension)
				{
					RegistryKey key(RegistryBaseKey::ClassesRoot, extension.GetExtension(), RegistryAccess::Read);
					return key.DoesValueExist(wxS("URL Protocol"));
				}
			}
		}
		return false;
	}
}
