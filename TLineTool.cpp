#pragma once
#include "TLineTool.h"

#include "TCanvas.h"
#include "resource.h"
#include "TTool.h"
#include "TLine.h"
#include "TTreeViewContent.h"

#include "TConfiguration.h"
#include "TDraw.h"
#include "TLineEdit.h"
#include "TAttach.h"
#include "TConstraintCoincide.h"
#include "TTransfer.h"
#include "TSolver.h"

TLineTool::TLineTool()
{
	Attach = new TAttach(pCanvas->m_hWnd, pShape, pConfig);
	bShowDimLine = false;
	MoveLine = new TRealLine;
	MoveLine->SetStyle(PS_SOLID, 1, pConfig->crPen);

	Line1 = new TLine;
	Line2 = new TLine;
	LineDim = new TLine;
	Line1->SetStyle(PS_DOT, 1, pConfig->crPen);
	Line2->SetStyle(PS_DOT, 1, pConfig->crPen);
	LineDim->SetStyle(PS_DOT, 1, pConfig->crPen);

	LineEdit = new TLineEdit;

	CoincideBegin = NULL;

	myElementType = ELEMENT_REALLINE;

	iPrevLineId = -1;
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

	if (CoincideBegin != NULL)
		delete CoincideBegin;
}

void TLineTool::OnMouseWheel(HWND hWnd, UINT nFlags, POINT ptPos)
{
	OnMouseMove(hWnd, nFlags, ptPos);
}

void TLineTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	Attach->AttachAll(ptPos, MoveLine->ptBegin);
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
		pt1 = pConfig->RealToScreen(MoveLine->ptBegin);
		pt2 = pConfig->RealToScreen(MoveLine->ptEnd);
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
		TDraw::DrawRealLine(hdc, *MoveLine, pConfig);//���һ�κ�ſ�ʼ����ʱ��

		if (bShowDimLine)
		{
			TDraw::DrawLine(hdc, *Line1);
			TDraw::DrawLine(hdc, *Line2);
			TDraw::DrawLine(hdc, *LineDim);
		}

	}
}



void TLineTool::AddIntoShape(TRealLine &RealLine)
{
	pShape->AddRealLine(RealLine);
}

