#pragma once
#include <Windows.h>
#include "String.h"

//Ϊ�����ظ�ˢ�£������༰�������಻�÷���WM_PAINT��Ϣ��ˢ�¶�����Canvas��ɡ�
//class TMainWindow;
class TCanvas;
class TShape;
class TConfiguration;
class TListView;
class TTreeViewContent;
class TSolver;
class TElement;
class TTool
{
private:
protected:
	//TMainWindow *pwin;
	HINSTANCE hInst;
	TShape *pShape;
	TCanvas *pCanvas;
	HWND hwndWin;
	TConfiguration *pConfig;
	TListView *pListView;
	TTreeViewContent *pTreeViewContent;
	TSolver *pSolver;
	//void TTool::RefreshTreeViewContent();
	POINT ptMouse;
	bool bShowTips;
	String sTips;
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
	void TTool::ResetTool();
	void TTool::RefreshCanvas();
	void TTool::RefreshEquations();
	void TTool::AddTreeViewItem(TElement *pElement, int iId);
};

