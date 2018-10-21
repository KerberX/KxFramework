#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewConstants.h"
#include "KxFramework/DataView/KxDataViewCtrl.h"
#include "KxFramework/DataView/KxDataViewItem.h"
#include <wx/selstore.h>
class KxDataViewCtrl;
class KxDataViewModel;
class KxDataViewVirtualListModel;
class KxDataViewColumn;
class KxDataViewTreeNode;
class KxDataViewRenderer;
class KxDataViewEditor;
class KxDataViewHeaderCtrl;
class KxDataViewEvent;

class KxDataViewMainWindow;
class KxDataViewMainWindowEditorTimer: public wxTimer
{
	private:
		KxDataViewMainWindow* m_Owner = NULL;

	public:
		KxDataViewMainWindowEditorTimer(KxDataViewMainWindow* owner)
			:m_Owner(owner)
		{
		}
		virtual void Notify() override;
};

//////////////////////////////////////////////////////////////////////////
class KxDataViewMainWindow: public wxWindow
{
	friend class KxDataViewRenderer;
	friend class KxDataViewEditor;
	friend class KxDataViewEvent;
	friend class KxDataViewCtrl;
	friend class KxDataViewHeaderCtrl;
	friend class KxDataViewMainWindowEditorTimer;
	friend class KxDataViewMainWindowMaxWidthCalculator;

	public:
		enum: size_t
		{
			INVALID_ROW = (size_t)-1,
			INVALID_COLUMN = (size_t)-1,
			INVALID_COUNT = (size_t)-1,
		};
		enum: int
		{
			// Sort when we're thawed later.
			SortColumn_OnThaw = -3,

			// Don't sort at all.
			SortColumn_None = -2,

			// Sort using the model default sort order.
			SortColumn_Default = -1
		};
		enum: int
		{
			// Cell padding on the left/right
			PADDING_RIGHTLEFT = 3,

			// Expander space margin
			EXPANDER_MARGIN = 4,
			EXPANDER_OFFSET = 4,
		};

	private:
		KxDataViewCtrl* m_Owner;
		int m_UniformLineHeight = 0;
		bool m_Dirty = true;

		KxDataViewColumn* m_CurrentColumn = NULL;
		size_t m_CurrentRow = INVALID_ROW;
		wxSelectionStore m_Selection;

		KxDataViewMainWindowEditorTimer m_EditorTimer;
		bool m_LastOnSame = false;

		bool m_HasFocus = false;
		bool m_UseCellFocus = false;
		bool m_IsCurrentColumnSetByKeyboard = false;
		bool m_IsMouseOverExpander = false;

		size_t m_HotTrackRow = INVALID_ROW;
		bool m_HotTrackRowEnabled = false;
		KxDataViewColumn* m_HotTrackColumn = NULL;

		// Drag and Drop
		size_t m_DragCount = 0;
		wxPoint m_DragStart = wxPoint(0, 0);

		bool m_DragEnabled = false;
		wxDataFormat m_DragFormat;

		bool m_DropEnabled = false;
		wxDataFormat m_DropFormat;
		bool m_DropHint = false;
		size_t m_DropHintLine = INVALID_ROW;

		// Double click logic
		size_t m_LineLastClicked = INVALID_ROW;
		size_t m_LineBeforeLastClicked = INVALID_ROW;
		size_t m_LineSelectSingleOnUp = INVALID_ROW;

		// The pen used to draw horizontal/vertical rules
		wxPen m_PenRuleH;
		wxPen m_PenRuleV;

		// the pen used to draw the expander and the lines
		wxPen m_PenExpander;

		// This is the tree structure of the model.
		// Make m_count = -1 will cause the class recalculate the real displaying number of rows.
		KxDataViewTreeNode* m_TreeRoot = NULL;
		size_t m_ItemsCount = INVALID_COUNT;

		// This is the tree node under the cursor
		KxDataViewTreeNode* m_TreeNodeUnderMouse = NULL;

		// sorted column + extra flags
		int m_Sorting_Column = SortColumn_Default;
		bool m_Sorting_OrderAsc = true;

