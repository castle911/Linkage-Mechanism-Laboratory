#pragma once
#include "TLineTool.h"

extern TMainWindow win;
TLineTool::TLineTool() 
{
	Config = &(win.m_Configuration);
	m_uiHit = 0;
	m_pptHit = NULL;
	bShowXAssist = false;
	bShowYAssist = false;
	bShowDimLine = false;
	MoveLine = new TRealLine(PS_SOLID, 1, Config->crPen);
	XAssistLine = new TRealLine(PS_DOT, 1, Config->crDot);
	YAssistLine = new TRealLine(PS_DOT, 1, Config->crDot);

	Line1 = new TLine(PS_DOT, 1, Config->crPen);
	Line2 = new TLine(PS_DOT, 1, Config->crPen);
	LineDim = new TLine(PS_DOT, 1, Config->crPen);

	LineEdit = new TEdit;
}


TLineTool::~TLineTool()
{
	DestroyWindow(LineEdit->m_hWnd);
	delete LineEdit;
	delete MoveLine;
	delete XAssistLine;
	delete YAssistLine;

	delete Line1;
	delete Line2;
	delete LineDim;
}


void TLineTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//switch (wParam)
	//{
	//case VK_SPACE:
	//case VK_RETURN:
	//	::SendMessage(win.Canvas.m_hWnd, WM_LBUTTONDOWN, 0, MAKELONG(Config->RealToScreenX(MoveLine->ptEnd.x),Config->RealToScreenY(MoveLine->ptEnd.y)));
	//	break;
	//}
}

void TLineTool::OnMouseWheel(HWND hWnd, UINT nFlags, POINT ptPos)
{
	OnMouseMove(hWnd, nFlags, ptPos);
}

void TLineTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	DPOINT ptDPos = Config->ScreenToReal(ptPos);
	if (m_uiHit>0)
	{
		MoveLine->ptEnd = ptDPos;
	}

	//���X�Ḩ����
	if (abs(Config->LengthToScreenY(ptDPos.y - MoveLine->ptBegin.y)) < 10)//��ǰ�����ʼ��y��ƫ��10����
	{
		bShowXAssist = true;

		//���ø�����x,y����
		if (ptDPos.x - XAssistLine->ptBegin.x > 0)//��ǰλ��λ�ڸ�������ʼ���ұ�
		{
			XAssistLine->ptEnd.x = Config->ScreenToReal({ win.Canvas.ClientRect.right, 0 }).x;
		}
		else
		{
			XAssistLine->ptEnd.x = Config->ScreenToReal({ win.Canvas.ClientRect.left, 0 }).x;
		}
		XAssistLine->ptEnd.y = XAssistLine->ptBegin.y;

		MoveLine->ptEnd.y = XAssistLine->ptEnd.y;//����ǰ��������������
	}
	else
	{
		bShowXAssist = false;
	}

	//���Y�Ḩ����
	if (abs(Config->LengthToScreenX(ptDPos.x - MoveLine->ptBegin.x)) < 10)
	{
		bShowYAssist = true;
		if (ptDPos.y - YAssistLine->ptBegin.y>0)
			YAssistLine->ptEnd.y = Config->ScreenToReal({ 0, win.Canvas.ClientRect.top }).y;
		else
			YAssistLine->ptEnd.y = Config->ScreenToReal({ 0, win.Canvas.ClientRect.bottom }).y;
		YAssistLine->ptEnd.x = YAssistLine->ptBegin.x;
		MoveLine->ptEnd.x = YAssistLine->ptEnd.x;//����
	}
	else
	{
		bShowYAssist = false;
	}

	if (m_uiHit > 0)
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
			LineEdit->ShowWindow(true);
		}
		else
			LineEdit->ShowWindow(false);
	}

	//��Canvasˢ��
}

//����WM_PAINT�¼��н��л���
void TLineTool::Draw(HDC hdc)
{
	if (bShowXAssist)
	{
		TDraw::DrawRealLine(hdc, *XAssistLine,Config);//
		TDraw::DrawCross(hdc, Config->RealToScreen(MoveLine->ptEnd), 18, *MoveLine);
	}
	if (bShowYAssist)
	{
		TDraw::DrawRealLine(hdc, *YAssistLine,Config);//
		TDraw::DrawCross(hdc, Config->RealToScreen(MoveLine->ptEnd), 18, *MoveLine);
	}
	if (m_uiHit > 0)
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

void TLineTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	if (bShowXAssist)
		ptPos = Config->RealToScreen(MoveLine->ptEnd);//��������������������
	if (bShowYAssist)
		ptPos = Config->RealToScreen(MoveLine->ptEnd);//��������������������

	if (m_uiHit > 0)
	{
		win.m_Shape.AddLine(m_pptHit[m_uiHit - 1], Config->ScreenToReal(ptPos),
			Config->iStyle, Config->iWidth, Config->crPen);//¼������
	}

	//��ǰ������ݴ�㼯
	m_uiHit++;
	m_pptHit = (DPOINT *)realloc(m_pptHit, m_uiHit*sizeof(DPOINT));
	if (m_pptHit == NULL)
	{
		assert(m_pptHit);
		return;
	}
	m_pptHit[m_uiHit - 1] = Config->ScreenToReal(ptPos);//��ǰ�����㼯

	//������ʱ��
	MoveLine->ptBegin = Config->ScreenToReal(ptPos);
	MoveLine->ptEnd = Config->ScreenToReal(ptPos);

	XAssistLine->ptBegin = Config->ScreenToReal(ptPos);
	XAssistLine->ptEnd = Config->ScreenToReal(ptPos);
	YAssistLine->ptBegin = Config->ScreenToReal(ptPos);
	YAssistLine->ptEnd = Config->ScreenToReal(ptPos);

	Line1->ptBegin = ptPos;
	Line1->ptEnd = ptPos;
	Line2->ptBegin = ptPos;
	Line2->ptEnd = ptPos;
	LineDim->ptBegin = ptPos;
	LineDim->ptEnd = ptPos;
	bShowDimLine = true;

	if (LineEdit->m_hWnd == NULL)
	{
		LineEdit->CreateEditEx(win.Canvas.m_hWnd, IDR_LINEEDIT, win.Canvas.m_hInst);
	}
	else
		LineEdit->ShowWindow(false);//�����ڻ����ߺ���ʾEdit

	//��������ˢ��
}

void TLineTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	//���Ѵ���Edit������Edit
	if (LineEdit->m_hWnd != NULL)
		LineEdit->ShowWindow(false);

	free(m_pptHit);
	m_pptHit = NULL;

	bShowXAssist = false;
	bShowYAssist = false;
	bShowDimLine = false;

	::InvalidateRect(win.Canvas.m_hWnd, &win.Canvas.ClientRect, false);

	//û�л��ߵ�����µ��Ҽ������ù���
	if (m_uiHit==0)
		::PostMessage(win.m_hWnd, WM_COMMAND, ID_SELECT, 0);
	m_uiHit = 0;
}

void TLineTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_CROSS));
}