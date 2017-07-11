#pragma once
#include "TWindow.h"
#include "resource.h"
#include "TRightWindow.h"
#include "TConfiguration.h"
#include "TToolbar.h"
#include "TManageTool.h"
#include "TCanvas.h"
#include "TShape.h"
#include "TStatus.h"
#include "TTrackbar.h"
#include "TEdit.h"
#include "TSolver.h"
#include "TCheckBox.h"

class TGraph;
class TConsole;
class TMainWindow: public TWindow
{
private:
	std::vector<TGraph *> vecpGraph;
	TCHAR szFileName[MAX_PATH];
	int m_iRightWindowWidth;
	void TMainWindow::SetCanvasPos();
	void TMainWindow::SetRightWindowPos();
public:
	TCanvas Canvas;
	TRightWindow RightWindow;
	TManageTool m_ManageTool;
	TShape m_Shape;
	TConfiguration m_Configuration;
	TToolbar m_Toolbar;
	TStatus m_Status;
	TTrackbar m_Trackbar;
	TCheckBox m_CheckBoxShowReal;
	TCheckBox m_CheckBoxTheme;
	TCheckBox m_CheckBoxAxes;

	//TSolver m_Solver;
	TSolver *pSolver;
	TConsole *pConsole;
	TMainWindow();
	~TMainWindow();

	void OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnSize(WPARAM wParam, LPARAM lParam);
	void OnNotify(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};