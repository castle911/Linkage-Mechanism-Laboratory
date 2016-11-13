#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#include <tchar.h>
#pragma comment(lib,"comctl32.lib")//��ʽʹ��

class TToolbar
{
private:
	HINSTANCE m_hInst;
	int iButtonNum;
	TBBUTTON *tbButtons; 
	void TToolbar::CreateImageList(UINT uMsg, int cx, int cy, UINT BitmapID, COLORREF crMask);
	void TToolbar::AddElement(int BitmapIndex, int idCommand, BYTE fsState, BYTE fsStyle, BYTE bReverse[], DWORD_PTR dwData, INT_PTR iString); 
public:
	bool bIsFlat;//������ʽ
	bool bTextOnRight;//�ı��������ұ�
	bool bAutoWrap;//��ť�Զ�����
	HWND m_hWnd;//���������
	TToolbar();
	~TToolbar();
	void TToolbar::CreateToolbar(HWND hwndParent, HINSTANCE hInst);
	void TToolbar::LoadImageList(int cx, int cy, UINT BitmapID, COLORREF crMask);
	void TToolbar::LoadHotImageList(int cx, int cy, UINT BitmapID, COLORREF crMask);
	void TToolbar::LoadDisabledImageList(int cx, int cy, UINT BitmapID, COLORREF crMask);
	void TToolbar::AddButton(int IconIndex, int idCommand, bool Enable, TCHAR iString[]); 
	void TToolbar::AddGroup(int IconIndex, int idCommand, bool Enable, TCHAR iString[]);
	void TToolbar::AddCheck(int IconIndex, int idCommand, bool Enable, TCHAR iString[]);
	void TToolbar::AddSeparator(int iWidth);
	void TToolbar::ShowToolbar();
	void TToolbar::FreshSize();
	RECT TToolbar::GetClientRect();
};

