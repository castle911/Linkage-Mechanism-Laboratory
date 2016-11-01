#pragma once
#include "TLineTool.h"

extern TMainWindow win;
TLineTool::TLineTool() 
{
	Attach.Load(win.Canvas.m_hWnd, &(win.m_Shape), &(win.m_Configuration));
	Config = &(win.m_Configuration);
	m_uiHit = 0;
	m_pptHit = NULL;
	bShowXAssist = false;
	bShowYAssist = false;
	bShowDimLine = false;
	MoveLine = new TRealLine;
	XAssistLine = new TRealLine;
	YAssistLine = new TRealLine;
	MoveLine->SetStyle(PS_SOLID, 1, Config->crPen);
	XAssistLine->SetStyle(PS_DOT, 1, Config->crDot);
	YAssistLine->SetStyle(PS_DOT, 1, Config->crDot);

	Line1 = new TLine;
	Line2 = new TLine; 
	LineDim = new TLine;
	Line1->SetStyle(PS_DOT, 1, Config->crPen);
	Line2->SetStyle(PS_DOT, 1, Config->crPen);
	LineDim->SetStyle(PS_DOT, 1, Config->crPen);

	iIvoryLine = 0;

	LineEdit = new TLineEdit;
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

	Attach.AttachAll(ptPos);

	//���X�Ḩ����
	if (abs(Config->LengthToScreenY(ptDPos.y - MoveLine->ptBegin.y)) < 10)//��ǰ�����ʼ��y��ƫ��10����
	{
		bShowXAssist = true;

		//���ø�����x,y����
		if (ptDPos.x - XAssistLine->ptBegin.x > 0)//��ǰλ��λ�ڸ�������ʼ���ұ�
		{
			XAssistLine->ptEnd.x = Config->ScreenToReal({ win.Canvas.ClientRect.right, 0 }).x;
			iIvoryLine = 1;
		}
		else
		{
			XAssistLine->ptEnd.x = Config->ScreenToReal({ win.Canvas.ClientRect.left, 0 }).x;
			iIvoryLine = 3;
		}
		XAssistLine->ptEnd.y = XAssistLine->ptBegin.y;

		MoveLine->ptEnd.y = XAssistLine->ptEnd.y;//����ǰ��������������
	}
	else
	{
		bShowXAssist = false;
		iIvoryLine = 0;
	}

	//���Y�Ḩ����
	if (abs(Config->LengthToScreenX(ptDPos.x - MoveLine->ptBegin.x)) < 10)
	{
		bShowYAssist = true;
		if (ptDPos.y - YAssistLine->ptBegin.y > 0)
		{
			YAssistLine->ptEnd.y = Config->ScreenToReal({ 0, win.Canvas.ClientRect.top }).y;
			iIvoryLine = 2;
		}
		else
		{
			YAssistLine->ptEnd.y = Config->ScreenToReal({ 0, win.Canvas.ClientRect.bottom }).y;
			iIvoryLine = 4;
		}
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
			LineEdit->SetVisible(true);
		}
		else
			LineEdit->SetVisible(false);
	}

	//��Canvasˢ��
}

//����WM_PAINT�¼��н��л���
void TLineTool::Draw(HDC hdc)
{
	//��X������
	if (bShowXAssist)
	{
		TDraw::DrawRealLine(hdc, *XAssistLine,Config);//
		TDraw::DrawCross(hdc, Config->RealToScreen(MoveLine->ptEnd), 18, MoveLine->logpenStyle);
	}

	//��Y������
	if (bShowYAssist)
	{
		TDraw::DrawRealLine(hdc, *YAssistLine,Config);//
		TDraw::DrawCross(hdc, Config->RealToScreen(MoveLine->ptEnd), 18, MoveLine->logpenStyle);
	}

	Attach.Draw(hdc);

	//����ʱ�߼��ߴ���
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
			RealLine.ptBegin = m_pptHit[m_uiHit - 1];

			//����õ��յ�����
			double angle;
			switch (iIvoryLine)
			{
			case 0:
				angle=TDraw::GetAngleFromPointReal(RealLine.ptBegin, MoveLine->ptEnd);
				RealLine.SetPoint(RealLine.ptBegin, num, angle);
				break;
			case 1:
				RealLine.ptEnd = { RealLine.ptBegin.x + num, RealLine.ptBegin.y };
				break;
			case 3:
				RealLine.ptEnd = { RealLine.ptBegin.x - num, RealLine.ptBegin.y };
				break;
			case 2:
				RealLine.ptEnd = { RealLine.ptBegin.x, RealLine.ptBegin.y+num };
				break;
			case 4:
				RealLine.ptEnd = { RealLine.ptBegin.x, RealLine.ptBegin.y - num };
				break;
			}
			RealLine.SetStyle(Config->iStyle, Config->iWidth, Config->crPen);

			//���
			win.m_Shape.AddRealLine(RealLine);

			//����ó����յ�����ݴ�㼯
			m_uiHit++;
			m_pptHit = (DPOINT *)realloc(m_pptHit, m_uiHit*sizeof(DPOINT));
			m_pptHit[m_uiHit - 1] = RealLine.ptEnd;

			//������ʱ��
			POINT ptNew = Config->RealToScreen(RealLine.ptEnd);
			InitialLine(ptNew);

			LineEdit->SetVisible(false);//�����ڻ����ߺ���ʾEdit

			TDraw::ClientPosToScreen(hWnd, &ptNew);
			SetCursorPos(ptNew.x, ptNew.y);
		}
		break;
	}
}
void TLineTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	if (bShowXAssist)
		ptPos = Config->RealToScreen(MoveLine->ptEnd);//��������������������
	if (bShowYAssist)
		ptPos = Config->RealToScreen(MoveLine->ptEnd);//��������������������

	//��һ�㼰��ǰ���������
	if (m_uiHit > 0)
	{
		TRealLine RealLine;
		RealLine.SetStyle(Config->iStyle, Config->iWidth, Config->crPen);
		RealLine.SetPoint(m_pptHit[m_uiHit - 1], Config->ScreenToReal(ptPos));
		win.m_Shape.AddRealLine(RealLine);
	}

	//��ǰ������ݴ�㼯
	m_uiHit++;
	m_pptHit = (DPOINT *)realloc(m_pptHit, m_uiHit*sizeof(DPOINT));
	m_pptHit[m_uiHit - 1] = Config->ScreenToReal(ptPos);

	//������ʱ��
	InitialLine(ptPos);

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

void TLineTool::InitialLine(POINT ptPos)
{
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
}

void TLineTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	//���Ѵ���Edit������Edit
	if (LineEdit->m_hWnd != NULL)
		LineEdit->SetVisible(false);

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