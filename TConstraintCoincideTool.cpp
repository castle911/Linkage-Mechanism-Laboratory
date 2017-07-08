#include "TConstraintCoincideTool.h"

#include "TAttach.h"

#include "TConstraintCoincide.h"

TConstraintCoincideTool::TConstraintCoincideTool()
{
	pCoincide = NULL;

	pAttach = NULL;

	Reset();
}

void TConstraintCoincideTool::Reset()
{
	if (pCoincide != NULL) delete pCoincide;
	pCoincide = NULL;

	if (pAttach != NULL) delete pAttach;
	pAttach = new TAttach(pCanvas, pShape, pConfig);

	bShowTips = true;

	dptHit.clear();
}

TConstraintCoincideTool::~TConstraintCoincideTool()
{
	if (pCoincide != NULL) delete pCoincide;

	if (pAttach != NULL) delete pAttach;
}

void TConstraintCoincideTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_CROSS));
}

void TConstraintCoincideTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_ESCAPE:
		if (dptHit.empty())
			ResetTool();
		else
			Reset();
		break;
	case VK_SPACE:
	case VK_RETURN:
		break;
	}
}

void TConstraintCoincideTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	ptMouse = ptPos;
	pAttach->AttachAll(ptPos);

	switch (dptHit.size())
	{
	case 0:
		if (pAttach->bAttachedEndpoint)//��ʰȡ
		{
			sTips = TEXT("�������˵㣺�����ȷ����һ��");
		}
		else
		sTips = TEXT("��ѡ��Ҫ����Լ���ĵ�һ��");

		break;
	case 1:
		if (pAttach->bAttachedEndpoint)//��ʰȡ
		{
			sTips = TEXT("�������˵㣺�����ȷ���ڶ���");
		}
		else
			sTips = TEXT("��ѡ��Ҫ����Լ���ĵڶ���");

		if (pAttach->pAttachElement == pCoincide->pElement[0])//ʰȡ����
			sTips = TEXT("������ͬһ�������Ͻ���Լ��");

		sTips += TEXT("\r\n�Ҽ�/Esc ��ȡ��");
		break;
	}

}

void TConstraintCoincideTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	if (pAttach->bAttachedEndpoint)//��ʰȡ
	{
		dptHit.push_back(ptPos);
		switch (dptHit.size())
		{
		case 1://���µ�һ��
			pCoincide = new TConstraintCoincide;
			pCoincide->SetStyle(pConfig->logpen);

			pCoincide->pElement[0] = pAttach->pAttachElement;
			pCoincide->PointIndexOfElement[0] = pAttach->iAttachElementPointIndex;

			pCoincide->BuildpDpt_inner(0);

			break;
		case 2:
			if (pAttach->pAttachElement == pCoincide->pElement[0])//ʰȡ����
			{
				sTips = TEXT("������ͬһ�������Ͻ���Լ��");
				break;
			}

			pCoincide->pElement[1] = pAttach->pAttachElement;
			pCoincide->PointIndexOfElement[1] = pAttach->iAttachElementPointIndex;

			AddIntoShape();
			Reset();
			sTips = TEXT("�������");
			break;
		}
	}
	else//��Ч���
		sTips = TEXT("ѡ����Ч�������Ѵ��ڹ����ϵĵ�");
}

void TConstraintCoincideTool::AddIntoShape()
{
	//���
	AddTreeViewItem(pCoincide, pShape->iNextId);
	pShape->AddElement(pCoincide);

	//ˢ�·�����
	RefreshEquations();
}

void TConstraintCoincideTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	switch (dptHit.size())
	{
	case 0:
		ResetTool();
		break;
	default:
	Reset();
	RefreshCanvas();
		break;
	}
}

void TConstraintCoincideTool::Draw(HDC hdc)
{
	if (pCoincide != NULL)
		TDraw::DrawConstraintCoincide(hdc, pCoincide->GetLinkDpt(0), pAttach->dptAttach, pCoincide->logpenStyleShow, pConfig);

	pAttach->Draw(hdc);

	if (bShowTips)
		TDraw::DrawTips(hdc, ptMouse, ClientRect, sTips.c_str(), pConfig);
}
