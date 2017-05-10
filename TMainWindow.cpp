#pragma once
#include "DetectMemoryLeak.h"

#include <process.h>
#include "TMainWindow.h"

#include "TToolTip.h"

#include "String.h"

#include "TConstraintCoincide.h" 
#include "TConstraintColinear.h"
#include "TSlider.h"
#include "TSolver.h"
#include "TConsole.h"
#include "TSelectTool.h"

#include "DialogAddDriver.h"
#include "DialogOption.h"
#include "DialogAbout.h"
#include "DialogAnimation.h"

#include "TGraph.h"

#include "FileFunction.h"

TMainWindow::TMainWindow()
{
	m_iRightWindowWidth = 200;
	pConsole = NULL;
	pSolver = new TSolver;
}


TMainWindow::~TMainWindow()
{
	if (pConsole != NULL)
	{
		delete pConsole;
	}

	if (pSolver != NULL)
		delete pSolver;

	for (auto iter = vecpGraph.begin(); iter != vecpGraph.end(); ++iter)
	{
		delete *iter;
	}
	//_CrtDumpMemoryLeaks();
}


void TMainWindow::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//��ʱm_hWnd��δ���£�����ʹ�á�ֻ����hWnd��

	//���ó�ʼ��
	this->m_Configuration.Initial(hWnd);

	//����������
	m_Toolbar.bAutoWrap = true;
	m_Toolbar.bIsFlat = true;
	//m_Toolbar.bTextOnRight = true;
	m_Toolbar.CreateToolbar(hWnd, m_hInst);
	m_Toolbar.LoadImageList(32, 32, IDR_TOOLBAR_MAIN, RGB(255, 255, 255));
	m_Toolbar.AddGroup(0, 0, ID_SELECT, true, TEXT("ѡ��"));
	m_Toolbar.AddGroup(1, 0, ID_DRAG, true, TEXT("�϶�"));
	m_Toolbar.AddButton(2, ID_REFRESH, true, TEXT("ˢ��"));
	m_Toolbar.AddSeparator(0);
	m_Toolbar.AddGroup(3, 0, ID_DRAW_FRAME, true, TEXT("����"));
	m_Toolbar.AddGroup(4, 0, ID_DRAW_BAR, true, TEXT("����"));
	m_Toolbar.AddGroup(5, 0, ID_DRAW_POLYLINE_BAR, true, TEXT("��θ�"));
	m_Toolbar.AddGroup(6, 0, ID_DRAW_LINE, true, TEXT("��"));
	m_Toolbar.AddGroup(7, 0, ID_DRAW_SLIDEWAY, true, TEXT("����"));
	m_Toolbar.AddGroup(8, 0, ID_DRAW_SLIDER, true, TEXT("����"));
	m_Toolbar.AddSeparator(0);
	m_Toolbar.AddGroup(9, 0, ID_DRAW_COINCIDE, true, TEXT("�غ�Լ��"));
	m_Toolbar.AddGroup(10, 0, ID_DRAW_COLINEAR, true, TEXT("����Լ��"));
	m_Toolbar.AddSeparator(0);
	m_Toolbar.AddButton(11, ID_SET_DRIVER, true, TEXT("��Ϊԭ����"));
	m_Toolbar.AddButton(12, ID_ANIMATION, true, TEXT("���������"));
	m_Toolbar.ShowToolbar();

	//����״̬��
	m_Status.Create(hWnd, IDR_STATUS, m_hInst, 24);
	m_Status.AddPart(IDR_STATUS_COORDINATE, 160, PT_FIXED);//����
	m_Status.AddPart(0, 0, PT_NONE);
	m_Status.AddPart(IDR_STATUS_CHECKBOX_SHOW_REAL, 22, PT_FIXED);//���˻���/��ʵ����
	m_Status.AddPart(IDR_STATUS_CHECKBOX_THEME, 22, PT_FIXED);//
	m_Status.AddPart(IDR_STATUS_PROPORTIONNAME, 40, PT_FIXED);//������
	m_Status.AddPart(IDR_STATUS_TRACKBAR, 120, PT_FIXED);//
	m_Status.AddPart(IDR_STATUS_PROPORTION, 60, PT_FIXED);//��������
	m_Status.AddPart(0, 6, PT_FIXED);//�����϶����ǵ�
	m_Status.FreshSize();

	m_Status.SetText(IDR_STATUS_PROPORTIONNAME, TEXT("������"), m_Configuration.GetProportion() * 100);
	m_Status.SetText(IDR_STATUS_PROPORTION, TEXT("%.0f%%"), m_Configuration.GetProportion() * 100);

	//������ʵ�л���ť
	m_CheckBoxShowReal.LoadCheckedBitmap(m_hInst, IDR_BITMAP_SHOW_REAL);
	m_CheckBoxShowReal.LoadUnCheckedBitmap(m_hInst, IDR_BITMAP_SHOW_SIMPLE);
	m_CheckBoxShowReal.CreateBitmapCheckBox(m_hInst, m_Status.m_hWnd, m_Status.GetPartRect(IDR_STATUS_CHECKBOX_SHOW_REAL, 1).left, m_Status.GetPartRect(IDR_STATUS_CHECKBOX_SHOW_REAL, 0).top,20,20, IDR_STATUS_CHECKBOX_SHOW_REAL);
	m_CheckBoxShowReal.SetCheckedAndBitmap(m_Configuration.bDrawReal);

	//��������л���ť
	m_CheckBoxTheme.LoadCheckedBitmap(m_hInst, IDB_BITMAP_THEME_DARK);
	m_CheckBoxTheme.LoadUnCheckedBitmap(m_hInst, IDB_BITMAP_THEME_BRIGHT);
	m_CheckBoxTheme.CreateBitmapCheckBox(m_hInst, m_Status.m_hWnd, m_Status.GetPartRect(IDR_STATUS_CHECKBOX_THEME, 1).left, m_Status.GetPartRect(IDR_STATUS_CHECKBOX_THEME, 0).top,20,20, IDR_STATUS_CHECKBOX_THEME);
	m_CheckBoxTheme.SetCheckedAndBitmap(m_Configuration.bThemeDark);

	//����tooltip
	CreateToolTip(m_Status.m_hWnd, m_CheckBoxShowReal.m_hWnd, m_hInst, TEXT("������л� ��ʵ/��ͼ ��ʾ"));
	CreateToolTip(m_Status.m_hWnd, m_CheckBoxTheme.m_hWnd, m_hInst, TEXT("������л� ��ɫ/ǳɫ ����"));

	//����Trackbar
	m_Trackbar.CreateTrackbar(m_Status.m_hWnd, this->m_hInst, m_Status.GetPartRect(IDR_STATUS_TRACKBAR, 0), IDR_TRACKBAR);
	m_Trackbar.SetRange(6);
	m_Trackbar.SetPos(3);

	//�����Ҵ���
	RightWindow.CreateEx(0, TEXT("RightWindow"), TEXT("RightWindow"),
		WS_CHILD,
		0, 0, 0, 0,
		hWnd,
		(HMENU)ID_RIGHTWINDOW,//id
		m_hInst);
	SetRightWindowPos();
	RightWindow.ShowWindow(TRUE);
	RightWindow.UpdateWindow();

	//��������
	Canvas.CreateEx(0, TEXT("canvas"), TEXT("canvas"),
		WS_CHILD,
		0,
		0,
		0,
		0,
		hWnd,
		(HMENU)ID_CANVAS,//id
		m_hInst);
	SetCanvasPos();
	Canvas.SetDoubleBuffer(true);
	Canvas.ShowWindow(TRUE);
	Canvas.UpdateWindow();

	_tcscpy(szFileName, TEXT(""));

	TCHAR szInputFileName[MAX_PATH];
	if (GetCommandLineByIndex(1, szInputFileName))
	{
		//ShowMessage(szInputFileName);
		_tcscpy(szFileName, szInputFileName);
		::PostMessage(hWnd, WM_COMMAND, MAKELONG(ID_OPEN, ID_OPEN_NOCHECK), 0);//�޵�����
	}
	else
		//Ĭ�ϳ�ʼ��
		this->OnCommand(MAKELONG(ID_NEW, ID_NEW_NOCHECK), 0);

}


