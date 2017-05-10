#pragma once
#include "DetectMemoryLeak.h"

#include "TControl.h"


TControl::TControl()
{
	m_hWnd = NULL;
	m_hParent = NULL;
	m_hInst = NULL;

	Text = NULL;
}


TControl::~TControl()
{
	if (Text != NULL)
		free(Text);
}

//��ʹ��x,y���꣬width,heightʹ��ԭ��С
void TControl::SetPositionOnlyOrigin(const RECT &rect)
{
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	SetPosition(rect.left, rect.top, rc.right, rc.bottom);
}

//right��bottom������ǿ�͸�
void TControl::SetRect(RECT &rect)
{
	::MoveWindow(m_hWnd, rect.left,rect.top,  rect.right, rect.bottom, true);
}

//�Խǵ�����
void TControl::SetRect(int x1, int y1, int x2, int y2)
{
	::MoveWindow(m_hWnd, x1, y1, x2 - x1, y2 - y1, true);
}


void TControl::SetPosition(int x, int y, int width, int height)
{
	::MoveWindow(m_hWnd, x, y, width, height, true);
	//::SetWindowPos(m_hWnd, HWND_TOP, x, y, width, height, 0);//SWP_SHOWWINDOW
}

//rect�и�ֵ��Ϊ����
void TControl::SetPosition(RECT rect)
{
	::MoveWindow(m_hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, true);
	//::SetWindowPos(m_hWnd, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 0);//SWP_SHOWWINDOW
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

void TControl::Invalidate()
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	InvalidateRect(m_hWnd, &rect, FALSE);
}

void TControl::SetFont(HFONT hFont)
{
	SendMessage(m_hWnd,             // Handle of edit control
		WM_SETFONT,         // Message to change the font
		(WPARAM)hFont,     // handle of the font
		MAKELPARAM(TRUE, 0) // Redraw text
		);
}

void CDECL TControl::SetText(TCHAR szFormat[], ...)
{
	TCHAR szBuffer[1024];
	va_list pArgList;
	va_start(pArgList, szFormat);
	_vsntprintf_s(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);
	va_end(pArgList);

	::SetWindowText(m_hWnd, szBuffer);
}

void TControl::GetText(TCHAR text[])
{
	::GetWindowText(m_hWnd, text, GetLength() + 1);//��֪��ΪʲôҪ��1��ȡ��ȫ
}

TCHAR * TControl::GetText()
{
	//if (Text != NULL)
	//	free(Text);
	Text = (TCHAR *)realloc(Text, (GetLength() + 1)*sizeof(TCHAR));
	GetText(Text);
	return Text;
}

int TControl::GetLength()
{
	return ::GetWindowTextLength(m_hWnd);
}

void TControl::SetVisible(bool bShow)
{
	::ShowWindow(m_hWnd, bShow ? SW_SHOWNORMAL : SW_HIDE);
}

bool TControl::GetVisible()
{
	return (bool)IsWindowVisible(m_hWnd);
}

void TControl::SetEnable(bool bEnable)
{
	EnableWindow(m_hWnd, bEnable);
}

bool TControl::GetEnable()
{
	return (bool)IsWindowEnabled(m_hWnd);
}

void TControl::SetDouble(double d)
{
	int n = 6;//һ��ʼ�ͼٶ�С��λ��Ϊ6λ
	long temp = (long)(d * 1e6);//��6λС��ȫ��ȡ������������������Ŀ���������С������
	for (n = 6; n>0; n--)
	{
		if (0 != temp % 10) break;
		temp = temp / 10;
	}
	SetText(TEXT("%.*f"),n, d);
}

double TControl::GetDouble()
{
	return _tcstod(GetText(), NULL);
}
