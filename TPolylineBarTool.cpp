
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
	Reset();

	if (pAttach != NULL)
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


	while (!vecpCoincide.empty())
	{
		delete vecpCoincide.back();
		vecpCoincide.pop_back();
	}

	while (!stackpColinear.empty())
	{
		delete stackpColinear.top();
		stackpColinear.pop();
	}

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
	sTips = TEXT("����Խ�����θ�");
	break;
	case 1:
		pPolylineBar->vecDpt.back() = TDraw::GetRelative(pAttach->dptAttach, dptHit[0], pPolylineBar->angle);
		sTips = TEXT("�Ҽ�/Esc��ȡ������");
		break;
	default:
		pPolylineBar->vecDpt.back() = TDraw::GetRelative(pAttach->dptAttach, dptHit[0], pPolylineBar->angle);

		sTips = TEXT("�Ҽ�/Enter/�ո����ɻ���\r\nEsc��ȡ������");
		break;
	}

	if (pAttach->bAttachedEndpoint)//��׽���˵�
	{
		sTips += TEXT("\r\n�������˵㣺�����غ�Լ��");
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
		break;
	default:
		DPOINT dptRelative = TDraw::GetRelative(pAttach->dptAttach, dptHit[0], pPolylineBar->angle);
		pPolylineBar->vecDpt.back() = dptRelative;
		pPolylineBar->vecDpt.push_back(dptRelative);
		break;
	}
	pPolylineBar->vecIsJoint.resize(pPolylineBar->vecDpt.size());//����Joint������С

	pAttach->vecdpt.push_back(pAttach->dptAttach);//ͬ������Attach����������


	if (pAttach->bAttachedEndpoint)
	{
		TConstraintCoincide *pCoincide = new TConstraintCoincide;
		pCoincide->SetStyle(pConfig->logpen);

		pCoincide->pElement[0] = pAttach->pAttachElement;
		pCoincide->PointIndexOfElement[0] = pAttach->iAttachElementPointIndex;

		pCoincide->pElement[1] = pPolylineBar;
		pCoincide->PointIndexOfElement[1] = dptHit.size();

		vecpCoincide.push_back(pCoincide);
	}

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
		pPolylineBar->vecDpt.erase(pPolylineBar->vecDpt.end() - 1);//ȥ�����һ��
		pPolylineBar->vecIsJoint.resize(pPolylineBar->vecDpt.size());

		AddIntoShape();
		Reset();
		break;
	}
}

void TPolylineBarTool::AddIntoShape()
{

	//���
	AddTreeViewItem(pPolylineBar, pShape->iNextId);

	TPolylineBar *pSavedPolylineBar = dynamic_cast<TPolylineBar *>(pShape->AddElement(pPolylineBar));

	//�غ�Լ�����
	while (!vecpCoincide.empty())
	{
		vecpCoincide.back()->pElement[1] = pSavedPolylineBar;//�����غ�Լ�����ӵ��ձ����Ԫ��

		AddTreeViewItem(vecpCoincide.back(), pShape->iNextId);
		pShape->AddElement(vecpCoincide.back());

		delete vecpCoincide.back();
		vecpCoincide.pop_back();
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

	for (auto pCoincide : vecpCoincide)
	{
		pCoincide->BuildpDpt();
		TDraw::DrawConstraintCoincide(hdc, pCoincide, pConfig);
	}

	pAttach->Draw(hdc);

	if (bShowTips)
		TDraw::DrawTips(hdc, ptMouse, ClientRect, sTips.c_str(), pConfig);
}