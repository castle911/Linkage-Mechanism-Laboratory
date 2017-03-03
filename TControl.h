#pragma once
#include <Windows.h>

class TControl
{
public:
	HWND m_hWnd;
	HINSTANCE m_hInst;
	TControl();
	~TControl();
	void TControl::SetRect(RECT &rect);
	void TControl::SetRect(int x1, int y1, int x2, int y2);
	void TControl::RegisterProc();//�������ں�ע��
protected:
	static WNDPROC oldControlProc;
	static LRESULT CALLBACK subControlProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//������Ϣ��������������
	virtual LRESULT TControl::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