		// Current editor
		KxDataViewEditor* m_CurrentEditor = NULL;

	protected:
		/* Events */
		void OnChar(wxKeyEvent& event);
		void OnCharHook(wxKeyEvent& event);
		void OnVerticalNavigation(const wxKeyEvent& event, int delta);
		void OnLeftKey(wxKeyEvent& event);
		void OnRightKey(wxKeyEvent& event);
		void OnMouse(wxMouseEvent& event);
		void OnSetFocus(wxFocusEvent& event);
		void OnKillFocus(wxFocusEvent& event);

		// Return false only if the event was vetoed by its handler.
		bool SendExpanderEvent(wxEventType type, const KxDataViewItem& item);
		void SendSelectionChangedEvent(const KxDataViewItem& item, KxDataViewColumn* column = NULL);
		void SendSelectionChangedEvent(size_t row, KxDataViewColumn* column = NULL);

		// Will return true if event allowed
		bool SendEditingStartedEvent(const KxDataViewItem& item, KxDataViewEditor* editor);
		bool SendEditingDoneEvent(const KxDataViewItem& item, KxDataViewEditor* editor, bool canceled, const wxAny& value);

		/* Drawing */
		void OnPaint(wxPaintEvent& event);
		void DrawCellBackground(KxDataViewRenderer* renderer, const wxRect& rect, KxDataViewCellState cellState);
		KxDataViewCellState GetCellStateForRow(size_t row) const;

		/* Drawing */
		// Override the base class method to resort if needed, i.e. if
		// SortPrepare() was called and ignored while we were frozen.
		virtual void DoThaw() override
		{
			if (m_Sorting_Column == SortColumn_OnThaw)
			{
				Resort();
				m_Sorting_Column = SortColumn_None;
			}
			wxWindow::DoThaw();
		}
		void UpdateDisplay();
		void RecalculateDisplay();

		/* Columns */
		void OnColumnsCountChanged();
		KxDataViewColumn* FindColumnForEditing(const KxDataViewItem& item, KxDataViewCellMode mode);
		size_t GetBestColumnWidth(size_t index) const;

		/* Items */
		void InvalidateItemCount()
		{
			m_ItemsCount = INVALID_COUNT;
		}
		void UpdateItemCount(size_t count)
		{
			m_ItemsCount = count;
			m_Selection.SetItemCount(count);
		}
		size_t RecalculateItemCount() const;

		/* Tree nodes */
		void SortPrepare();
		
		KxDataViewTreeNode* FindNode(const KxDataViewItem& item);
		KxDataViewTreeNode* GetTreeNodeByRow(size_t row) const;
		KxDataViewTreeNode* GetTreeNodeByItem(const KxDataViewItem& item) = delete; // We don't need this temporarily

		// Methods for building the mapping tree
		void BuildTreeHelper(const KxDataViewItem& item, KxDataViewTreeNode* node);
		void BuildTree(KxDataViewModel* model);
		void DestroyTree();

		/* Misc */
		virtual void OnInternalIdle() override;
		virtual void OnEditorTimer();

	public:
		KxDataViewMainWindow(KxDataViewCtrl* parent,
							 wxWindowID id,
							 const wxPoint& pos = wxDefaultPosition,
							 const wxSize& size = wxDefaultSize
		);
		virtual ~KxDataViewMainWindow();

	public:
		void CreateEventTemplate(KxDataViewEvent& event, const KxDataViewItem& item = KxDataViewItem(), KxDataViewColumn* column = NULL, bool noCurrent = false);

		void SetOwner(KxDataViewCtrl* owner)
		{
			m_Owner = owner;
		}
		KxDataViewCtrl* GetOwner()
		{
			return m_Owner;
		}
		const KxDataViewCtrl* GetOwner() const
		{
			return m_Owner;
		}
		
		/* Model */
		bool IsList() const
		{
			return GetModel()->IsListModel();
		}
		bool IsVirtualList() const
		{
			return m_TreeRoot == NULL;
		}

