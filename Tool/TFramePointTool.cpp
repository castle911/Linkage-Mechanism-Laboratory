#pragma once
#include "..\resource.h"
#include "TFramePointTool.h"
#include "..\Control\TCanvas.h"

#include "..\Control\TTreeViewContent.h"
#include "..\Element\TConstraintCoincide.h"

TFramePointTool::TFramePointTool()
{
	Attach = new TAttach(pCanvas, pShape, pConfig);

	tempFramePoint.SetStyle(pConfig);

	pPrevFramePoint = NULL;

	bShowTips = true;
}


TFramePointTool::~TFramePointTool()
{
	delete Attach;
}


void TFramePointTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	ptMouse = ptPos;

	Attach->AttachAll(ptPos);
	tempFramePoint.dpt = Attach->dptAttach;

	sTips = TEXT("����Խ������ܵ�");
	if (Attach->bAttachedEndpoint)
	{
		sTips += TEXT("\r\n�������˵㣺�����غ�Լ��");
	}
}


void TFramePointTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	tempFramePoint.dpt = Attach->dptAttach;

	tempFramePoint.vecDpt.clear();
	tempFramePoint.vecDpt.push_back({ 0, 0 });

	pTreeViewContent->AddItem(&tempFramePoint, pShape->iNextId);
	pPrevFramePoint = pShape->AddElement(&tempFramePoint);

	sTips = TEXT("�ѽ�����");

	if (Attach->pAttachElement!=NULL)
	switch (Attach->pAttachElement->eType)
	{
	case ELEMENT_FRAMEPOINT:
	case ELEMENT_SLIDEWAY:
		return;
	}
	if (Attach->bAttachedEndpoint)
	{
		TConstraintCoincide coincide;
		coincide.SetStyle(pConfig->logpen);

		coincide.pElement[0] = Attach->pAttachElement;
		coincide.PointIndexOfElement[0] = Attach->iAttachElementPointIndex;

		coincide.pElement[1] = pPrevFramePoint;
		coincide.PointIndexOfElement[1] = 0;

		//Լ�����
		pTreeViewContent->AddItem(&coincide, pShape->iNextId);
		pShape->AddElement(&coincide);

		RefreshEquations();
	}

}

void TFramePointTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::PostMessage(hwndWin, WM_COMMAND, ID_SELECT, 0);
	pCanvas->Invalidate();
}

//����WM_PAINT�¼��н��л���
void TFramePointTool::Draw(HDC hdc)
{
	if (bShowTips)
		TDraw::DrawTips(hdc, ptMouse, ClientRect, sTips.c_str(), pConfig);

	TDraw::DrawFramePoint(hdc, &tempFramePoint, pConfig);
	//�Ѵ洢����Canvas�������
	Attach->Draw(hdc);
}

void TFramePointTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_CROSS));
}