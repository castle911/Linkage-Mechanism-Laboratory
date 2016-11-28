#pragma once
#include <Windows.h>

//Ϊ�����ظ�ˢ�£������༰�������಻�÷���WM_PAINT��Ϣ��ˢ�¶�����Canvas��ɡ�
//class TMainWindow;
class TCanvas;
class TShape;
class TConfiguration;
class TListView;
class TTreeViewContent;
class TTool
{
private:
protected:
	//TMainWindow *pwin;
	TShape *pShape;
	TCanvas *pCanvas;
	HWND hwndWin;
	TConfiguration *pConfig;
	TListView *pListView;
	TTreeViewContent *pTreeViewContent;
	//void TTool::RefreshTreeViewContent();
public:
	TTool();
	virtual ~TTool();
	virtual void OnKeyDown(HWND hWnd, WPARAM wParam,LPARAM lParam)   { }
	virtual void OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)   { }
	virtual void OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)   { }
	virtual void OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)   { }
	virtual void OnMouseWheel(HWND hWnd, UINT nFlags, POINT ptPos)   { }
	virtual void OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)   { }
	virtual void Draw(HDC hdc){}
};