void TLineTool::AddIntoTreeViewContent(TElement *Element, int id)
{
	if (Element->eType == ELEMENT_REALLINE)
		Element->eType = myElementType;
	pTreeViewContent->AddItem(Element, pShape->iNextId);
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

			if (abs(num) > 1e-6)
			{
				TRealLine RealLine;
				RealLine.ptBegin = dptHit[dptHit.size() - 1];

				//����õ��յ�����
				switch (Attach->GetiIvoryLine())
				{
				case 0:
					double angle;
					angle = TDraw::GetAngleFromPointReal(RealLine.ptBegin, MoveLine->ptEnd);
					RealLine.SetPoint(RealLine.ptBegin, num, angle);
					break;
				case 1://��
					RealLine.ptEnd = { RealLine.ptBegin.x + num, RealLine.ptBegin.y };
					RealLine.dLength = num;
					RealLine.dAngle = 0;
					break;
				case 3://��
					RealLine.ptEnd = { RealLine.ptBegin.x - num, RealLine.ptBegin.y };
					RealLine.dLength = num;
					RealLine.dAngle = M_PI;
					break;
				case 2://��
					RealLine.ptEnd = { RealLine.ptBegin.x, RealLine.ptBegin.y + num };
					RealLine.dLength = num;
					RealLine.dAngle = M_PI/2;
					break;
				case 4://��
					RealLine.ptEnd = { RealLine.ptBegin.x, RealLine.ptBegin.y - num };
					RealLine.dLength = num;
					RealLine.dAngle = 1.5*M_PI;
					break;
				}
				RealLine.SetStyle(pConfig->iStyle, pConfig->iWidth, pConfig->crPen);

				//���
				iPrevLineId = pShape->iNextId;
				AddIntoTreeViewContent(&RealLine, pShape->iNextId);
				AddIntoShape(RealLine);

				if (CoincideBegin != NULL)
				{
					//��һ��Լ�����
					CoincideBegin->ElementId2 = iPrevLineId;
					AddIntoTreeViewContent(CoincideBegin, pShape->iNextId);
					pShape->AddCoincide(*CoincideBegin);
					pSolver->RefreshEquations(true);

					delete CoincideBegin;
					CoincideBegin = NULL;
				}

				//��������Լ����ID��һ��.end=ID���.begin
				CoincideBegin = new TConstraintCoincide;
				CoincideBegin->eElementType1 = myElementType;
				CoincideBegin->ElementId1 = iPrevLineId;//��һ����id
				CoincideBegin->Element1PointIndex = 2;//ptEnd

				CoincideBegin->eElementType2 = myElementType;
				//CoincideBegin->ElementId2 = pShape->iNextId;//�˴�δ��⣬��Ϊ��ǰ���
				CoincideBegin->Element2PointIndex = 1;//ptBegin

				//����ó����յ�����ݴ�㼯
				dptHit.push_back(RealLine.ptEnd);

				//������ʱ��
				POINT ptNew = pConfig->RealToScreen(RealLine.ptEnd);
				InitialLine(RealLine.ptEnd);
				Attach->InitialLine(ptNew);

				LineEdit->SetVisible(false);//�����ڻ����ߺ���ʾEdit

				//�ƶ���������
				TDraw::ClientPosToScreen(hWnd, &ptNew);
				SetCursorPos(ptNew.x, ptNew.y);
			}
		}
		break;
	}
}
void TLineTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	ptPos = pConfig->RealToScreen(MoveLine->ptEnd);

	//����Լ��
	if (dptHit.size() == 0)//��һ�㲶׽����Լ��
	{
		if (Attach->iAttachElementId != -1)
		{
			//ID����׽=ID��Ԫ��.begin
			CoincideBegin = new TConstraintCoincide;
			CoincideBegin->eElementType1 = Attach->eAttachElementType;
			CoincideBegin->ElementId1 = Attach->iAttachElementId;
			CoincideBegin->Element1PointIndex = Attach->iAttachElementPointIndex;

			CoincideBegin->eElementType2 = myElementType;
			//CoincideBegin->ElementId2 = pShape->iNextId;//�˴�δ��⣬��Ϊ��һ��Element��id
			CoincideBegin->Element2PointIndex = 1;//ptBegin
		}
	}
	else//�ǵ�һ��
	{
		if (ptPos.x == ptPrevPos.x && ptPos.y == ptPrevPos.y)
			return;

		//������1���������
		TRealLine RealLine;
		RealLine.SetStyle(pConfig->iStyle, pConfig->iWidth, pConfig->crPen);
		RealLine.SetPoint(dptHit[dptHit.size() - 1], MoveLine->ptEnd);

		iPrevLineId = pShape->iNextId;
		AddIntoTreeViewContent(&RealLine, pShape->iNextId);
		AddIntoShape(RealLine);

		if (CoincideBegin != NULL)
		{
			//��һ��Լ�����
			CoincideBegin->ElementId2 = iPrevLineId;
			AddIntoTreeViewContent(CoincideBegin, pShape->iNextId);
			pShape->AddCoincide(*CoincideBegin);
			pSolver->RefreshEquations(true);
			//RefreshTreeViewContent();
			delete CoincideBegin;
			CoincideBegin = NULL;
		}

		if (Attach->iAttachElementId != -1)//���յ㲶׽����
		{
			//ID����׽=ID���.end
			CoincideBegin = new TConstraintCoincide;
			CoincideBegin->eElementType1 = Attach->eAttachElementType;
			CoincideBegin->ElementId1 = Attach->iAttachElementId;
			CoincideBegin->Element1PointIndex = Attach->iAttachElementPointIndex;

			CoincideBegin->eElementType2 = myElementType;
			CoincideBegin->ElementId2 = iPrevLineId;//�˴�����⣬�����Ԫ��idΪnextid-1
			CoincideBegin->Element2PointIndex = 2;//ptEnd

			//�յ��������
			AddIntoTreeViewContent(CoincideBegin, pShape->iNextId);
			pShape->AddCoincide(*CoincideBegin);
			pSolver->RefreshEquations(true);

			delete CoincideBegin;
			CoincideBegin = NULL;
		}


			//��������Լ����ID��һ��.end=ID���.begin
			CoincideBegin = new TConstraintCoincide;
			CoincideBegin->eElementType1 = myElementType;
			CoincideBegin->ElementId1 = iPrevLineId;//��һ����id
			CoincideBegin->Element1PointIndex = 2;//ptEnd

			CoincideBegin->eElementType2 = myElementType;
			//CoincideBegin->ElementId2 = pShape->iNextId;//�˴�δ��⣬��Ϊ��ǰ���
			CoincideBegin->Element2PointIndex = 1;//ptBegin
	}

	//��ǰ������ݴ�㼯
	dptHit.push_back(MoveLine->ptEnd);

	//������ʱ��
	InitialLine(MoveLine->ptEnd);
	Attach->InitialLine(ptPos);

	//����LineEdit������
	if (LineEdit->m_hWnd == NULL)
	{
		LineEdit->CreateEditEx(pCanvas->m_hWnd, IDR_LineEdit, pCanvas->m_hInst);
	}
	else
		LineEdit->SetVisible(false);//�����ڻ����ߺ���ʾEdit

	ptPrevPos = ptPos;
	//��������ˢ��
}

void TLineTool::InitialLine(DPOINT dptPos)
{
	POINT ptPos = pConfig->RealToScreen(dptPos);
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
	if (dptHit.size() == 0)//û�е������µ��Ҽ������ù���
		::PostMessage(hwndWin, WM_COMMAND, ID_SELECT, 0);
	else
	{
		//���Ѵ���Edit������Edit
		if (LineEdit->m_hWnd != NULL)
			LineEdit->SetVisible(false);

		bShowDimLine = false;

		//�����ݴ��غ�Լ��
		if (CoincideBegin != NULL)
		{
			delete CoincideBegin;
			CoincideBegin = NULL;
		}

		dptHit.swap(std::vector<DPOINT>());

		::InvalidateRect(pCanvas->m_hWnd, &(pCanvas->ClientRect), false);

	}
}

void TLineTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_CROSS));
}