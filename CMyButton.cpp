#include "CMyButton.h"


//MyButton.cpp
CMybutton::CMybutton()
{
	m_IsInRect = FALSE;
	m_IsBmpLoad = FALSE;
	m_BmpHeight = 0;
	m_BmpWidth = 0;

}

CMybutton::~CMybutton()
{
}

void CMybutton::DrawMyButton(HDC hdc, char state)
{
	RECT buttonRect;
	GetClientRect(m_hWnd, &buttonRect);
	SetStretchBltMode(hdc, HALFTONE);
	LONG width = buttonRect.right - buttonRect.left;
	LONG height = buttonRect.bottom - buttonRect.top;
	if (m_IsBmpLoad == TRUE)
	{
		switch (state)
		{
		case 1:
			StretchBlt(hdc, 0, 0, width, height, m_memBmp, 0, 0, m_BmpWidth
				, m_BmpHeight, SRCCOPY);
			break;
		case 2:
			StretchBlt(hdc, 0, 0, width, height, m_memBmp, m_BmpWidth, 0, m_BmpWidth
				, m_BmpHeight, SRCCOPY);
			break;
		case 3:
			StretchBlt(hdc, 0, 0, width, height, m_memBmp, m_BmpWidth * 2, 0, m_BmpWidth
				, m_BmpHeight, SRCCOPY);
			break;
		case 4:
			StretchBlt(hdc, 0, 0, width, height, m_memBmp, m_BmpWidth * 3, 0, m_BmpWidth
				, m_BmpHeight, SRCCOPY);
			break;
		default:
			break;
		}
	}
}

void CMybutton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your code to draw the specified item
	HDC dc;
	String buttonText;
	RECT buttonRect;
	UINT buttonState;
	if (m_IsBmpLoad == TRUE)
	{
		buttonState = lpDrawItemStruct->itemState;
		GetClientRect(m_hWnd, &buttonRect);

		TCHAR *sz = new TCHAR[GetWindowTextLength(m_hWnd) + 1];
		GetWindowText(m_hWnd, sz, GetWindowTextLength(m_hWnd));
		buttonText = sz;
		delete[] sz;

		if (buttonState & ODS_DISABLED)
		{
			DrawMyButton(m_hDC, 4);
			SetTextColor(m_hDC, RGB(80, 80, 80));
		}
		else if (buttonState & ODS_SELECTED)
		{
			DrawMyButton(m_hDC, 3);
		}
		else if (m_IsInRect == TRUE)
		{
			DrawMyButton(m_hDC, 2);
		}
		else 	//Ĭ�������
		{
			DrawMyButton(m_hDC, 1);
			SetTextColor(m_hDC, RGB(0, 0, 0));
		}
		if (buttonState & ODS_FOCUS)
		{
			RECT FocTect(buttonRect);	//���콹������
			TDraw::SetMarginRect(&FocTect, 2);
			//FocTect.DeflateRect(2, 2, 2, 2);	//���ý��������С
			DrawFocusRect(m_hDC, &FocTect);	//���ƽ����
			lpDrawItemStruct->itemAction = ODA_FOCUS;
		}
		SetBkMode(m_hDC, TRANSPARENT);	//���ñ���͸��
		if (buttonState & ODS_SELECTED)
		{
			RECT rect;
			rect.left = buttonRect.left;
			rect.top = buttonRect.top + 2;
			rect.right = buttonRect.right + 2;
			rect.bottom = buttonRect.bottom;
			DrawText(m_hDC, buttonText.c_str(), buttonText.length(), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);	//���ư�ť�ı�
		}
		else
			DrawText(m_hDC, buttonText.c_str(), buttonText.length(), &buttonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);	//���ư�ť�ı�

	}
}


void CMybutton::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	POINT point;	//����POINT����
	GetCursorPos(&point); 	//������λ��
	RECT rcWnd;	//�����������
	GetWindowRect(m_hWnd, &rcWnd); 	//��ð�ť����
	if (PtInRect(&rcWnd, point)) 	//�ж�����Ƿ��ڰ�ť��
	{


	}
	else	//����ƶ�����ť��
	{
		m_IsInRect = FALSE;	//����m_IsInRect����ֵ
		Invalidate(); 	  //�ػ水ť
		KillTimer(m_hWnd, 1);
	}
}

void CMybutton::OnMouseMove(UINT nFlags, POINT point)
{
	// TODO: Add your message handler code here and/or call default
	RECT rcWnd;	//�����������
	GetClientRect(m_hWnd, &rcWnd);
	if (PtInRect(&rcWnd, point))
	{
		if (m_IsInRect == FALSE)
		{
			m_IsInRect = TRUE;
			Invalidate();
			SetTimer(m_hWnd, 1, 50, NULL);
		}

	}
	//	CButton::OnMouseMove(nFlags, point);
}

void CMybutton::LoadBtBmp(UINT sID, int heigh, int width)
{
	m_hDC = GetWindowDC(m_hWnd);
	LoadBitmap(m_hInst, MAKEINTRESOURCE(sID));
	m_memBmp = CreateCompatibleDC(m_hDC);
	SelectObject(m_memBmp, m_BtBmp);
	m_BmpHeight = heigh;
	m_BmpWidth = width;
	m_IsBmpLoad = TRUE;
}


LRESULT CMybutton::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_TIMER:
		OnTimer(lParam);
		return 0;
	case WM_MOUSEMOVE:
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		OnMouseMove(wParam, pt);
		return 0;
	case WM_DRAWITEM:
		DRAWITEMSTRUCT *DrawItemStruct = (DRAWITEMSTRUCT *)lParam;
		DrawItem(DrawItemStruct);
		return 0;
	}
	return CallWindowProc(wndproc, hWnd, uMsg, wParam, lParam);
}