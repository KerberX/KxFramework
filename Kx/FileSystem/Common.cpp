#include "KxStdAfx.h"
#include "Common.h"
#include "NamespacePrefix.h"

namespace KxFramework::FileSystem
{
	wxString GetNamespaceString(FSPathNamespace type)
	{
		switch (type)
		{
			case FSPathNamespace::NT:
			{
				return NamespacePrefix::NT;
			}
			case FSPathNamespace::Win32File:
			{
				return NamespacePrefix::Win32File;
			}
			case FSPathNamespace::Win32FileUNC:
			{
				return NamespacePrefix::Win32FileUNC;
			}
			case FSPathNamespace::Win32Device:
			{
				return NamespacePrefix::Win32Device;
			}
			case FSPathNamespace::Network:
			{
				return NamespacePrefix::Network;
			}
			case FSPathNamespace::NetworkUNC:
			{
				return NamespacePrefix::NetworkUNC;
			}
		};
		return wxEmptyString;
	}
	
	wxString GetForbiddenChars()
	{
		return wxFileName::GetForbiddenChars();
	}
	wxString GetForbiddenCharsExceptSeparators()
	{
		wxString forbiddenChars = FileSystem::GetForbiddenChars();
		forbiddenChars.Replace(wxS('\\'), wxEmptyString);
		forbiddenChars.Replace(wxS('/'), wxEmptyString);

		return forbiddenChars;
	}
}