void TMainWindow::OnNotify(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId = LOWORD(wParam);
	int wmEvent = HIWORD(wParam);
	if (wmId == IDR_TRACKBAR)
	{
		//�϶���Trackbar
		switch (m_Trackbar.GetPos())
		{
		case 0:
			m_Configuration.SetDPU(0.125); break;
		case 1:
			m_Configuration.SetDPU(0.25); break;
		case 2:
			m_Configuration.SetDPU(0.5); break;
		case 3:
			m_Configuration.SetDPU(1.0); break;
		case 4:
			m_Configuration.SetDPU(2.0); break;
		case 5:
			m_Configuration.SetDPU(4.0); break;
		case 6:
			m_Configuration.SetDPU(8.0); break;
		}
		m_Status.SetText(IDR_STATUS_PROPORTIONNAME, TEXT("������"), m_Configuration.GetProportion() * 100);
		m_Status.SetText(IDR_STATUS_PROPORTION, TEXT("%.0f%%"), m_Configuration.GetProportion() * 100);
		InvalidateRect(Canvas.m_hWnd, &Canvas.ClientRect, false);
	}
}


void TMainWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int wmId = LOWORD(wParam);
	int wmEvent = HIWORD(wParam);
	TManageTool *p_Managetool = &m_ManageTool;
	switch (wmId)
	{
	case ID_NEW:
		if (wmEvent != ID_NEW_NOCHECK)
			if (_tcslen(szFileName) > 0 || m_Shape.Element.size() > 0)
			{
				if (MessageBox(m_hWnd, TEXT("�Ƿ��½��ļ���"), TEXT(""), MB_YESNO) == IDNO)
					break;
			}
		_tcscpy(szFileName, TEXT(""));
		SetText(szName);
		m_ManageTool.CloseCurTool();//�رչ���
		m_Shape.ReleaseAll();//�ͷ�Ԫ��
		RightWindow.TreeViewContent.DeleteAllItems();//ɾ��tree
		RightWindow.ListView.DeleteAllItems();//ɾ��list

		RightWindow.TreeViewContent.Initial();
		this->m_Configuration.SetOrg(this->Canvas.ClientRect.right / 2, this->Canvas.ClientRect.bottom / 2);
		OnCommand(MAKELONG(ID_SELECT, 0), 0);

		pSolver->RefreshEquations();
		::InvalidateRect(Canvas.m_hWnd, &(Canvas.ClientRect), FALSE);
		break;
	case ID_OPEN:
		if (m_Shape.Element.size() > 0 && _tcslen(szFileName) > 0)//�����������е�ǰ�ļ���
		{
			if (MessageBox(m_hWnd, TEXT("�ļ�δ���棬��ȻҪ���ļ���"), NULL, MB_YESNO) == IDNO)
				return;
		}

		if (wmEvent != ID_OPEN_NOCHECK)
		{
			//׼���򿪶Ի���
			OPENFILENAME ofn;
			InitialOpenFileName(&ofn, m_hWnd, szFileName);
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;//�޶��ļ��������
			if (GetOpenFileName(&ofn) == FALSE)
				break;
		}


		//��ΪNew��ı�szFileName�������ȱ����ٻָ�
		TCHAR szFileNameBackup[MAX_PATH];
		_tcscpy(szFileNameBackup, szFileName);
		this->OnCommand(MAKELONG(ID_NEW, ID_NEW_NOCHECK), 0);//��������ʼ��
		_tcscpy(szFileName, szFileNameBackup);

		if (m_Shape.ReadFromFile(szFileName))
		{
			RightWindow.TreeViewContent.AddAllItem();
			SetText(TEXT("%s - %s"), szName, szFileName);
			pSolver->RefreshEquations();
		}
		else
		{//��ȡʧ��
			if (GetLastError() == ERROR_NO)//
				MessageBox(NULL, TEXT("�ļ���ʽ����ȷ����ȡ��ֹ��"), TEXT(""), MB_OK);
			else
				ShowMessage(TEXT("Error:%d"), GetLastError());

			this->OnCommand(MAKELONG(ID_NEW, ID_NEW_NOCHECK), 0);
		}

		break;
	case ID_SAVE:
	{
		if (wmEvent != ID_SAVE_NOCHECK)//���������治���Ի����Թ���if
			if (_tcslen(szFileName) == 0 || GetFileExists(szFileName) == false)//û�е�ǰ�ļ����ߵ�ǰ�ļ���ʧЧ
			{
				//�����Ի���
				OPENFILENAME ofn;
				InitialOpenFileName(&ofn, m_hWnd, szFileName);

				ofn.Flags = OFN_PATHMUSTEXIST;
				ofn.lpstrDefExt = TEXT("mds");

				if (GetSaveFileName(&ofn) == FALSE)
					return;
			}

		if (!m_Shape.SaveToFile(szFileName))
			ShowMessage(TEXT("Error:%d"), GetLastError());
		else
		{
			ShowMessage(TEXT("����%s�ɹ���"), szFileName);
			SetText(TEXT("%s - %s"), szName, szFileName);
		}
	}
	break;
	case ID_SAVEAS:
	{
		OPENFILENAME ofn;
		InitialOpenFileName(&ofn, m_hWnd, szFileName);

		ofn.Flags = OFN_PATHMUSTEXIST;
		ofn.lpstrDefExt = TEXT("mds");

		if (GetSaveFileName(&ofn) == FALSE)
			return;

		this->OnCommand(MAKELONG(ID_SAVE, ID_SAVE_NOCHECK), 0);

		break;
	}
	case ID_EXIT:
		PostMessage(m_hWnd, WM_CLOSE, 0, 0);
		break;
	case ID_SELECT:
	case ID_DRAG:
	case ID_DRAW_BAR:
	case ID_DRAW_FRAME:
	case ID_DRAW_LINE:
	case ID_DRAW_POLYLINE_BAR:
	case ID_DRAW_SLIDEWAY:
	case ID_DRAW_SLIDER:
	case ID_DRAW_COINCIDE:
	case ID_DRAW_COLINEAR:
		SendMessage(m_Toolbar.m_hWnd, WM_USER, wmId, 0);//ˢ��Toolbar���radio״̬
		p_Managetool->SetCurActiveTool(wmId);
		break;
	case ID_DELETE_ELEMENT:
		p_Managetool->m_pCurrentTool->OnKeyDown(m_hWnd, VK_DELETE, 0);
		break;
	case IDR_STATUS_CHECKBOX_SHOW_REAL:
		if (m_CheckBoxShowReal.GetChecked())
		{
			m_CheckBoxShowReal.SetBitmapIsChecked();
			m_Configuration.bDrawReal = true;
		}
		else
		{
			m_CheckBoxShowReal.SetBitmapIsUnChecked();
			m_Configuration.bDrawReal = false;
		}
		Canvas.Invalidate();
		break;
	case IDR_STATUS_CHECKBOX_THEME:
		if (m_CheckBoxTheme.GetChecked())
		{
			m_CheckBoxTheme.SetBitmapIsChecked();
			m_Configuration.bThemeDark = true;
			m_Configuration.SetTheme(true);
		}
		else
		{
			m_CheckBoxTheme.SetBitmapIsUnChecked();
			m_Configuration.bThemeDark = false;
			m_Configuration.SetTheme(false);
		}
		Canvas.Invalidate();
		break;
	case ID_ANALYZE_MECHANISM://��������
		if (pConsole == NULL)
			OnCommand(ID_SHOW_CONSOLE, 0);
		//pSolver->RefreshEquations(true);

		pSolver->ClearOutput();
		//pSolver->ClearEuqations();
		pSolver->Demo();

		break;
	case ID_SHOW_CONSOLE:
	{
		if (pConsole != NULL)
			pConsole->ShowWindow(SW_SHOWNORMAL);
		else
		{
			pConsole = new TConsole;
			pConsole->pSolver = pSolver;
			pConsole->CreateEx(0, TEXT("Console"), TEXT("Console"),
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				m_hWnd, (HMENU)0, m_hInst);
			pConsole->ShowWindow(SW_SHOWNORMAL);
			pConsole->UpdateWindow();
			//pConsole->MessageLoop();
		}

		break;
	}
	case ID_DRAW_GRAPH:
	{
		TGraph *pGraph = new TGraph(&m_Configuration);
		pGraph->bShowMouseLine = true;
		pGraph->CreateEx(0, TEXT("ͼ��"), TEXT("ͼ��"),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			m_hWnd, (HMENU)0, m_hInst);
		pGraph->SetDoubleBuffer(true);
		pGraph->ShowWindow(SW_SHOWNORMAL);


		pGraph->UpdateWindow();


		std::vector<DPOINT> dptVector, dptVector2;
		DPOINT dpt;
		for (double x = 0; x < 1000; ++x)
		{
			dpt.x = x;
			dpt.y = sin(x / 50) * 50;
			dptVector.push_back(dpt);

			dpt.y = cos(x / 50) * 50;
			dptVector2.push_back(dpt);
		}

		pGraph->InputDptVector(dptVector, { PS_SOLID, { 1, 0 }, 0 }, true);
		pGraph->InputDptVector(dptVector2, { PS_SOLID, { 1, 0 }, RGB(255, 0, 0) }, true);

		pGraph->SetMargin(40);

		vecpGraph.push_back(pGraph);
		break;
	}
	case ID_SET_DRIVER:
	{
		if (m_ManageTool.m_uiCurActiveTool == ID_SELECT || m_ManageTool.m_uiCurActiveTool == ID_DRAG)
		{
			if (((TSelectTool *)m_ManageTool.m_pCurrentTool)->CanBeDriver())
			{
				//ѡ�񹤾� �� ѡ��Ԫ�ؿ����� �򵯳�ԭ�����Ի���
				if (((TSelectTool *)m_ManageTool.m_pCurrentTool)->iPickIndex != -1)
				{
					//Ԥ��
					DialogAddDriver::iElementId = m_Shape.Element[((TSelectTool *)m_ManageTool.m_pCurrentTool)->iPickIndex]->id;
					DialogAddDriver::dElementValue = m_Shape.Element[((TSelectTool *)m_ManageTool.m_pCurrentTool)->iPickIndex]->angle;
				}
				if (-1 == DialogBox(m_hInst, MAKEINTRESOURCE(IDD_DIALOG_ADD_DRIVER), m_hWnd, DialogAddDriver::DlgAddDriverProc))
				{
					MessageBox(NULL, TEXT("���ڴ�ʧ�ܡ�"), TEXT(""), MB_ICONERROR);
				}
			}
			else
				MessageBox(m_hWnd, TEXT("����ʹ��ѡ�񹤾�ѡ��һ��Ԫ�أ�����Ϊԭ������"), TEXT(""), MB_ICONINFORMATION);
		}
		else
			MessageBox(m_hWnd, TEXT("����ʹ��ѡ�񹤾�ѡ��һ��Ԫ�أ�����Ϊԭ������"), TEXT(""), MB_ICONINFORMATION);
		break;
	}
	case ID_ANIMATION:
	{
		if (-1 == DialogBox(m_hInst, MAKEINTRESOURCE(IDD_DIALOG_ANIMATION), m_hWnd, DialogAnimation::DlgAnimationProc))
		{
			MessageBox(NULL, TEXT("���ڴ�ʧ�ܡ�"), TEXT(""), MB_ICONERROR);
		}
		break;
	}
	case ID_OPTION:
	{
		if (-1 == DialogBox(m_hInst, MAKEINTRESOURCE(IDD_DIALOG_OPTION), m_hWnd, DlgOptionProc))
		{
			MessageBox(NULL, TEXT("���ڴ�ʧ�ܡ�"), TEXT(""), MB_ICONERROR);
		}
		break;
	}
	case ID_ABOUT:
	{
		if (-1 == DialogBox(m_hInst, MAKEINTRESOURCE(IDD_DIALOG_ABOUT), m_hWnd, DlgAboutProc))
		{
			MessageBox(NULL, TEXT("���ڴ�ʧ�ܡ�"), TEXT(""), MB_ICONERROR);
		}
		break;
	}
	case ID_DELETE_GRAPH:
		for (auto iter = vecpGraph.begin(); iter != vecpGraph.end(); ++iter)
		{
			if (*iter == (TGraph *)lParam)
			{
				delete *iter;
				vecpGraph.erase(iter);
				break;
			}
		}
		break;
	case ID_DELETE_CONSOLE:
		if (pConsole != NULL)
		{
			delete pConsole;
			pConsole = NULL;
		}
		break;
	case ID_REFRESH:
		pSolver->RefreshEquations();
		pSolver->ClearOutput();
		pSolver->ClearConstraint();
		//pSolver->AddMouseConstraint(false, iPickIndex, pConfig->ScreenToReal(ptPos));
		pSolver->Solve();
		::InvalidateRect(Canvas.m_hWnd, &(Canvas.ClientRect), FALSE);
		break;
	case ID_VIEW_SUITABLE:
		//DPOINT center;
		//double left, right, top, bottom;

		break;
	case IDR_LINEEDIT://not deal
		break;
	default:
		assert(0);
		break;
	}
}

