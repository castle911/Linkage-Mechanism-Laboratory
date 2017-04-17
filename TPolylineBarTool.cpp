
#include "TPolylineBarTool.h"

#include "TAttach.h"

#include "TPolylineBar.h"
#include "TConstraintCoincide.h"
#include "TConstraintColinear.h"

TPolylineBarTool::TPolylineBarTool()
{
	pPolylineBar = NULL;
	pAttach = NULL;

	Reset();
}


TPolylineBarTool::~TPolylineBarTool()
{
	delete pAttach;

	if (pPolylineBar != NULL)
		delete pPolylineBar;
}

void TPolylineBarTool::Reset()
{
	//������ʱ��
	if (pPolylineBar != NULL)
		delete pPolylineBar;
	pPolylineBar = new TPolylineBar;
	pPolylineBar->SetStyle(pConfig);

	//����״̬
	dptHit.clear();

	bShowTips = true;
	sTips = TEXT("����Խ�����θ�");

	if (pAttach != NULL)
		delete pAttach;
	pAttach = new TAttach(pCanvas, pShape, pConfig);

	RefreshCanvas();
}

void TPolylineBarTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_CROSS));
}

void TPolylineBarTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	ptMouse = ptPos;
	if (dptHit.empty())
		pAttach->AttachAll(ptPos);
	else
		pAttach->AttachAll(ptPos, dptHit.back());

	switch (dptHit.size())
	{
	case 0:
		break;
	default:
		pPolylineBar->vecDpt.back() = TDraw::GetRelative(pAttach->dptAttach, dptHit[0], pPolylineBar->angle);
		break;
	}
}

void TPolylineBarTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	switch (dptHit.size())
	{
	case 0:
		pPolylineBar->dpt = pAttach->dptAttach;
		pPolylineBar->angle = 0;
		pPolylineBar->vecDpt.push_back({ 0, 0 });
		pPolylineBar->vecDpt.push_back({ 0, 0 });
		sTips = TEXT("�Ҽ�/Esc��ȡ������");
		break;
	default:
		DPOINT dptRelative = TDraw::GetRelative(pAttach->dptAttach, dptHit[0], pPolylineBar->angle);
		pPolylineBar->vecDpt.back() = dptRelative;
		pPolylineBar->vecDpt.push_back(dptRelative);
		sTips = TEXT("�Ҽ�/Enter/�ո����ɻ���\r\nEsc��ȡ������");
		break;
	}
	pAttach->vecdpt.push_back(pAttach->dptAttach);

	dptHit.push_back(pAttach->dptAttach);
}

void TPolylineBarTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

void TPolylineBarTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	switch (dptHit.size())
	{
	case 0://�л�����
		ResetTool();
	case 1:
		Reset();
		break;
	default:
		AddIntoShape();
		break;
	}
}

void TPolylineBarTool::AddIntoShape()
{

	//���
	AddTreeViewItem(pPolylineBar, pShape->iNextId);
	TPolylineBar *pSavedPolylineBar = pShape->AddElement(pPolylineBar);

	//�غ�Լ�����
	while (!stackpCoincide.empty())
	{
		stackpCoincide.top()->pElement[1] = pSavedPolylineBar;

		AddTreeViewItem(stackpCoincide.top(), pShape->iNextId);
		pShape->AddElement(stackpCoincide.top());

		delete stackpCoincide.top();
		stackpCoincide.pop();
	}

	//����Լ�����
	while (!stackpColinear.empty())
	{
		stackpColinear.top()->pElement[1] = pSavedPolylineBar;

		AddTreeViewItem(stackpColinear.top(), pShape->iNextId);
		pShape->AddElement(stackpColinear.top());

		delete stackpColinear.top();
		stackpColinear.pop();
	}

	//ˢ�·�����
	RefreshEquations();
}

void TPolylineBarTool::Draw(HDC hdc)
{
	TDraw::DrawPolylineBar(hdc,pPolylineBar, pConfig);

	pAttach->Draw(hdc);

	if (bShowTips)
		TDraw::DrawTips(hdc, ptMouse, sTips.c_str(), pConfig);
}