		KxDataViewModel* GetModel()
		{
			return GetOwner()->GetModel();
		}
		const KxDataViewModel* GetModel() const
		{
			return GetOwner()->GetModel();
		}
		
		KxDataViewVirtualListModel* GetVirtualListModel();
		const KxDataViewVirtualListModel* GetVirtualListModel() const;

		bool SwapTreeNodes(const KxDataViewItem& item1, const KxDataViewItem& item2);

		/* Notifications */
		bool ItemAdded(const KxDataViewItem& parent, const KxDataViewItem& item);
		bool ItemDeleted(const KxDataViewItem& parent, const KxDataViewItem& item);
		bool ItemChanged(const KxDataViewItem& item);
		bool ValueChanged(const KxDataViewItem& item, KxDataViewColumn* column);
		bool ItemsCleared();
		void Resort();

		/* Rows refreshing */
		void RefreshRow(size_t row);
		void RefreshRows(size_t from, size_t to);
		void RefreshRowsAfter(size_t firstRow);

		/* Item rect */
		wxRect GetLineRect(size_t row) const;
		int GetLineStart(size_t row) const; // 'row' * 'm_lineHeight' in fixed mode
		int GetLineHeight(size_t row) const; // 'm_lineHeight' in fixed mode
		int GetLineHeightModel(const KxDataViewTreeNode* node) const;
		int GetLineHeightModel(size_t row) const;
		size_t GetLineAt(size_t yCoord) const; // 'yCoord' / 'm_lineHeight' in fixed mode
		int GetLineWidth() const;

		int GetUniformRowHeight() const
		{
			return m_UniformLineHeight;
		}
		void SetUniformRowHeight(int height);
		int GetDefaultRowHeight(KxDataViewDefaultRowHeightType type = KxDVC_ROW_HEIGHT_DEFAULT) const;

		/* Drag and Drop */
		wxBitmap CreateItemBitmap(size_t row, int& indent);
		bool EnableDragSource(const wxDataFormat& format);
		bool EnableDropTarget(const wxDataFormat& format);

		void OnDragDropGetRowItem(const wxPoint& pos, size_t& row, KxDataViewItem& item);
		void RemoveDropHint();
		wxDragResult OnDragOver(const wxDataFormat& format, const wxPoint& pos, wxDragResult dragResult);
		wxDragResult OnDragData(const wxDataFormat& format, const wxPoint& pos, wxDragResult dragResult);
		bool OnDrop(const wxDataFormat& format, const wxPoint& pos);
		void OnDragDropLeave();

		/* Scrolling */
		void ScrollWindow(int dx, int dy, const wxRect* rect = NULL);
		void ScrollTo(size_t row, size_t column = INVALID_COLUMN);

		/* Current row and column */
		size_t GetCurrentRow() const
		{
			return m_CurrentRow;
		}
		bool HasCurrentRow()
		{
			return m_CurrentRow != INVALID_ROW;
		}
		void ChangeCurrentRow(size_t row);
		bool TryAdvanceCurrentColumn(KxDataViewTreeNode* node, wxKeyEvent& event, bool moveForward);

		KxDataViewColumn* GetCurrentColumn() const
		{
			return m_CurrentColumn;
		}
		void ClearCurrentColumn()
		{
			m_CurrentColumn = NULL;
		}

		KxDataViewItem GetHotTrackItem() const;
		KxDataViewColumn* GetHotTrackColumn() const;

		/* Selection */
		bool IsSingleSelection() const
		{
			return !GetParent()->HasFlag(KxDV_MULTIPLE_SELECTION);
		}
		bool IsMultipleSelection() const
		{
			return !IsSingleSelection();
		}
		bool IsEmpty()
		{
			return GetRowCount() == 0;
		}

		const wxSelectionStore& GetSelections() const
		{
			return m_Selection;
		}
		bool IsSelectionEmpty() const
		{
			return m_Selection.IsEmpty();
		}

