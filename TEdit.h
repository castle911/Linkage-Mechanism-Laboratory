#pragma once
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

class TEdit
{
protected:
	HWND hParent;
	bool bVisible;
	static WNDPROC oldEditProc;
	static LRESULT CALLBACK subEditProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam); 

	//������Ϣ��������������
	virtual LRESULT TEdit::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	//����VK_RETURN,VK_ESCAPE������ֵ�����Ƿ����Ĭ����Ϣ����
	virtual bool TEdit::OnKeyDown(WPARAM wParam, LPARAM lParam){ return true; };

	//�����ַ�����������ֵ�����Ƿ����Ĭ����Ϣ����
	virtual bool TEdit::OnChar(WPARAM wParam, LPARAM lParam){ return true; };
public:
	bool bMultiLine,bAutoHScrol,bAutoVScrol,bNoHideSel;
	HFONT m_hFont;
	HWND m_hWnd;
	HINSTANCE m_hInst;
	TEdit();
	~TEdit();
	void TEdit::CreateEditEx(HWND hParent, UINT id, HINSTANCE hInst, DWORD dwStyle = WS_EX_CLIENTEDGE);//����Edit
	void TEdit::SetPos(int x, int y, int width, int height);//���ô�С��λ��
	void TEdit::SetPos(RECT rect);//���ô�С��λ��
	void TEdit::SetVisible(bool bShow);//���ÿɼ���
	void TEdit::SetDefaultGuiFont();
	void CDECL TEdit::SetText(TCHAR szFormat[], ...);//��������
	void TEdit::GetText(TCHAR text[]);
	int TEdit::GetLength();//��ȡ�ַ�������
	void TEdit::SetSelect(int iStart, int iEnd);//ѡ��
	void TEdit::SetFont(HFONT hFont);
	void TEdit::SetSelectAll();//ȫѡ
	bool TEdit::GetVisible();
};

