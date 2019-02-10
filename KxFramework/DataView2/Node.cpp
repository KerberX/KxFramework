#include "KxStdAfx.h"
#include "Node.h"
#include "MainWindow.h"
#include "View.h"

namespace KxDataView2
{
	class Comparator
	{
		private:
			const View* m_View = nullptr;
			const SortOrder m_SortOrder;

		public:
			Comparator(const MainWindow* mainWindow, const SortOrder& sortOrder)
				:m_SortOrder(sortOrder), m_View(mainWindow->GetView())
			{
			}

		public:
			bool operator()(const Node* node1, const Node* node2) const
			{
				const Column* sortingColumn = m_View->GetSortingColumn();
				if (sortingColumn)
				{
					const bool multiColumnSort = m_View->IsMultiColumnSortUsed();
					const bool sortAscending = m_SortOrder.IsAscending();

					const bool isLess = node1->Compare(*node2, *sortingColumn);
					if (!multiColumnSort)
					{
						return sortAscending ? isLess : !isLess;
					}
					return isLess;
				}
				return false;
			}
	};
}

namespace KxDataView2
{
	void Node::PutChildInSortOrder(Node* node)
	{
		// The childNode has changed, and may need to be moved to another location in the sorted child list.
		if (IsNodeExpanded() && !m_SortOrder.IsNone())
		{
			// This is more than an optimization, the code below assumes that 1 is a valid index.
			if (m_Children.size() > 1)
			{
				// We should already be sorted in the right order.
				wxASSERT(m_SortOrder == window->GetSortOrder());

				// First find the node in the current child list
				intptr_t oldLocation = -1;
				for (size_t index = 0; index < m_Children.size(); ++index)
				{
					if (m_Children[index] == node)
					{
						oldLocation = index;
						break;
					}
				}
				if (oldLocation < 0)
				{
					// Not our child?
					return;
				}

				// Check if we actually need to move the node.
				Comparator comparator(m_RootNode->GetMainWindow(), m_SortOrder);
				bool locationChanged = false;

				if (oldLocation == 0)
				{
					// Compare with the next item (as we return early in the case of only a single child,
					// we know that there is one) to check if the item is now out of order.
					if (!comparator(node, m_Children[1]))
					{
						locationChanged = true;
					}
				}
				else
				{
					// Compare with the previous item.
					if (!comparator(m_Children[oldLocation - 1], node))
					{
						locationChanged = true;
					}
				}
				if (!locationChanged)
				{
					return;
				}

				// Remove and reinsert the node in the child list
				m_Children.erase(m_Children.begin() + oldLocation);

				// Use binary search to find the correct position to insert at.
				auto it = std::lower_bound(m_Children.begin(), m_Children.end(), node, comparator);
				m_Children.insert(it, node);
				RecalcIndexes(std::distance(m_Children.begin(), it));

				// Make sure the change is actually shown right away
				m_RootNode->GetMainWindow()->UpdateDisplay();
			}
		}
	}
	void Node::Resort()
	{
		if (IsNodeExpanded())
		{
			const SortOrder sortOrder = m_RootNode->GetMainWindow()->GetSortOrder();
			if (!sortOrder.IsNone())
			{
				// Only sort the children if they aren't already sorted by the wanted criteria.
				if (m_SortOrder != sortOrder)
				{
					std::sort(m_Children.begin(), m_Children.end(), Comparator(m_RootNode->GetMainWindow(), sortOrder));
					m_SortOrder = sortOrder;
					RecalcIndexes();
				}

				// There may be open child nodes that also need a resort.
				for (Node* childNode: m_Children)
				{
					if (childNode->HasChildren())
					{
						childNode->Resort();
					}
				}
			}
		}
	}

	void Node::CalcSubTreeCount()
	{
		if (!IsRootNode() && HasChildren())
		{
			intptr_t count = 0;
			for (const Node* node: m_Children)
			{
				count += node->GetSubTreeCount() + 1;
			}

			if (m_IsExpanded)
			{
				ChangeSubTreeCount(-count);
			}
			else
			{
				ChangeSubTreeCount(+count);

				// Sort the children if needed
				Resort();
			}
		}
	}
	intptr_t Node::GetSubTreeCount() const
	{
		return m_SubTreeCount;
	}
	void Node::ChangeSubTreeCount(intptr_t num)
	{
		m_SubTreeCount += num;

		if (m_ParentNode)
		{
			m_ParentNode->ChangeSubTreeCount(num);
		}
	}
	void Node::InitNodeFromThis(Node& node)
	{
		node.m_ParentNode = this;
		node.m_RootNode = m_RootNode;
		node.m_SortOrder = m_SortOrder;
	}
	void Node::RecalcIndexes(size_t startAt)
	{
		for (size_t i = startAt; i < m_Children.size(); ++i)
		{
			m_Children[i]->m_IndexWithinParent = i;
		}
	}

