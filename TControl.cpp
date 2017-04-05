#pragma once
#include "DetectMemoryLeak.h"

#include "TControl.h"


TControl::TControl()
{
	m_hWnd = NULL;
	m_hParent = NULL;
	m_hInst = NULL;
}


TControl::~TControl()
{
}

void TControl::SetPos(RECT &rect)//ֻʹ��rect��left��top�����û������m_iWidth��m_iHeight������
{
	::MoveWindow(m_hWnd, rect.left, rect.top, m_iWidth,m_iHeight, true);
}

void TControl::SetRect(RECT &rect)
{
	::MoveWindow(m_hWnd, rect.left,rect.top,  rect.right, rect.bottom, true);
}

void TControl::SetRect(int x1, int y1, int x2, int y2)
{
	::MoveWindow(m_hWnd, x1, y1, x2 - x1, y2 - y1, true);
}

LRESULT CALLBACK TControl::subControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TControl * pControl;
	pControl = (TControl *)GetWindowLong(hWnd, GWL_USERDATA);

	WNDPROC oldProc;
	oldProc = (WNDPROC)GetProp(hWnd, TEXT("oldProc"));
	if (pControl)
		return pControl->WndProc((WNDPROC)oldProc, hWnd, uMsg, wParam, lParam);
	else
		return CallWindowProc((WNDPROC)oldProc, hWnd, uMsg, wParam, lParam);
}

void TControl::LinkControl(HWND hwndControl)//���ӵ����пؼ������ڶԻ����У�
{
	m_hWnd = hwndControl;
	RegisterProc();
}

void TControl::RegisterProc()//�������ں�ע��
{
	SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

	WNDPROC oldProc= (WNDPROC)::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)subControlProc);

	SetProp(m_hWnd, TEXT("oldProc"), oldProc);
}

LRESULT TControl::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)//������Ϣ��������������
{
	return CallWindowProc(wndproc, hWnd, uMsg, wParam, lParam);
}