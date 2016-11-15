#pragma once
#include "TLineTool.h"

#include "resource.h"
#include "TTool.h"
#include "TLine.h"

#include "TConfiguration.h"
#include "TMainWindow.h"
#include "TDraw.h"
#include "TLineEdit.h"
#include "TAttach.h"

extern TMainWindow win;
TLineTool::TLineTool() 
{
	Attach = new TAttach(win.Canvas.m_hWnd, &(win.m_Shape), &(win.m_Configuration));
	Config = &(win.m_Configuration);
	bShowDimLine = false;
	MoveLine = new TRealLine;
	MoveLine->SetStyle(PS_SOLID, 1, Config->crPen);

	Line1 = new TLine;
	Line2 = new TLine; 
	LineDim = new TLine;
	Line1->SetStyle(PS_DOT, 1, Config->crPen);
	Line2->SetStyle(PS_DOT, 1, Config->crPen);
	LineDim->SetStyle(PS_DOT, 1, Config->crPen);

	LineEdit = new TLineEdit;
}


TLineTool::~TLineTool()
{
	DestroyWindow(LineEdit->m_hWnd);
	delete Attach;

	delete LineEdit;
	delete MoveLine;

	delete Line1;
	delete Line2;
	delete LineDim;
}

void TLineTool::OnMouseWheel(HWND hWnd, UINT nFlags, POINT ptPos)
{
	OnMouseMove(hWnd, nFlags, ptPos);
}
 
void TLineTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	Attach->AttachAll(ptPos,MoveLine->ptBegin);
	MoveLine->ptEnd = Attach->dptAttach;

	if (dptHit.size() > 0)
	{
		//�����ݴ��߳��Ȳ�����Edit
		MoveLine->CalcLength();
		LineEdit->SetText(TEXT("%.3f"), MoveLine->dLength);
		SetFocus(LineEdit->m_hWnd);
		LineEdit->SetSelectAll();

		//����ߴ��߸�������
		int dist = 40;//�ݴ��ߺͳߴ��߾���
		double theta = TDraw::GetAngleFromPointReal(MoveLine->ptBegin, MoveLine->ptEnd);
		pt1 = Config->RealToScreen(MoveLine->ptBegin);
		pt2 = Config->RealToScreen(MoveLine->ptEnd);
		Line1->ptBegin = pt1;
		Line2->ptBegin = pt2;
		if (MoveLine->ptEnd.y - MoveLine->ptBegin.y >= 0)//λ��1,2����
		{
			TDraw::Move(&pt1, 1, theta + M_PI / 2, dist);
			TDraw::Move(&pt2, 1, theta + M_PI / 2, dist);
		}
		else
		{
			TDraw::Move(&pt1, 1, theta - M_PI / 2, dist);
			TDraw::Move(&pt2, 1, theta - M_PI / 2, dist);
		}
		Line1->ptEnd = pt1;
		Line2->ptEnd = pt2;
		LineDim->ptBegin = pt1; LineDim->ptEnd = pt2;
		//���ߴ��߳���Ϊ0����ʾ
		if (LineDim->ptBegin.x == LineDim->ptEnd.x && LineDim->ptBegin.y == LineDim->ptEnd.y)
			bShowDimLine = false;
		else
			bShowDimLine = true;

		//����Editλ�ò���ʾ
		if (LineEdit->m_hWnd != NULL && bShowDimLine == true)
		{
			int width = 60, height = 20;
			POINT ptMiddle;
			ptMiddle.x = (LineDim->ptEnd.x + LineDim->ptBegin.x) / 2;
			ptMiddle.y = (LineDim->ptEnd.y + LineDim->ptBegin.y) / 2;

			LineEdit->SetPos(ptMiddle.x - width / 2, ptMiddle.y - height / 2, width, height);
			LineEdit->SetVisible(true);
		}
		else
			LineEdit->SetVisible(false);
	}

	//��Canvasˢ��
}

//�����Ȳ���WM_PAINT�¼��н��л���
void TLineTool::Draw(HDC hdc)
{

	Attach->Draw(hdc);

	//����ʱ�߼��ߴ���
	if (dptHit.size() > 0)
	{
		TDraw::DrawRealLine(hdc, *MoveLine,Config);//���һ�κ�ſ�ʼ����ʱ��

		if (bShowDimLine)
		{
			TDraw::DrawLine(hdc, *Line1);
			TDraw::DrawLine(hdc, *Line2);
			TDraw::DrawLine(hdc, *LineDim);
		}

	}
}