	Node::~Node()
	{
		if (Model* model = GetModel())
		{
			for (Node* node: m_Children)
			{
				model->OnDeleteNode(node);
			}
		}
	}
	Row Node::FindChild(const Node& node) const
	{
		auto it = std::find(m_Children.begin(), m_Children.end(), &node);
		if (it != m_Children.end())
		{
			return std::distance(m_Children.begin(), it);
		}
		return Row();
	}
	int Node::GetIndentLevel() const
	{
		if (!IsRootNode())
		{
			int level = 0;

			const Node* node = this;
			while (node->GetParent()->GetParent())
			{
				node = node->GetParent();
				level++;
			}
			return level;
		}
		return -1;
	}

	void Node::DetachAllChildren()
	{
		m_Children.clear();
	}
	void Node::AttachChild(Node* node, size_t index)
	{
		// Flag indicating whether we should retain existing sorted list when inserting the child node.
		bool shouldInsertSorted = false;
		SortOrder controlSortOrder = m_RootNode->GetMainWindow()->GetSortOrder();

		if (controlSortOrder.IsNone())
		{
			// We should insert assuming an unsorted list. This will cause the child list to lose the current sort order, if any.
			ResetSortOrder();
		}
		else if (!HasChildren())
		{
			if (IsNodeExpanded())
			{
				// We don't need to search for the right place to insert the first item (there is only one),
				// but we do need to remember the sort order to use for the subsequent ones.
				m_SortOrder = controlSortOrder;
			}
			else
			{
				// We're inserting the first child of a closed node. We can choose whether to consider this empty
				// child list sorted or unsorted. By choosing unsorted, we postpone comparisons until the parent
				// node is opened in the view, which may be never.
				ResetSortOrder();
			}
		}
		else if (IsNodeExpanded())
		{
			// For open branches, children should be already sorted.
			wxASSERT_MSG(m_SortOrder == controlSortOrder, wxS("Logic error in DataView2 sorting code"));

			// We can use fast insertion.
			shouldInsertSorted = true;
		}
		else if (m_SortOrder == controlSortOrder)
		{
			// The children are already sorted by the correct criteria (because the node must have been opened
			// in the same time in the past). Even though it is closed now, we still insert in sort order to
			// avoid a later resort.
			shouldInsertSorted = true;
		}
		else
		{
			// The children of this closed node aren't sorted by the correct criteria, so we just insert unsorted.
			ResetSortOrder();
		}

		InitNodeFromThis(*node);
		if (shouldInsertSorted)
		{
			// Use binary search to find the correct position to insert at.
			auto it = std::lower_bound(m_Children.begin(), m_Children.end(), node, Comparator(m_RootNode->GetMainWindow(), controlSortOrder));
			m_Children.insert(it, node);
			RecalcIndexes(std::distance(m_Children.begin(), it));
		}
		else
		{
			index = std::min(index, m_Children.size());
			m_Children.insert(m_Children.begin() + index, node);
			RecalcIndexes(index);
		}
		m_RootNode->GetMainWindow()->OnNodeAdded(*this);
	}
	void Node::InsertChild(Node* node, size_t index)
	{
		AttachChild(node, index);
	}

	Node* Node::DetachChild(size_t index)
	{
		if (index < m_Children.size())
		{
			auto it = m_Children.begin() + index;
			Node* node = *it;
			node->m_ParentNode = nullptr;
			m_Children.erase(it);
			RecalcIndexes(index);

			const intptr_t removedCount = node->GetSubTreeCount() + 1;
			if (MainWindow* mainWindow = GetMainWindow())
			{
				mainWindow->OnNodeRemoved(*node, removedCount);
			}
			return node;
		}
		return nullptr;
	}
	Node* Node::DetachChild(Node& node)
	{
		if (Row index = FindChild(node); index.IsOK())
		{
			return DetachChild(index);
		}
		return nullptr;
	}
	Node* Node::Detach()
	{
		return m_ParentNode->DetachChild(*this);
	}
	
	void Node::RemoveChild(Node& node)
	{
		if (Row index = FindChild(node); index.IsOK())
		{
			RemoveChild(index);
		}
	}
	void Node::RemoveChild(size_t index)
	{
		Model* model = GetModel();
		if (Node* node = DetachChild(index); node && model)
		{
			model->OnDeleteNode(node);
		}
	}
	void Node::Remove()
	{
		m_ParentNode->RemoveChild(*this);
	}
	bool Node::Swap(Node& otherNode)
	{
		if (this != &otherNode && m_ParentNode == otherNode.GetParent())
		{
			auto it = m_Children.begin() + m_IndexWithinParent;
			auto otherIt = otherNode.m_Children.begin() + otherNode.m_IndexWithinParent;
			std::iter_swap(it, otherIt);
			return true;
		}
		return false;
	}
}

