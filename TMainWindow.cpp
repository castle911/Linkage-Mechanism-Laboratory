#pragma once
#include "DetectMemoryLeak.h"

#include <process.h>
#include "TMainWindow.h"

#include "TConstraintCoincide.h" 
#include "TSolver.h"
#include "TConsole.h"
#include "TSelectTool.h"

#include "DialogAddDriver.h"

#include "TGraph.h"

#define _CRT_SECURE_NO_WARNINGS

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
	m_Toolbar.AddGroup(0,0, ID_SELECT, true, TEXT("ѡ��"));
	m_Toolbar.AddGroup(1, 0, ID_DRAG, true, TEXT("�϶�"));
	m_Toolbar.AddButton(2,  ID_REFRESH, true, TEXT("ˢ��"));
	m_Toolbar.AddSeparator(0);
	m_Toolbar.AddGroup(3, 0, ID_DRAW_FRAME, true, TEXT("����"));
	m_Toolbar.AddGroup(4, 0, ID_DRAW_BAR, true, TEXT("����"));
	m_Toolbar.AddGroup(5, 0, ID_DRAW_LINE, true, TEXT("��"));
	m_Toolbar.AddGroup(6, 0, ID_DRAW_SLIDEWAY, true, TEXT("����"));
	m_Toolbar.AddGroup(7, 0, ID_DRAW_SLIDER, true, TEXT("����"));
	m_Toolbar.AddButton(8, ID_SET_DRIVER, true, TEXT("��Ϊԭ����"));
	m_Toolbar.ShowToolbar();

	//����״̬��
	m_Status.Create(hWnd, IDR_STATUS, m_hInst, 24);
	m_Status.AddPart(IDR_STATUS_COORDINATE, 160, PT_FIXED);//����
	m_Status.AddPart(0, 0, PT_NONE);
	m_Status.AddPart(IDR_STATUS_PROPORTIONNAME, 40, PT_FIXED);//������
	m_Status.AddPart(IDR_STATUS_TRACKBAR, 120, PT_FIXED);//
	m_Status.AddPart(IDR_STATUS_PROPORTION, 60, PT_FIXED);//100%
	m_Status.AddPart(0, 6, PT_FIXED);//�����϶����ǵ�
	m_Status.FreshSize();

	m_Status.SetText(IDR_STATUS_PROPORTIONNAME, TEXT("������"), m_Configuration.GetProportion() * 100);
	m_Status.SetText(IDR_STATUS_PROPORTION, TEXT("%.0f%%"), m_Configuration.GetProportion() * 100);

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

bool GetFileExists(TCHAR filename[])
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(filename, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		return false;
	}
	else {
		FindClose(hFind);
		return true;
	}
}

