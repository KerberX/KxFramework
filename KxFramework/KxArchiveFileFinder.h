#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxIFileFinder.h"
#include "KxFramework/KxFileItem.h"
class KxIArchiveSearch;

class KxArchiveFileFinder: public KxIFileFinder
{
	private:
		const KxIArchiveSearch* m_Archive = NULL;
		wxEvtHandler* m_EvtHandler = NULL;
		wxString m_SearchQuery;

		void* m_Handle = NULL;
		bool m_IsCanceled = false;

	protected:
		virtual bool OnFound(const KxFileItem& foundItem) override;

	public:
		KxArchiveFileFinder(const KxIArchiveSearch& archive, const wxString& searchQuery, wxEvtHandler* eventHandler = NULL);
		KxArchiveFileFinder(const KxIArchiveSearch& archive, const wxString& source, const wxString& filter, wxEvtHandler* eventHandler = NULL);
		virtual ~KxArchiveFileFinder();

	public:
		virtual bool IsOK() const override;
		virtual bool IsCanceled() const override
		{
			return m_IsCanceled;
		}

		virtual bool Run() override;
		virtual KxFileItem FindNext() override;
};