namespace KxDataView2
{
	MainWindow* Node::GetMainWindow() const
	{
		return m_RootNode ? m_RootNode->GetMainWindow() : nullptr;
	}
	View* Node::GetView() const
	{
		return m_RootNode ? m_RootNode->GetView() : nullptr;
	}
	Model* Node::GetModel() const
	{
		return m_RootNode ? m_RootNode->GetModel() : nullptr;
	}
	bool Node::IsRenderable(const Column& column) const
	{
		if (MainWindow* mainWindow = GetMainWindow())
		{
			return &GetRenderer(column) != &mainWindow->GetNullRenderer();
		}
		return false;
	}

	bool Node::IsExpanded() const
	{
		return IsNodeExpanded() && HasChildren();
	}
	void Node::SetExpanded(bool expanded)
	{
		if (MainWindow* mainWindow = GetMainWindow())
		{
			if (expanded)
			{
				mainWindow->Expand(*this);
			}
			else
			{
				mainWindow->Collapse(*this);
			}
		}
	}
	void Node::Expand()
	{
		if (View* view = GetView())
		{
			view->Expand(*this);
		}
	}
	void Node::Collapse()
	{
		if (View* view = GetView())
		{
			view->Collapse(*this);
		}
	}
	void Node::ToggleExpanded()
	{
		SetExpanded(!IsExpanded());
	}

	void Node::Refresh()
	{
		if (MainWindow* mainWindow = GetMainWindow())
		{
			mainWindow->OnCellChanged(*this, nullptr);
		}
	}
	void Node::Refresh(Column& column)
	{
		if (MainWindow* mainWindow = GetMainWindow())
		{
			mainWindow->OnCellChanged(*this, &column);
		}
	}
	void Node::Edit(Column& column)
	{
		if (MainWindow* mainWindow = GetMainWindow())
		{
			mainWindow->BeginEdit(*this, column);
		}
	}

	Row Node::GetRow() const
	{
		if (MainWindow* mainWindow = GetMainWindow())
		{
			return mainWindow->GetRowByNode(*this);
		}
		return {};
	}
	bool Node::IsSelected() const
	{
		if (View* view = GetView())
		{
			return view->IsSelected(*this);
		}
		return false;
	}
	bool Node::IsCurrent() const
	{
		if (View* view = GetView())
		{
			return view->GetCurrentItem() == this;
		}
		return false;
	}
	bool Node::IsHotTracked() const
	{
		if (View* view = GetView())
		{
			return view->GetHotTrackedItem() == this;
		}
		return false;
	}
	void Node::SetSelected(bool value)
	{
		if (View* view = GetView())
		{
			if (value)
			{
				view->Select(*this);
			}
			else
			{
				view->Unselect(*this);
			}
		}
	}
	void Node::EnsureVisible(const Column* column)
	{
		if (View* view = GetView())
		{
			view->EnsureVisible(*this, column);
		}
	}

	wxRect Node::GetCellRect(const Column* column) const
	{
		if (View* view = GetView())
		{
			return view->GetAdjustedItemRect(*this, column);
		}
		return {};
	}
	wxPoint Node::GetDropdownMenuPosition(const Column* column) const
	{
		if (View* view = GetView())
		{
			return view->GetDropdownMenuPosition(*this, column);
		}
		return {};
	}
}

namespace KxDataView2
{
	bool NodeOperation::DoWalk(Node& node, NodeOperation& func)
	{
		switch (func(node))
		{
			case Result::Done:
			{
				return true;
			}
			case Result::SkipSubTree:
			{
				return false;
			}
			case Result::Continue:
			{
				break;
			}
		};

		if (node.HasChildren())
		{
			for (Node* childNode: node.GetChildren())
			{
				if (DoWalk(*childNode, func))
				{
					return true;
				}
			}
		}
		return false;
	}
}

namespace KxDataView2
{
	void RootNode::ResetAll()
	{
		if (Model* model = GetModel())
		{
			model->OnDetachRootNode(*this);
		}

		Node::ResetAll();
		Init();
	}

	View* RootNode::GetView() const
	{
		return m_MainWindow ? m_MainWindow->GetView() : nullptr;
	}
	Model* RootNode::GetModel() const
	{
		return m_MainWindow ? m_MainWindow->GetModel() : nullptr;
	}
}

namespace KxDataView2
{
	NodeOperation::Result NodeOperation_RowToNode::operator()(Node& node)
	{
		m_CurrentRow++;
		if (m_CurrentRow == m_Row)
		{
			m_ResultNode = &node;
			return Result::Done;
		}

		if (node.GetSubTreeCount() + m_CurrentRow < m_Row)
		{
			m_CurrentRow += node.GetSubTreeCount();
			return Result::SkipSubTree;
		}
		else
		{
			// If the current node has only leaf children, we can find the desired node directly.
			// This can speed up finding the node in some cases, and will have a very good effect for list views.
			if (node.HasChildren() && node.GetChildren().size() == (size_t)node.GetSubTreeCount())
			{
				const size_t index = m_Row - m_CurrentRow - 1;
				if (index < node.GetChildrenCount())
				{
					m_ResultNode = node.GetChildren()[index];
					return Result::Done;
				}
			}
			return Result::Continue;
		}
	}
}