void InitialOpenFileName(OPENFILENAME *ofn, HWND hwnd, TCHAR szFile[], DWORD nMaxFile = MAX_PATH)
{
	// Initialize OPENFILENAME
	ZeroMemory(ofn, sizeof(OPENFILENAME));
	ofn->lStructSize = sizeof(OPENFILENAME);
	ofn->hwndOwner = hwnd;
	ofn->lpstrFile = szFile;
	ofn->lpstrFile[0] = TEXT('\0');
	ofn->nMaxFile = nMaxFile;
	ofn->lpstrFilter = TEXT("��������ļ�(*.mds)\0*.mds\0\0");
	ofn->nFilterIndex = 1;
	ofn->lpstrFileTitle = NULL;
	ofn->nMaxFileTitle = 0;
	ofn->lpstrInitialDir = NULL;
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
		m_ManageTool.CloseCurTool();
		m_Shape.ReleaseAll();
		RightWindow.TreeViewContent.DeleteAllItems();
		RightWindow.ListView.DeleteAllItems();

		RightWindow.TreeViewContent.Initial();
		this->m_Configuration.SetOrg(this->Canvas.ClientRect.right / 2, this->Canvas.ClientRect.bottom / 2);
		m_ManageTool.SetCurActiveTool(ID_SELECT);

		//m_Solver.RefreshEquations(true);
		pSolver->RefreshEquations(true);
		::InvalidateRect(Canvas.m_hWnd, &(Canvas.ClientRect), FALSE);
		break;
	case ID_OPEN:
		if (m_Shape.Element.size() > 0 && _tcslen(szFileName) > 0)//�����������е�ǰ�ļ���
		{
			if (MessageBox(m_hWnd, TEXT("�ļ�δ���棬��ȻҪ���ļ���"), NULL, MB_YESNO) == IDNO)
				return;
		}

		//׼���򿪶Ի���
		OPENFILENAME ofn;
		HANDLE hf;

		InitialOpenFileName(&ofn, m_hWnd, szFileName);
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (GetOpenFileName(&ofn) == TRUE)
		{
			TCHAR szFileNameBackup[MAX_PATH];
			_tcscpy(szFileNameBackup, szFileName);
			this->OnCommand(MAKELONG(ID_NEW, ID_NEW_NOCHECK), 0);//��������ʼ��
			_tcscpy(szFileName, szFileNameBackup);

			hf = CreateFile(ofn.lpstrFile,
				GENERIC_READ,
				0,
				(LPSECURITY_ATTRIBUTES)NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				(HANDLE)NULL);
			if (GetLastError() != 0)
			{
				ShowMessage(TEXT("Error:%d"), GetLastError());
				return;
			}

			//����Ԫ������
			DWORD real_pos = 0;
			DWORD *now_pos = &real_pos;
			int size;
			ReadFile(hf, &size, sizeof(size), now_pos, NULL);
			if (GetLastError() != 0)
			{
				ShowMessage(TEXT("Error:%d"), GetLastError());
				return;
			}
			*now_pos += sizeof(size);

			EnumElementType eType;
			for (int i = 0; i < size; i++)
			{
				//��������
				ReadFile(hf, &eType, sizeof(EnumElementType), now_pos, NULL);
				if (GetLastError() != 0)
				{
					ShowMessage(TEXT("Error:%d"), GetLastError());
					return;
				}
				*now_pos += sizeof(EnumElementType);

				//�������Ͷ���Ԫ��
				switch (eType)
				{
				case ELEMENT_BAR:
				{
					TBar *temp = new TBar;
					ReadFile(hf, temp, m_Shape.GetSizeOfElement(eType), now_pos, NULL);
					if (GetLastError() != 0)
					{
						ShowMessage(TEXT("Error:%d"), GetLastError());
					}
					else
						m_Shape.AddElement(temp);
					delete temp;
					break;
				}
				case ELEMENT_REALLINE:
				{
					TRealLine *temp = new TRealLine;
					ReadFile(hf, temp, m_Shape.GetSizeOfElement(eType), now_pos, NULL);
					if (GetLastError() != 0)
					{
						ShowMessage(TEXT("Error:%d"), GetLastError());
					}
					else
						m_Shape.AddElement(temp);
					delete temp;
					break;
				}
				case ELEMENT_FRAMEPOINT:
				{
					TFramePoint *temp = new TFramePoint;
					ReadFile(hf, temp, m_Shape.GetSizeOfElement(eType), now_pos, NULL);
					if (GetLastError() != 0)
					{
						ShowMessage(TEXT("Error:%d"), GetLastError());
					}
					else
						m_Shape.AddElement(temp); 
					delete temp;
					break;
				}
				case ELEMENT_SLIDEWAY:
				{
					TSlideway *temp = new TSlideway;
					ReadFile(hf, temp, m_Shape.GetSizeOfElement(eType), now_pos, NULL);
					if (GetLastError() != 0)
					{
						ShowMessage(TEXT("Error:%d"), GetLastError());
					}
					else
						m_Shape.AddElement(temp);
					delete temp;
					break;
				}
				case CONSTRAINT_COINCIDE:
				{
					TConstraintCoincide *temp = new TConstraintCoincide;
					ReadFile(hf, temp, m_Shape.GetSizeOfElement(eType), now_pos, NULL);
					if (GetLastError() != 0)
					{
						ShowMessage(TEXT("Error:%d"), GetLastError());
					}
					else
					{
						//�����Լ������������Ӽ�id
						int id1, id2;
						ReadFile(hf, &id1, sizeof(id1), now_pos, NULL);
						ReadFile(hf, &id2, sizeof(id2), now_pos, NULL);

						temp->pElement[0] = m_Shape.GetElementById(id1);
						temp->pElement[1] = m_Shape.GetElementById(id2);

						temp->BuildpDpt();

						m_Shape.AddElement(temp);
					}
					delete temp;
					break;
				}
				default:
					assert(0);
					break;
				}
			}

			CloseHandle(hf);
			RightWindow.TreeViewContent.AddAllItem();
			SetText(TEXT("%s - %s"), szName, szFileName);
			//m_Solver.RefreshEquations(true);
			pSolver->RefreshEquations(true);
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

		//�޵���ֱ�Ӵ洢
		HANDLE hf;
		hf = CreateFile(szFileName,
			GENERIC_WRITE,
			0,
			(LPSECURITY_ATTRIBUTES)NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			(HANDLE)NULL);
		if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != 0)
		{
			ShowMessage(TEXT("Error:%d"), GetLastError());
			return;
		}
		DWORD real_pos = 0;
		DWORD *now_pos = &real_pos;

		//д��Ԫ������
		int size = m_Shape.Element.size();
		WriteFile(hf, &size, sizeof(size), now_pos, NULL);
		*now_pos += sizeof(size);
		if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != 0)
		{
			ShowMessage(TEXT("Error:%d"), GetLastError());
			return;
		}

		//����д��Element
		for (auto element : m_Shape.Element)
		{
			//д�����ͱ��
			WriteFile(hf, &(element->eType), sizeof(EnumElementType), now_pos, NULL);
			*now_pos += sizeof(EnumElementType);
			if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != 0)
			{
				ShowMessage(TEXT("Error:%d"), GetLastError());
				return;
			}

			//д������
			WriteFile(hf, element, m_Shape.GetSizeOfElement(element->eType), now_pos, NULL);
			*now_pos += m_Shape.GetSizeOfElement(element->eType);
			if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != 0)
			{
				ShowMessage(TEXT("Error:%d"), GetLastError());
				return;
			}

			//�����Լ����д�������Ӽ�id
			if (element->eType == CONSTRAINT_COINCIDE)
			{
				TConstraintCoincide *temp = (TConstraintCoincide *)element;
				WriteFile(hf, &(temp->pElement[0]->id), sizeof(temp->pElement[0]->id), now_pos, NULL);
				*now_pos += sizeof(temp->pElement[0]->id);
				WriteFile(hf, &(temp->pElement[1]->id), sizeof(temp->pElement[1]->id), now_pos, NULL);
				*now_pos += sizeof(temp->pElement[1]->id);
			}

		}
		CloseHandle(hf);
		ShowMessage(TEXT("����%s�ɹ���"), szFileName);
		SetText(TEXT("%s - %s"), szName, szFileName);

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
	case ID_DRAW_SLIDEWAY:
	case ID_DRAW_SLIDER:
		SendMessage(m_Toolbar.m_hWnd, WM_USER, wmId, 0);
		p_Managetool->SetCurActiveTool(wmId);
		break;
	case ID_ANALYZE_MECHANISM:
		if (pConsole == NULL)
			OnCommand(ID_SHOW_CONSOLE, 0);
		pSolver->RefreshEquations(true);
		break;
	case ID_SHOW_CONSOLE:
	{
		if (pConsole != NULL)
			pConsole->ShowWindow(SW_SHOWNORMAL);
		else
		{
			pConsole = new TConsole;
			//pConsole->pSolver = &m_Solver;
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

		vecpGraph.push_back(pGraph);

		std::vector<DPOINT> dptVector;
		DPOINT dpt;
		double y;
		//for (double t = 0; t < 1000; ++t)
		//{
		//	dpt.x = t / 10 * cos(t / 10);
		//	dpt.y = t / 10 * sin(t / 10);
		//	dptVector.push_back(dpt);
		//}
		for (double x = 0; x < 1000; ++x)
		{
			dpt.x = x;
			dpt.y = sin(x / 50);
			dptVector.push_back(dpt);
		}
		pGraph->InputDptVector(dptVector);
		break;
	}
	case ID_SET_DRIVER:
	{
		if (m_ManageTool.m_uiCurActiveTool == ID_SELECT)
		{
			if (((TSelectTool *)m_ManageTool.m_pCurrentTool)->CanBeDriver())
				//ѡ�񹤾� �� ѡ��Ԫ�ؿ����� �򵯳�ԭ�����Ի���
				if (-1 == DialogBox(m_hInst, MAKEINTRESOURCE(IDD_DIALOG_ADD_DRIVER), m_hWnd, DlgAddDriverProc))
				{
					MessageBox(NULL, TEXT("���ڴ�ʧ�ܡ�"), TEXT(""), MB_ICONERROR);
				}
				else
					break;
		}

		MessageBox(m_hWnd, TEXT("����ʹ��ѡ�񹤾�ѡ��һ��Ԫ�أ�����Ϊԭ������"), TEXT(""), MB_ICONINFORMATION);
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
		pSolver->RefreshEquations(true);
		pSolver->ClearOutput();
		pSolver->ClearConstraint();
		//pSolver->AddMouseConstraint(false, iPickIndex, pConfig->ScreenToReal(ptPos));
		pSolver->Solve(true);
		::InvalidateRect(Canvas.m_hWnd, &(Canvas.ClientRect), FALSE);
		break;
	case ID_VIEW_SUITABLE:
		//DPOINT center;
		//double left, right, top, bottom;

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
	m_Trackbar.MoveWindow(m_Status.GetPartRect(IDR_STATUS_TRACKBAR, 0));//TrackbarǶ��Status

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
