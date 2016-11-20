#include "TTreeViewContent.h"

#include "TMainWindow.h"
#include "TShape.h"

extern TMainWindow win;
TTreeViewContent::TTreeViewContent()
{
	pShape = &(win.m_Shape);
}


TTreeViewContent::~TTreeViewContent()
{
}

void TTreeViewContent::DeleteAllItems()
{
	TreeView_DeleteAllItems(m_hWnd);
	Item.swap(std::vector<TItem>());
}

void TTreeViewContent::DeleteById(int id)
{
	for (int i = 0; i < Item.size(); i++)
	{
		if (Item[i].ObjectId == id)
		{
			SelectNull();
			TreeView_DeleteItem(m_hWnd, Item[i].hTreeItem);
			std::vector<TItem>::iterator iter = Item.begin() + i;
			Item.erase(iter);
		}
	}
}

void TTreeViewContent::AddItem(TElement *Element, int id)
{
	HTREEITEM temp;
	TCHAR buffer[64], szTypeName[16];
	//ID:0 ���� ����
	wsprintf(buffer, TEXT("ID:%d "), id);
	Element->GetElementTypeName(szTypeName);
	wsprintf(buffer, TEXT("%s %s %s"), buffer, szTypeName, Element->Name);

	TItem tempItem;
	switch (Element->eType)
	{
	case ELEMENT_REALLINE:
	case ELEMENT_FRAMEPOINT:
	case ELEMENT_BAR:
		temp = InsertTreeviewItem(buffer, hPrevObject);
		TreeView_Expand(m_hWnd, hPrevObject, TVE_EXPAND);
		tempItem.ObjectId = id;
		tempItem.hTreeItem = temp;
		Item.push_back(tempItem);
		break;
	case CONSTRAINT_COINCIDE:
		temp = InsertTreeviewItem(buffer, hPrevConstraint);
		TreeView_Expand(m_hWnd, hPrevConstraint, TVE_EXPAND);
		tempItem.ObjectId = id;
		tempItem.hTreeItem = temp;
		Item.push_back(tempItem);
		break;
	}
}

void TTreeViewContent::Initial()
{
	hPrevObject = InsertTreeviewItem(TEXT("Ԫ��"), TVI_ROOT);
	hPrevConstraint = InsertTreeviewItem(TEXT("Լ��"), TVI_ROOT);
}

void TTreeViewContent::SelectNull()
{
	TreeView_SelectItem(m_hWnd, NULL);
}

void TTreeViewContent::SelectById(int id)
{
	for (int i = 0; i < Item.size(); i++)
	{
		if (Item[i].ObjectId == id)
		{
			//SetFocus(m_hWnd);
			TreeView_Select(m_hWnd, Item[i].hTreeItem, TVGN_CARET);
		}
	}
}

HTREEITEM TTreeViewContent::GetHTreeItemFromId(int id)
{
	for (int i = 0; i < Item.size(); i++)
	{
		if (Item[i].ObjectId==id)
		{
			return Item[i].hTreeItem;
		}
	}
	return NULL;
}

int TTreeViewContent::GetIdFromHTreeView(HTREEITEM hTreeItem)
{
	for (int i = 0; i < Item.size(); i++)
	{
		if (Item[i].hTreeItem == hTreeItem)
		{
			return Item[i].ObjectId;
		}
	}
	return -1;
}