void TMainWindow::SetCanvasPos()
{
	::MoveWindow(Canvas.m_hWnd,
		ClientRect.left,
		ClientRect.top + m_Toolbar.GetClientRect().bottom,
		ClientRect.right - ClientRect.left - (RightWindow.ClientRect.right - RightWindow.ClientRect.left),
		ClientRect.bottom - m_Toolbar.GetClientRect().bottom - m_Status.GetClientRect().bottom, true);
}

void TMainWindow::SetRightWindowPos()
{
	::MoveWindow(RightWindow.m_hWnd,
		ClientRect.right - m_iRightWindowWidth,
		ClientRect.top + m_Toolbar.GetClientRect().bottom,
		m_iRightWindowWidth,
		ClientRect.bottom - m_Toolbar.GetClientRect().bottom - m_Status.GetClientRect().bottom, true);
}

//ֻ�д˴��������ִ�С
void TMainWindow::OnSize(WPARAM wParam, LPARAM lParam)
{
	if (wParam == SIZE_MINIMIZED)
		return;
	m_Toolbar.FreshSize();
	m_Status.FreshSize();
	m_Trackbar.SetRect(m_Status.GetPartRect(IDR_STATUS_TRACKBAR, 0));//TrackbarǶ��Status
	m_CheckBoxShowReal.SetPositionOnlyOrigin(m_Status.GetPartRect(IDR_STATUS_CHECKBOX_SHOW_REAL, 1));
	m_CheckBoxTheme.SetPositionOnlyOrigin(m_Status.GetPartRect(IDR_STATUS_CHECKBOX_THEME, 1));

	SetRightWindowPos();


	//��¼������ԭ����Ա�����Canvas�䶯���ٰ������ָ�����λ��
	DPOINT OrgProportion;
	if (Canvas.ClientRect.right != 0 && Canvas.ClientRect.bottom != 0)
	{
		OrgProportion.x = (double)m_Configuration.GetOrg().x / (double)Canvas.ClientRect.right;
		OrgProportion.y = (double)m_Configuration.GetOrg().y / (double)Canvas.ClientRect.bottom;
	}
	else
		OrgProportion = { 1.0, 1.0 };

	SetCanvasPos();

	m_Configuration.SetOrg((LONG)(OrgProportion.x*Canvas.ClientRect.right), (LONG)(OrgProportion.y*Canvas.ClientRect.bottom));

	Canvas.Invalidate();
}
