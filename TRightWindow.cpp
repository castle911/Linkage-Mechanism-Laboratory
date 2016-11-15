#pragma once
#include "TRightWindow.h"
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#pragma comment(lib, "comctl32.lib")

#include "resource.h"
#include "TEdit.h"
#include "TDraw.h"

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
	ShowMessage(TEXT("%d"), ListView_GetItemCount(ListView.m_hWnd));
}

void TRightWindow::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//��ʱm_hWnd��δ���£�����ʹ�á�ֻ����hWnd��
	TreeView.CreateTreeViewEx(hWnd, IDC_TREEVIEW, m_hInst);
	SetTreeViewPos();

	ListView.CreateListViewEx(hWnd, IDC_LISTVIEW, m_hInst);
	SetListViewPos();
	ListView.AddColumn(TEXT("��Ŀ"), 80);
	ListView.AddColumn(TEXT("����"), 93);

	//ListView.InsertAttributeItem(0, TEXT("����"), 200, TEXT("%d"), 1);
	//ListView.InsertAttributeItem(1, TEXT("����"), 200, TEXT("����"), 0);
	//ListView.InsertAttributeItem(2, TEXT("����"), 200, TEXT("ʵ��"), 0);
	//ListView.InsertAttributeItem(3, TEXT("�߿�"), 200, TEXT("1"), 0);
	//ListView.InsertAttributeItem(4, TEXT("��ɫ"), 200, TEXT("��ɫ"), 0);
	//ListView.InsertAttributeItem(5, TEXT("X1"), 200, TEXT("%f"), 0.23);
	//ListView.InsertAttributeItem(6, TEXT("Y1"), 200, TEXT("%f"), 0.11);
	//ListView.InsertAttributeItem(7, TEXT("X2"), 200, TEXT("%f"), 0.34);
	//ListView.InsertAttributeItem(8, TEXT("Y2"), 200, TEXT("%f"), 0.13);
	//ListView.InsertAttributeItem(9, TEXT("����"), 200, TEXT("%d"), 100);
	//for (int i = 10; i < 30;i++)
	//	ListView.InsertAttributeItem(i, TEXT("X"), 200, TEXT("%d"), i);

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
	::MoveWindow(TreeView.m_hWnd,
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