#pragma once
#include "Resource.h"
#include "TFramePointTool.h"
#include "TCanvas.h"

#include "TTreeViewContent.h"
#include "TConstraintCoincide.h"
#include "TSolver.h"

TFramePointTool::TFramePointTool()
{
	Attach = new TAttach(pCanvas->m_hWnd, pShape, pConfig);

	tempFramePoint.SetStyle(PS_SOLID, 1, pConfig->crPen);

	pPrevFramePoint = NULL;
}


TFramePointTool::~TFramePointTool()
{
}


void TFramePointTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	Attach->AttachAll(ptPos);
	tempFramePoint.dpt = Attach->dptAttach;
}


void TFramePointTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	tempFramePoint.dpt = Attach->dptAttach;

	pTreeViewContent->AddItem(&tempFramePoint, pShape->iNextId);
	pPrevFramePoint=pShape->AddFramePoint(tempFramePoint);

	if (Attach->pAttachElement != NULL)
	{
		TConstraintCoincide coincide;
		coincide.SetStyle(pConfig->iStyle, pConfig->iWidth, pConfig->crPen);
		coincide.pElement1 = Attach->pAttachElement;
		coincide.Element1PointIndex = Attach->iAttachElementPointIndex;

		coincide.pElement2 = pPrevFramePoint;
		coincide.Element2PointIndex = 0;

		pTreeViewContent->AddItem(&coincide, pShape->iNextId);

		pShape->AddCoincide(coincide, pConfig);//Լ�����

		pSolver->RefreshEquations(true);
	}

}

void TFramePointTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::PostMessage(hwndWin, WM_COMMAND, ID_SELECT, 0);
}

//����WM_PAINT�¼��н��л���
void TFramePointTool::Draw(HDC hdc)
{
	Attach->Draw(hdc);

	//�Ѵ洢����Canvas�������
}

void TFramePointTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_CROSS));
}