//LineEdit����ʱCanvas�ǽ��ղ���KEYDOWN�ģ���ȻLineToolҲ���ղ�����
//KEYDOWN��Ϣ��LineEdit�ػ��͸�Canvas���ٴ���������
void TLineTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_ESCAPE:
		OnRButtonDown(hWnd, 0, { 0, 0 });
		break;
	case VK_SPACE:
	case VK_RETURN:
		if (LineEdit->GetVisible())
		{
			//�õ��ַ���ת��
			double num;
			TCHAR buffer[64];
			LineEdit->GetText(buffer);
			num = TTransfer::TCHAR2double(buffer);

			TRealLine RealLine;
			RealLine.ptBegin = dptHit[dptHit.size() - 1];

			//����õ��յ�����
			switch (Attach->GetiIvoryLine())
			{
			case 0:
				double angle;
				angle=TDraw::GetAngleFromPointReal(RealLine.ptBegin, MoveLine->ptEnd);
				RealLine.SetPoint(RealLine.ptBegin, num, angle);
				break;
			case 1:
				RealLine.ptEnd = { RealLine.ptBegin.x + num, RealLine.ptBegin.y };
				RealLine.dLength = num;
				break;
			case 3:
				RealLine.ptEnd = { RealLine.ptBegin.x - num, RealLine.ptBegin.y };
				RealLine.dLength = num;
				break;
			case 2:
				RealLine.ptEnd = { RealLine.ptBegin.x, RealLine.ptBegin.y + num };
				RealLine.dLength = num;
				break;
			case 4:
				RealLine.ptEnd = { RealLine.ptBegin.x, RealLine.ptBegin.y - num };
				RealLine.dLength = num;
				break;
			}
			RealLine.SetStyle(Config->iStyle, Config->iWidth, Config->crPen);

			//���
			win.m_Shape.AddRealLine(RealLine);

			//����ó����յ�����ݴ�㼯
			dptHit.push_back(RealLine.ptEnd);

			//������ʱ��
			POINT ptNew = Config->RealToScreen(RealLine.ptEnd);
			InitialLine(RealLine.ptEnd);
			Attach->InitialLine(ptNew);

			LineEdit->SetVisible(false);//�����ڻ����ߺ���ʾEdit

			//�ƶ���������
			TDraw::ClientPosToScreen(hWnd, &ptNew);
			SetCursorPos(ptNew.x, ptNew.y);
		}
		break;
	}
}
void TLineTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	ptPos = Config->RealToScreen(MoveLine->ptEnd);

	//��һ�㼰��ǰ���������
	if (dptHit.size() > 0)
	{
		TRealLine RealLine;
		RealLine.SetStyle(Config->iStyle, Config->iWidth, Config->crPen);
		RealLine.SetPoint(dptHit[dptHit.size() - 1], MoveLine->ptEnd);
		win.m_Shape.AddRealLine(RealLine);
	}

	//��ǰ������ݴ�㼯
	dptHit.push_back(MoveLine->ptEnd);

	//������ʱ��
	InitialLine(MoveLine->ptEnd);
	Attach->InitialLine(ptPos);

	//����LineEdit������
	if (LineEdit->m_hWnd == NULL)
	{
		LineEdit->CreateEditEx(win.Canvas.m_hWnd, IDR_LineEdit, win.Canvas.m_hInst);
		LineEdit->Load(hWnd);
	}
	else
		LineEdit->SetVisible(false);//�����ڻ����ߺ���ʾEdit

	//��������ˢ��
}

void TLineTool::InitialLine(DPOINT dptPos)
{
	POINT ptPos = Config->RealToScreen(dptPos);
	MoveLine->ptBegin = dptPos;
	MoveLine->ptEnd = dptPos;

	Line1->ptBegin = ptPos;
	Line1->ptEnd = ptPos;
	Line2->ptBegin = ptPos;
	Line2->ptEnd = ptPos;
	LineDim->ptBegin = ptPos;
	LineDim->ptEnd = ptPos;
	bShowDimLine = true;
}

void TLineTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	//���Ѵ���Edit������Edit
	if (LineEdit->m_hWnd != NULL)
		LineEdit->SetVisible(false);

	bShowDimLine = false;


	//û�л��ߵ�����µ��Ҽ������ù���
	if (dptHit.size()==0)
		::PostMessage(win.m_hWnd, WM_COMMAND, ID_SELECT, 0);
	else
	{
		dptHit.clear();

		::InvalidateRect(win.Canvas.m_hWnd, &win.Canvas.ClientRect, false);

	}
}

void TLineTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_CROSS));
}