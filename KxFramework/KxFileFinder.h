#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxIFileFinder.h"
#include "KxFramework/KxFileItem.h"

class KxFileFinder: public KxIFileFinder
{
	public:
		static bool IsDirectoryEmpty(const wxString& directoryPath);

	private:
		wxEvtHandler* m_EvtHandler = NULL;
		wxString m_SearchQuery;
		bool m_IsCanceled = false;
		bool m_CaseSensitive = false;

		HANDLE m_Handle = INVALID_HANDLE_VALUE;
		WIN32_FIND_DATAW m_FindData = {0};

	private:
		bool OnFound(const WIN32_FIND_DATAW& fileInfo);

	protected:
		virtual bool OnFound(const KxFileItem& foundItem) override;

	public:
		KxFileFinder(const wxString& searchQuery, wxEvtHandler* eventHandler = NULL);
		KxFileFinder(const wxString& source, const wxString& filter, wxEvtHandler* eventHandler = NULL);
		virtual ~KxFileFinder();

	public:
		virtual bool IsOK() const override;
		virtual bool IsCanceled() const override
		{
			return m_IsCanceled;
		}
		
		bool IsCaseSensitive() const
		{
			return m_CaseSensitive;
		}
		void SetCaseSensitive(bool value)
		{
			m_CaseSensitive = value;
		}

		virtual bool Run() override;
		virtual KxFileItem FindNext() override;
		void NotifyFound(const KxFileItem& foundItem)
		{
			m_IsCanceled = !OnFound(foundItem);
		}

		wxString GetSource() const
		{
			return ExtrctSourceFromSearchQuery(m_SearchQuery);
		}
};
