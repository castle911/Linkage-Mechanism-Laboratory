#pragma once
#include "TTool.h"

class TRealLine;
class TConfiguration;
class TShape;
class TCanvas;
class TSelectTool:public TTool
{
private:
	UINT uiLastLineStyle;
	int iPickRealLineIndex;
	TShape *Shape;
	TConfiguration *Config;
	TCanvas *Canvas;
	void TSelectTool::Draw(HDC hdc);
	void OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	bool TSelectTool::PickRealLine(POINT ptPos, TRealLine &RealLine);
public:
	TSelectTool();
	~TSelectTool();
};

