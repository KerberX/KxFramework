#include "KxStdAfx.h"
#include "KxFramework/KxFileItem.h"
#include "KxFramework/KxFileFinder.h"

void KxFileItem::MakeNull(bool attribuesOnly)
{
	if (attribuesOnly)
	{
		m_Attributes = INVALID_FILE_ATTRIBUTES;
		m_ReparsePointAttributes = 0;
		m_CreationTime = wxDefaultDateTime;
		m_LastAccessTime = wxDefaultDateTime;
		m_ModificationTime = wxDefaultDateTime;
		m_FileSize = -1;
	}
	else
	{
		*this = KxFileItem();
	}
}
void KxFileItem::Set(const WIN32_FIND_DATAW& fileInfo)
{
	m_Attributes = fileInfo.dwFileAttributes;
	if (IsReparsePoint())
	{
		m_ReparsePointAttributes = fileInfo.dwReserved0;
	}

	m_FileSize = -1;
	if (IsFile())
	{
		ULARGE_INTEGER size = {0};
		size.HighPart = fileInfo.nFileSizeHigh;
		size.LowPart = fileInfo.nFileSizeLow;
		m_FileSize = (int64_t)size.QuadPart;
	}

	m_Name.reserve(ARRAYSIZE(fileInfo.cFileName));
	m_Name = fileInfo.cFileName;

	SetTime(fileInfo.ftCreationTime, m_CreationTime);
	SetTime(fileInfo.ftLastAccessTime, m_LastAccessTime);
	SetTime(fileInfo.ftLastWriteTime, m_ModificationTime);
}
void KxFileItem::SetTime(const FILETIME& fileTime, wxDateTime& fileTimeWx) const
{
	if (fileTime.dwHighDateTime != 0 && fileTime.dwLowDateTime != 0)
	{
		SYSTEMTIME systemTime = {0};
		::FileTimeToSystemTime(&fileTime, &systemTime);
		fileTimeWx.SetFromMSWSysTime(systemTime);
	}
}

KxFileItem::KxFileItem(const wxString& fullPath)
{
	m_Source = fullPath.BeforeLast(wxS('\\'), &m_Name);
	UpdateInfo();
}
KxFileItem::KxFileItem(const wxString& source, const wxString& fileName)
	:m_Source(source), m_Name(fileName)
{
	UpdateInfo();
}
KxFileItem::KxFileItem(KxFileFinder* finder, const WIN32_FIND_DATAW& fileInfo)
	:m_Source(finder->GetSource())
{
	Set(fileInfo);
}
KxFileItem::~KxFileItem()
{
}

bool KxFileItem::IsCurrentOrParent() const
{
	return m_Name == wxS("..") || m_Name == wxS(".");
}
bool KxFileItem::IsDirectoryEmpty() const
{
	return IsDirectory() && KxFileFinder::IsDirectoryEmpty(m_Source);
}

bool KxFileItem::UpdateInfo()
{
	wxString query = m_Source + wxS('\\') + m_Name;

	WIN32_FIND_DATAW info = {0};
	HANDLE searchHandle = ::FindFirstFileExW(query, FindExInfoBasic, &info, FindExSearchNameMatch, NULL, 0);
	if (searchHandle != INVALID_HANDLE_VALUE)
	{
		Set(info);
		::FindClose(searchHandle);
		return true;
	}
	else
	{
		MakeNull(true);
		return false;
	}
}