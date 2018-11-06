#pragma once
#include <Windows.h>
#include "..\Common\String.h"

class TControl
{
private:
	HFONT m_hFont;
	static LRESULT CALLBACK TControl::subControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	//LONG m_iWidth, m_iHeight;
	HWND m_hParent;
	HWND m_hWnd;
	HINSTANCE m_hInst;
	TControl();
	~TControl();
	void TControl::LinkControl(HWND hwnd);
	void TControl::LinkControl(HWND hDlg, int id);
	RECT TControl::GetClientRect();
	void TControl::SetRect(RECT &rect);
	void TControl::SetRect(int x1, int y1, int x2, int y2); 
	//void TControl::SetPos(RECT &rect);
	void TControl::Invalidate();
	void TControl::SetFont(HFONT hFont);
	void TControl::SetDefaultGuiFont();
	void TControl::SetFont(TCHAR FontName[], int FontSize);
	void TControl::SetText(const String &s);
	void CDECL TControl::SetText(const TCHAR szFormat[], ...);//��������
	void TControl::GetText(TCHAR text[]);
	TCHAR * TControl::GetText();
	int TControl::GetLength();//��ȡ�ַ�������	
	void TControl::SetPosition(int x, int y, int width, int height);//���ô�С��λ��
	void TControl::SetPosition(RECT rect);//���ô�С��λ��
	void TControl::SetPositionOnlyOrigin(const RECT &rect);
	void TControl::SetVisible(bool bShow);//���ÿɼ���
	bool TControl::GetVisible();
	void TControl::SetDouble(double d);
	double TControl::GetDouble();
	void TControl::SetEnable(bool bEnable);
	bool TControl::GetEnable();
protected:
	TCHAR *Text;
	void TControl::RegisterProc();//�������ں�ע��

	virtual LRESULT TControl::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//������Ϣ���������ɸ���

};
