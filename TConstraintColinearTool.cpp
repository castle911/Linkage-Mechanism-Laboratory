#include "TConstraintColinearTool.h"

#include "TAttach.h"
#include "TConstraintColinear.h"

TConstraintColinearTool::TConstraintColinearTool()
{
	pColinear = NULL;

	pAttach = NULL;

	Reset();
}

void TConstraintColinearTool::Reset()
{
	if (pColinear != NULL) delete pColinear;
	pColinear = NULL;

	if (pAttach != NULL) delete pAttach;
	pAttach = new TAttach(pCanvas, pShape, pConfig);

	bShowTips = true;

	dptHit.clear();
}

TConstraintColinearTool::~TConstraintColinearTool()
{
	if (pColinear != NULL) delete pColinear;

	if (pAttach != NULL) delete pAttach;
}

void TConstraintColinearTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_CROSS));
}

void TConstraintColinearTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

void TConstraintColinearTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	ptMouse = ptPos;
	pAttach->AttachLine(ptPos);

	switch (dptHit.size())
	{
	case 0:
		if (pAttach->bShowExtensionLine)//��ʰȡ
		{
			sTips = TEXT("�������������ȷ����һ�����ӹ���");
		}
		else
			sTips = TEXT("��ѡ��Ҫ����Լ���ĵ�һ�����ӹ���");

		break;
	case 1:
		if (pAttach->bShowExtensionLine)//��ʰȡ
		{
		if (pAttach->pAttachElement == pColinear->pElement[0])//ʰȡ����
			sTips = TEXT("������ͬһ�������Ͻ���Լ��");
		else
			sTips = TEXT("�������������ȷ���ڶ������ӹ���");
		}
		else
			sTips = TEXT("��ѡ��Ҫ����Լ���ĵڶ������ӹ���");


		sTips += TEXT("\r\n�Ҽ�/Esc ��ȡ��");
		break;
	}

}


void TConstraintColinearTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	if (pAttach->bShowExtensionLine)//��ʰȡ
	{
		dptHit.push_back(ptPos);
		switch (dptHit.size())
		{
		case 1://���µ�һ��
			pColinear = new TConstraintColinear;
			pColinear->SetStyle(pConfig->logpen);

			pColinear->pElement[0] = pAttach->pAttachElement;
			pColinear->PointBeginIndexOfElement[0] = pAttach->iAttachLinePointIndex[0];
			pColinear->PointEndIndexOfElement[0] = pAttach->iAttachLinePointIndex[1];


			break;
		case 2:
			if (pAttach->pAttachElement == pColinear->pElement[0])//ʰȡ����
			{
				sTips = TEXT("������ͬһ�������Ͻ���Լ��");
				break;
			}

			pColinear->pElement[1] = pAttach->pAttachElement;
			pColinear->PointBeginIndexOfElement[1] = pAttach->iAttachLinePointIndex[0];
			pColinear->PointEndIndexOfElement[1] = pAttach->iAttachLinePointIndex[1];

			AddIntoShape();
			Reset();
			sTips = TEXT("�������");
			break;
		}
	}
	else//��Ч���
		sTips = TEXT("ѡ����Ч�������Ѵ��ڹ����ϵĵ�");
}

void TConstraintColinearTool::AddIntoShape()
{
	//���
	AddTreeViewItem(pColinear, pShape->iNextId);
	pShape->AddElement(pColinear);

	//ˢ�·�����
	RefreshEquations();
}

void TConstraintColinearTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
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

void TConstraintColinearTool::Draw(HDC hdc)
{

	if (pColinear != NULL)
	{
	POINT pt1 = pConfig->RealToScreen(TDraw::GetCenter(pColinear->pElement[0]->GetAbsolutePointByIndex(pColinear->PointBeginIndexOfElement[0]),
		pColinear->pElement[0]->GetAbsolutePointByIndex(pColinear->PointEndIndexOfElement[0])));
	POINT pt2 = pConfig->RealToScreen(pAttach->dptAttach);

		TDraw::DrawConstraintColinear_inner(hdc, pt1, pt2, pColinear->logpenStyleShow, pConfig);
	}

	pAttach->Draw(hdc);

	if (bShowTips)
		TDraw::DrawTips(hdc, ptMouse, sTips.c_str(), pConfig);
}