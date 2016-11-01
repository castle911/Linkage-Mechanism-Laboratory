#pragma once
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

class TEdit
{
protected:
	bool bVisible;
	static WNDPROC oldEditProc;
	static LRESULT CALLBACK subEditProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam); 
	void TEdit::CreateEx(DWORD dwExStyle, LPCTSTR lpszClass, LPCTSTR lpszName, DWORD dwStyle,int x, int y, int nWidth, int nHeight, HWND hParent,HMENU hMenu, HINSTANCE hInst);

	virtual LRESULT TEdit::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	//����VK_RETURN,VK_ESCAPE������ֵ�����Ƿ����Ĭ����Ϣ����
	virtual bool TEdit::OnKeyDown(WPARAM wParam, LPARAM lParam){ return true; };

	//�����ַ�����������ֵ�����Ƿ����Ĭ����Ϣ����
	virtual bool TEdit::OnChar(WPARAM wParam, LPARAM lParam){ return true; };
public:
	HWND m_hWnd;
	HINSTANCE m_hInst;
	TEdit();
	~TEdit();
	void TEdit::CreateEditEx(HWND hParent, UINT id, HINSTANCE hInst);//����Edit
	void TEdit::SetPos(int x, int y, int width, int height);//���ô�С��λ��
	void TEdit::SetVisible(bool bShow);//���ÿɼ���
	void CDECL TEdit::SetText(TCHAR szFormat[], ...);//��������
	void TEdit::GetText(TCHAR text[]);
	int TEdit::GetLength();//��ȡ�ַ�������
	void TEdit::SetSelect(int iStart, int iEnd);//ѡ��
	void TEdit::SetSelectAll();//ȫѡ
	bool TEdit::GetVisible();
};

