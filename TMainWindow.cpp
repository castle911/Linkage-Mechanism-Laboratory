#pragma once
#include "TMainWindow.h"

TMainWindow::TMainWindow()
{
	m_iRightWindowWidth = 200;
}


TMainWindow::~TMainWindow()
{
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
	m_Toolbar.AddButton(0, ID_SELECT, true, TEXT("ѡ��"));
	m_Toolbar.AddSeparator(0);
	m_Toolbar.AddButton(1, ID_DRAW_FRAME, true, TEXT("����"));
	m_Toolbar.AddButton(2, ID_DRAW_BAR, true, TEXT("����"));
	m_Toolbar.AddButton(3, ID_DRAW_LINE, true, TEXT("��"));
	m_Toolbar.ShowToolbar();

	//����״̬��
	m_Status.Create(hWnd, IDR_STATUS, m_hInst, 24);
	m_Status.AddPart(IDR_STATUS_COORDINATE, 160, PT_FIXED);
	m_Status.AddPart(0, 0, PT_NONE);
	m_Status.AddPart(IDR_STATUS_PROPORTIONNAME, 40, PT_FIXED);
	m_Status.AddPart(IDR_STATUS_TRACKBAR, 120, PT_FIXED);
	m_Status.AddPart(IDR_STATUS_PROPORTION, 60, PT_FIXED);
	m_Status.AddPart(0, 6, PT_FIXED);
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
		0,0,0,0,
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


	////����������
	//TToolbar m_Toolbar2;
	//m_Toolbar2.CreateToolbar(Canvas.m_hWnd, m_hInst);
	//m_Toolbar2.LoadImageList(32, 32, IDR_TOOLBAR_MAIN, RGB(255, 255, 255));
	//m_Toolbar2.AddButton(0, ID_SELECT, true, TEXT("ѡ��"));
	//m_Toolbar2.AddSeparator(0);
	//m_Toolbar2.AddButton(1, ID_DRAW_LINE, true, TEXT("����"));
	//m_Toolbar2.AddButton(2, ID_DRAW_LINE, true, TEXT("����"));
	//m_Toolbar2.AddGroup(3, ID_NEW, true, NULL);
	//m_Toolbar2.ShowToolbar();

	//Ĭ�ϳ�ʼ��
	this->OnCommand(MAKELONG(ID_NEW, 0), 0);

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
	case ID_ACCELERATOR_NEW:
	case ID_NEW:
		this->m_Configuration.SetOrg(this->Canvas.ClientRect.right / 2, this->Canvas.ClientRect.bottom / 2);

		break;
	case ID_EXIT:
		PostMessage(m_hWnd, WM_CLOSE, 0, 0);
		break;
	case ID_SELECT:
		p_Managetool->SetCurActiveTool(ID_SELECT);
		break;
	case ID_DRAW_FRAME:
		p_Managetool->SetCurActiveTool(ID_DRAW_FRAME);
		break;
	case ID_DRAW_LINE:
		p_Managetool->SetCurActiveTool(ID_DRAW_LINE);
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
		OrgProportion.x = (double)m_Configuration.GetOrg().x / (double)Canvas.ClientRect.right;
		OrgProportion.y = (double)m_Configuration.GetOrg().y / (double)Canvas.ClientRect.bottom;

	SetCanvasPos();

		m_Configuration.SetOrg((LONG)(OrgProportion.x*Canvas.ClientRect.right), (LONG)(OrgProportion.y*Canvas.ClientRect.bottom));

	::InvalidateRect(Canvas.m_hWnd, &Canvas.ClientRect, FALSE);
}
