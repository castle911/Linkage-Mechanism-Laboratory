#pragma once
#include "resource.h"
#include "TTool.h"
#include "TLine.h"

#include "TConfiguration.h"
#include "TMainWindow.h"
#include "TDraw.h"
#include "TLineEdit.h"
#include "TAttach.h"

class TLineTool :public TTool
{
private:
	TAttach *Attach;
	TConfiguration *Config;
	UINT m_uiHit;//点击数
	DPOINT *m_pptHit;//每次的点击位置
	TRealLine *MoveLine;//显示点的位置
	bool bShowDimLine;
	POINT pt1, pt2;
	TLine *Line1, *Line2, *LineDim;

public:
	TLineEdit *LineEdit;
	TLineTool();
	~TLineTool();
	void OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnMouseWheel(HWND hWnd, UINT nFlags, POINT ptPos);
	void TLineTool::InitialLine(DPOINT dptPos);
	void Draw(HDC hdc);
};

