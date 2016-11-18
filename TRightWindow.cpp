#pragma once
#include "TRightWindow.h"
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#pragma comment(lib, "comctl32.lib")

#include "resource.h"
#include "TEdit.h"
#include "TDraw.h"
#include "TSelectTool.h"

#include "TMainWindow.h"

extern TMainWindow win;

TRightWindow::TRightWindow()
{
}


TRightWindow::~TRightWindow()
{
}

void TRightWindow::OnDraw(HDC hdc)
{
	TDraw::FillRect(hdc, &ClientRect, RGB(240, 240, 240));
}

void TRightWindow::OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//ShowMessage(TEXT("%d"), ListView_GetItemCount(ListView.m_hWnd));
}

void TRightWindow::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//��ʱm_hWnd��δ���£�����ʹ�á�ֻ����hWnd��
	TreeViewContent.CreateTreeViewEx(hWnd, IDC_TREEVIEW, m_hInst);
	TreeViewContent.Initial();
	SetTreeViewPos();

	ListView.CreateListViewEx(hWnd, IDC_LISTVIEW, m_hInst);
	SetListViewPos();
	ListView.AddColumn(TEXT("��Ŀ"), 60);
	ListView.AddColumn(TEXT("����"), 113);


}

void TRightWindow::OnNotify(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId = LOWORD(wParam);
	int wmEvent = HIWORD(wParam);
	if (wmId == IDC_TREEVIEW)
	{
		switch (((LPNMHDR)lParam)->code)
		{
		case TVN_SELCHANGED:
			LPNMTREEVIEW pnmtv = (LPNMTREEVIEW)lParam;
			//������ǰ���ߣ�ʹ��ѡ�񹤾�
			if (win.m_ManageTool.m_uiCurActiveTool!=ID_SELECT)
				win.m_ManageTool.SetCurActiveTool(ID_SELECT);

			//�õ���ѡ���id
			int id = TreeViewContent.GetIdFromHTreeView(pnmtv->itemNew.hItem);

			//����ѡ�񹤾�
			((TSelectTool *)win.m_ManageTool.m_pCurrentTool)->SelectById(id);

			//ˢ����ʾ
			::InvalidateRect(win.Canvas.m_hWnd, &(win.Canvas.ClientRect), FALSE);
			break;
		}
	}
}

void TRightWindow::SetListViewPos()
{
	::MoveWindow(ListView.m_hWnd,
		ClientRect.left + 5,
		ClientRect.top + ClientRect.bottom / 2,
		ClientRect.right - 10,
		ClientRect.bottom - 10 - ClientRect.bottom / 2, true);
}

void TRightWindow::SetTreeViewPos()
{
	::MoveWindow(TreeViewContent.m_hWnd,
		ClientRect.left + 5,
		ClientRect.top + 5 ,
		ClientRect.right - 10,
		ClientRect.bottom - 10 - ClientRect.bottom / 2, true);
}

void TRightWindow::OnSize(WPARAM wParam, LPARAM lParam)
{
	SetTreeViewPos();
	SetListViewPos();
}