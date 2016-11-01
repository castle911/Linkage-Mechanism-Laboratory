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

	//检测X轴辅助线
	if (abs(Config->LengthToScreenY(ptDPos.y - MoveLine->ptBegin.y)) < 10)//当前点和起始点y轴偏差10像素
	{
		bShowXAssist = true;

		//设置辅助线x,y坐标
		if (ptDPos.x - XAssistLine->ptBegin.x > 0)//当前位置位于辅助线起始点右边
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

		MoveLine->ptEnd.y = XAssistLine->ptEnd.y;//将当前点吸附到坐标轴
	}
	else
	{
		bShowXAssist = false;
		iIvoryLine = 0;
	}

	//检测Y轴辅助线
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
		MoveLine->ptEnd.x = YAssistLine->ptEnd.x;//吸附
	}
	else
	{
		bShowYAssist = false;
	}

	if (m_uiHit > 0)
	{
		//计算暂存线长度并存入Edit
		MoveLine->CalcLength();
		LineEdit->SetText(TEXT("%.3f"), MoveLine->dLength);
		SetFocus(LineEdit->m_hWnd);
		LineEdit->SetSelectAll();

		//计算尺寸线各点坐标
		int dist = 40;//暂存线和尺寸线距离
		double theta = TDraw::GetAngleFromPointReal(MoveLine->ptBegin, MoveLine->ptEnd);
		pt1 = Config->RealToScreen(MoveLine->ptBegin);
		pt2 = Config->RealToScreen(MoveLine->ptEnd);
		Line1->ptBegin = pt1;
		Line2->ptBegin = pt2;
		if (MoveLine->ptEnd.y - MoveLine->ptBegin.y >= 0)//位于1,2象限
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
		//若尺寸线长度为0则不显示
		if (LineDim->ptBegin.x == LineDim->ptEnd.x && LineDim->ptBegin.y == LineDim->ptEnd.y)
			bShowDimLine = false;
		else
			bShowDimLine = true;

		//设置Edit位置并显示
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

	//由Canvas刷新
}

//插入WM_PAINT事件中进行绘制
void TLineTool::Draw(HDC hdc)
{
	//画X辅助线
	if (bShowXAssist)
	{
		TDraw::DrawRealLine(hdc, *XAssistLine,Config);//
		TDraw::DrawCross(hdc, Config->RealToScreen(MoveLine->ptEnd), 18, MoveLine->logpenStyle);
	}

	//画Y辅助线
	if (bShowYAssist)
	{
		TDraw::DrawRealLine(hdc, *YAssistLine,Config);//
		TDraw::DrawCross(hdc, Config->RealToScreen(MoveLine->ptEnd), 18, MoveLine->logpenStyle);
	}

	Attach.Draw(hdc);

	//画临时线及尺寸线
	if (m_uiHit > 0)
	{
		TDraw::DrawRealLine(hdc, *MoveLine,Config);//点过一次后才开始画临时线

		if (bShowDimLine)
		{
			TDraw::DrawLine(hdc, *Line1);
			TDraw::DrawLine(hdc, *Line2);
			TDraw::DrawLine(hdc, *LineDim);
		}

	}
}

//LineEdit出现时Canvas是接收不到KEYDOWN的，自然LineTool也接收不到。
//KEYDOWN消息由LineEdit截获发送给Canvas，再传递下来。
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
			//得到字符并转换
			double num;
			TCHAR buffer[64];
			LineEdit->GetText(buffer);
			num = TTransfer::TCHAR2double(buffer);

			TRealLine RealLine;
			RealLine.ptBegin = m_pptHit[m_uiHit - 1];

			//计算得到终点坐标
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

			//入库
			win.m_Shape.AddRealLine(RealLine);

			//计算得出的终点存入暂存点集
			m_uiHit++;
			m_pptHit = (DPOINT *)realloc(m_pptHit, m_uiHit*sizeof(DPOINT));
			m_pptHit[m_uiHit - 1] = RealLine.ptEnd;

			//设置临时线
			POINT ptNew = Config->RealToScreen(RealLine.ptEnd);
			InitialLine(ptNew);

			LineEdit->SetVisible(false);//避免在画完线后还显示Edit

			TDraw::ClientPosToScreen(hWnd, &ptNew);
			SetCursorPos(ptNew.x, ptNew.y);
		}
		break;
	}
}
void TLineTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	if (bShowXAssist)
		ptPos = Config->RealToScreen(MoveLine->ptEnd);//若吸附点出现则进行吸附
	if (bShowYAssist)
		ptPos = Config->RealToScreen(MoveLine->ptEnd);//若吸附点出现则进行吸附

	//上一点及当前点存入数据
	if (m_uiHit > 0)
	{
		TRealLine RealLine;
		RealLine.SetStyle(Config->iStyle, Config->iWidth, Config->crPen);
		RealLine.SetPoint(m_pptHit[m_uiHit - 1], Config->ScreenToReal(ptPos));
		win.m_Shape.AddRealLine(RealLine);
	}

	//当前点存入暂存点集
	m_uiHit++;
	m_pptHit = (DPOINT *)realloc(m_pptHit, m_uiHit*sizeof(DPOINT));
	m_pptHit[m_uiHit - 1] = Config->ScreenToReal(ptPos);

	//设置临时线
	InitialLine(ptPos);

	//创建LineEdit或隐藏
	if (LineEdit->m_hWnd == NULL)
	{
		LineEdit->CreateEditEx(win.Canvas.m_hWnd, IDR_LineEdit, win.Canvas.m_hInst);
		LineEdit->Load(hWnd);
	}
	else
		LineEdit->SetVisible(false);//避免在画完线后还显示Edit

	//交给画布刷新
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
	//若已创建Edit则隐藏Edit
	if (LineEdit->m_hWnd != NULL)
		LineEdit->SetVisible(false);

	free(m_pptHit);
	m_pptHit = NULL;

	bShowXAssist = false;
	bShowYAssist = false;
	bShowDimLine = false;

	::InvalidateRect(win.Canvas.m_hWnd, &win.Canvas.ClientRect, false);

	//没有画线的情况下点右键则重置工具
	if (m_uiHit==0)
		::PostMessage(win.m_hWnd, WM_COMMAND, ID_SELECT, 0);
	m_uiHit = 0;
}

void TLineTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_CROSS));
}