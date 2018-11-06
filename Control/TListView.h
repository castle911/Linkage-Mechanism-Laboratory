#pragma once
#include <vector>
#include <Windows.h>

#include <CommCtrl.h>
#include "..\Common\tchar_head.h"
#include <stdio.h>
#pragma comment(lib, "comctl32.lib")

#include "TControl.h"
#include "TListViewEdit.h"
#include "..\Common\DPOINT.h"

#include "..\enumCtrlType.h"

class TTreeViewContent;
class TShape;
class TListView:public TControl
{
private:
	RECT rc;
	POINT origin;
	int iRowCount, iColumnCount;
	TListViewEdit tempEdit;
	void TListView::DeleteAllEdit();
	std::vector<enumCtrlType> vecCtrlType;
	std::vector<void *> vecpContent;
	TShape *pShape;
	TTreeViewContent *pTreeViewContent;
protected:
	//������Ϣ��������������
	LRESULT TListView::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
public:
	TListView();
	~TListView();
	int id;
	void TListView::CreateListViewEx(HWND hParent, UINT id, HINSTANCE hInst);//����ListView
	void TListView::AddColumn(TCHAR text[], int width, int styleLVCFMT = LVCFMT_LEFT);//����б�ǩ
	void TListView::InsertColumn(int index, TCHAR text[], int width, int styleLVCFMT = LVCFMT_LEFT);//�����б�ǩ
	void CDECL TListView::InsertItem(int index, int subitem,const TCHAR szFormat[], ...);//������Ŀ
	void CDECL TListView::AddAttributeItem(const TCHAR szName[],enumCtrlType eCtrlType,void *pContent,const TCHAR szEditFormat[], ...);
	void CDECL TListView::SetAttributeItemValue(int index, TCHAR szFormat[], ...);//δ��
	int TListView::GetItemCount();//�õ���Ŀ��
	void TListView::DeleteAllItems();//���ȫ����
	RECT TListView::GetGridRect(int index, int subitem);//�����кź��кŵõ���������
	RECT TListView::GetGridRectInMargin(int index, int subitem);//�õ����ӱ߿��ڵ�����
	//void TListView::LoadObject(int index);
};

