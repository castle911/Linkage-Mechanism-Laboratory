#pragma once
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

#include "resource.h"
#include "TConfiguration.h"
#include "TMainWindow.h"
#include "TCanvas.h"
#include "TManageTool.h"
#include "TLineTool.h"
#include "TDraw.h"

extern TMainWindow win;
TCanvas::TCanvas()
{
	Config = NULL;
	bMButtonPressing = false;
}


TCanvas::~TCanvas()
{
	Config = NULL;
}

void TCanvas::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	Config = &(win.m_Configuration);
}

void TCanvas::DealMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TManageTool *pManageTool = &(win.m_ManageTool);
	if (pManageTool->m_pCurrentTool != NULL)
	{
		pManageTool->Message(hWnd, uMsg, wParam, lParam);
	}
	if (uMsg == WM_SETCURSOR)
		pManageTool->SetCursor(hWnd, uMsg, wParam, lParam);
}

void TCanvas::OnKeyDown(WPARAM wParam, LPARAM lParam)	
{
	DealMessage(m_hWnd, WM_KEYDOWN, wParam, lParam);
}

void TCanvas::OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetFocus(hWnd);
	DealMessage(hWnd, uMsg, wParam, lParam);
	InvalidateRect(this->m_hWnd, &(this->ClientRect), false);
}

void TCanvas::OnRButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetFocus(hWnd);
	DealMessage(hWnd, uMsg, wParam, lParam);
}

void TCanvas::OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT ptPos;

	ptPos.x = LOWORD(lParam);
	ptPos.y = HIWORD(lParam);

	switch (Config->uUnits)
	{
	case UNITS_MM:
		win.m_Status.SetText(IDR_STATUS_COORDINATE, TEXT("(%.3f mm,%.3f mm)"), Config->ScreenToReal(ptPos).x, Config->ScreenToReal(ptPos).y);
		break;
	case UNITS_INCH:
		win.m_Status.SetText(IDR_STATUS_COORDINATE, TEXT("(%.3f in,%.3f in)"), Config->ScreenToReal(ptPos).x, Config->ScreenToReal(ptPos).y);
		break;
	}

	//���м����£��϶�����ԭ�㲢ˢ����ʾ
	if (bMButtonPressing)
	{
		Config->SetOrg(Config->GetOrg().x + (ptPos.x - uiMoveStartX),
			Config->GetOrg().y + (ptPos.y - uiMoveStartY));
		//Config->Org.x += (ptPos.x - uiMoveStartX);
		//Config->Org.y += (ptPos.y - uiMoveStartY);
		uiMoveStartX = ptPos.x;
		uiMoveStartY = ptPos.y;
	}

	DealMessage(hWnd, uMsg, wParam, lParam);

	InvalidateRect(this->m_hWnd, &(this->ClientRect), false);
}

void TCanvas::OnSetCursor(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (bMButtonPressing)
		SetCursor(LoadCursor(NULL, IDC_SIZEALL));
	else
		DealMessage(hWnd, uMsg, wParam, lParam);
}

void TCanvas::OnMButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bMButtonPressing = true;
	uiMoveStartX = LOWORD(lParam);
	uiMoveStartY = HIWORD(lParam);
	PostMessage(hWnd, WM_SETCURSOR, 0, 0);
}

void TCanvas::OnMButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bMButtonPressing = false;
	PostMessage(hWnd, WM_SETCURSOR, 0, 0);
}

void TCanvas::OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	POINT ptPos;
	ptPos.x = LOWORD(lParam);
	ptPos.y = HIWORD(lParam);
	//WM_MOUSEWHEEL�¼���õ����������Ļ�ľ�������
	ScreenToClient(hWnd, &ptPos);

	//����ȡ�����λ�õ�ԭ���ʵ�ʾ��룬�ڱ����䶯������ͬ��ʵ�ʾ����ƶ��µ�ԭ��λ�á�
	//�䶯ǰ���������ʵ�ʾ��벻�䡣
	double xlen = Config->ScreenToLengthX(ptPos.x - Config->GetOrg().x);
	double ylen = Config->ScreenToLengthY(ptPos.y - Config->GetOrg().y);

	double OldProportion = Config->GetProportion();
	if (zDelta > 0)//��ǰ��
		Config->SetDPU(Config->GetProportion() * 2);
	else
		Config->SetDPU(Config->GetProportion() / 2);

	Config->SetOrg(ptPos.x - Config->LengthToScreenX(xlen), ptPos.y - Config->LengthToScreenY(ylen));

	win.m_Status.SetText(IDR_STATUS_PROPORTIONNAME, TEXT("������"), Config->GetProportion() * 100);
	win.m_Status.SetText(IDR_STATUS_PROPORTION, TEXT("%.0f%%"), Config->GetProportion() * 100);

	int iProportion = (int)(Config->GetProportion() * 100);
	if (iProportion < 12.5)
		win.m_Trackbar.SetPos(0);
	else
		if (iProportion > 800)
			win.m_Trackbar.SetPos(6);
		else
			switch (iProportion)
		{
			case 13:
				win.m_Trackbar.SetPos(0);
				break;
			case 25:
				win.m_Trackbar.SetPos(1);
				break;
			case 50:
				win.m_Trackbar.SetPos(2);
				break;
			case 100:
				win.m_Trackbar.SetPos(3);
				break;
			case 200:
				win.m_Trackbar.SetPos(4);
				break;
			case 400:
				win.m_Trackbar.SetPos(5);
				break;
			case 800:
				win.m_Trackbar.SetPos(6);
				break;
		}

	DealMessage(hWnd, uMsg, wParam, lParam);
	::InvalidateRect(m_hWnd, &ClientRect, false);
}

void TCanvas::OnDraw(HDC hdc)
{
	SetBkMode(hdc, TRANSPARENT);

	//��䱳��
	//RECT rect;
	//GetClientRect(m_hWnd, &rect);
	TDraw::FillRect(hdc, &ClientRect, Config->crBackground);

	//������
	TDraw::DrawGrid(hdc, ClientRect, Config);

	//������ԭ��
	TDraw::DrawAxes(hdc, Config->GetOrg().x, Config->GetOrg().y, Config->crCoordinate);

	//ͼ�λ���
	for (unsigned int i = 0; i < win.m_Shape.RealLine.size(); i++)
	{
		TDraw::DrawRealLine(hdc, win.m_Shape.RealLine[i], Config);
	}

	for (unsigned int i = 0; i < win.m_Shape.FramePoint.size(); i++)
	{
		TDraw::DrawFramePoint(hdc, win.m_Shape.FramePoint[i], Config);
	}

	//���������
	if (win.m_ManageTool.m_pCurrentTool!=NULL)
		win.m_ManageTool.m_pCurrentTool->Draw(hdc);//������ʹ���е�ͼ�λ��ƽ��ɹ�����ִ��

}