		// If a valid row is specified and it was previously selected, it is left
		// selected and the function returns false. Otherwise, i.e. if there is
		// really no selection left in the control, it returns true.
		bool UnselectAllRows(size_t exceptThisRow = INVALID_ROW);

		void ClearSelection()
		{
			m_Selection.SelectRange(0, GetRowCount() - 1, false);
		}
		void SelectRow(size_t row, bool select);
		void SelectRows(size_t from, size_t to);
		void SelectRows(const KxIntPtrVector& selection);
		void SelectAllRows()
		{
			m_Selection.SelectRange(0, GetRowCount() - 1);
			Refresh();
		}
		void ReverseRowSelection(size_t row);
		bool IsRowSelected(size_t row) const
		{
			return m_Selection.IsSelected(row);
		}

		/* View */
		void SetRuleHPen(const wxPen& pen)
		{
			m_PenRuleH = pen;
		}
		void SetRuleVPen(const wxPen& pen)
		{
			m_PenRuleV = pen;
		}

		size_t GetCountPerPage() const;
		int GetEndOfLastCol() const;
		size_t GetFirstVisibleRow() const;

		// I changed this method to non-const because in the tree view,
		// the displaying number of the tree are changing along with the
		// expanding/collapsing of the tree nodes
		// ???
		size_t GetLastVisibleRow() const;
		size_t GetRowCount() const;

		void HitTest(const wxPoint& pos, KxDataViewItem& item, KxDataViewColumn*& column);
		wxRect GetItemRect(const KxDataViewItem& item, const KxDataViewColumn* column);

		// Adjust last column to window size
		void UpdateColumnSizes();

		/* Rows */
		void Expand(size_t row);
		void Collapse(size_t row);
		void ToggleExpand(size_t row)
		{
			IsExpanded(row) ? Collapse(row) : Expand(row);
		}
		bool IsExpanded(size_t row) const;
		bool HasChildren(size_t row) const;

		// Some useful functions for row and item mapping
		KxDataViewItem GetItemByRow(size_t row) const;
		size_t GetRowByItem(const KxDataViewItem& item) const;

		// Called by wxDataViewCtrl and our own OnRenameTimer() to start edit the specified item in the given column.
		bool IsCellEditable(const KxDataViewItem& item, const KxDataViewColumn* column, KxDataViewCellMode mode) const;
		
		bool BeginEdit(const KxDataViewItem& item, const KxDataViewColumn* column);
		void EndEdit();
		void CancelEdit();

		/* Columns */
		int GetSortColumn() const
		{
			return m_Sorting_Column;
		}
		bool IsAscendingSort() const
		{
			return m_Sorting_OrderAsc;
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxDataViewMainWindow);
};

//////////////////////////////////////////////////////////////////////////
class wxDragImage;
class KxDataViewMainWindowDropSource: public wxDropSource
{
	private:
		KxDataViewMainWindow* m_MainWindow = NULL;
		wxWindow* m_DragImage = NULL;
		wxBitmap m_HintBitmap;
		wxPoint m_HintPosition;

		wxPoint m_Distance = wxDefaultPosition;
		size_t m_Row = KxDataViewMainWindow::INVALID_ROW;

	private:
		void OnPaint(wxPaintEvent& event);
		void OnScroll(wxMouseEvent& event);
		virtual bool GiveFeedback(wxDragResult effect) override;

		wxPoint GetHintPosition(const wxPoint& mousePos) const;

	public:
		KxDataViewMainWindowDropSource(KxDataViewMainWindow* mainWindow, size_t row);
		virtual ~KxDataViewMainWindowDropSource();
};

class KxDataViewMainWindowDropTarget: public wxDropTarget
{
	private:
		KxDataViewMainWindow* m_MainWindow = NULL;

	private:
		virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult dragResult) override;
		virtual bool OnDrop(wxCoord x, wxCoord y) override;
		virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult dragResult) override;
		virtual void OnLeave() override;

	public:
		KxDataViewMainWindowDropTarget(wxDataObject* dataObject, KxDataViewMainWindow* mainWindow);
};
