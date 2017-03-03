#pragma once
#include <vector>

#include "TTreeView.h"

class TElement;
class TShape;
class TTreeViewContent :
	public TTreeView
{
private:
	HTREEITEM hPrevObject;
	HTREEITEM hPrevConstraint;
	TShape *pShape;
public:
	struct TItem
	{
		int ObjectId;
		HTREEITEM hTreeItem;
	};
	std::vector<TItem> Item;
	TTreeViewContent();
	~TTreeViewContent();
	void TTreeViewContent::Initial();
	int TTreeViewContent::GetIdFromHTreeView(HTREEITEM hTreeItem);
	HTREEITEM TTreeViewContent::GetHTreeItemFromId(int id);
	void TTreeViewContent::DeleteAllItems();
	void TTreeViewContent::SelectById(int id);//������RightWindow OnNotify
	void TTreeViewContent::AddItem(TElement *Element,int id);
	void TTreeViewContent::DeleteById(int id);
	void TTreeViewContent::SelectNull();
	void TTreeViewContent::RefreshAvailableItem();
	void TTreeViewContent::AddAllItem();//�����ڶ����ļ�ʱʹ��
	LRESULT TTreeViewContent::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

