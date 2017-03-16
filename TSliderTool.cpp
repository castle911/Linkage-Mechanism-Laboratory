#pragma once
#include "TSlider.h"

#include "TSolver.h"
#include "TCanvas.h"
#include "resource.h"
#include "TAttach.h"
#include "TTreeViewContent.h"

#include "TConstraintCoincide.h"
#include "TConstraintColinear.h"
#include "TSliderTool.h"

TSliderTool::TSliderTool()
{
	pAttach = new TAttach(pCanvas, pShape, pConfig);
	pSlider = NULL;

	Reset();
}


TSliderTool::~TSliderTool()
{
	delete pAttach;
	if (pSlider != NULL)
		delete pSlider;
}

void TSliderTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_CROSS));
}


void TSliderTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

}

void TSliderTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	ptMouse = ptPos;
	pAttach->AttachAll(ptPos);

	switch (dptHit.size())
	{
	case 0:
		//������ʱ����
		pSlider->dpt = pAttach->dptAttach;
		if (pAttach->bShowExtensionLine)//��ʰȡֱ��
		{
			pSlider->angle = pAttach->pAttachElement->angle;
		}
		else
		{
			pSlider->angle = 0.0;
		}


		break;
	case 1://���п�λ��
		//������λ��
		
		pSlider->vecDpt[1] =TDraw::GetRelative(pAttach->dptAttach,pSlider->dpt,pSlider->angle);
		pSlider->vecLine[0] = { 0, 1 };

		break;
	}
}

void TSliderTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	dptHit.push_back(ptPos);

	switch (dptHit.size())
	{
	case 1://��һ�ε���������û���
	{
		//���û���
		pSlider->dpt = pAttach->dptAttach;
		pSlider->vecDpt[0] = TDraw::GetRelative(pSlider->dpt, pSlider->dpt, pSlider->angle);
		pSlider->vecDpt.resize(2);
		pSlider->vecIsJoint.resize(2);
		pSlider->vecLine.resize(1);

		if (pAttach->bAttachedEndpoint)//��׽���˵� ������غ�Լ��
		{
			//����غ�Լ��
			TConstraintCoincide *pCoincide=new TConstraintCoincide;
			pCoincide->SetStyle(pConfig->logpen);
			pCoincide->pElement[0] = pAttach->pAttachElement;
			pCoincide->PointIndexOfElement[0] = pAttach->iAttachElementPointIndex;

			pCoincide->pElement[1] = pSlider;
			pCoincide->PointIndexOfElement[1] = 0;

			stackpCoincide.push(pCoincide);
		}

		if (pAttach->bShowExtensionLine)//��ʰȡֱ��
		{

			pSlider->angle = pAttach->pAttachElement->angle;

			//���ù���Լ��
			TConstraintColinear *pColinear = new TConstraintColinear;
			pColinear->SetStyle(pConfig->logpen);
			pColinear->pElement[0] = pAttach->pAttachElement;
			pColinear->pElement[1] = pSlider;

			stackpColinear.push(pColinear);

		}
		else
		{
			pSlider->angle = 0.0;
		}

		sTips = TEXT("�����ȷ�������λ��\n�����Ҽ��ɲ����û����");
		break;
	}
	case 2://�ڶ��ε����


		if (pAttach->bAttachedEndpoint)//��׽���˵� ������غ�Լ��
		{
			//����غ�Լ��
			TConstraintCoincide *pCoincide = new TConstraintCoincide;
			pCoincide->SetStyle(pConfig->logpen);
			pCoincide->pElement[0] = pAttach->pAttachElement;
			pCoincide->PointIndexOfElement[0] = pAttach->iAttachElementPointIndex;

			pCoincide->pElement[1] = pSlider;
			pCoincide->PointIndexOfElement[1] = 1;

			stackpCoincide.push(pCoincide);

		}

		//���
		AddIntoShape();

		//����״̬
		Reset();
		OnMouseMove(hWnd, nFlags, ptPos);

		break;
	}
}

void TSliderTool::Reset()
{
	//������ʱ��
	if (pSlider != NULL)
		delete pSlider;
	pSlider = new TSlider;
	pSlider->SetStyle(pConfig->logpen);
	pSlider->vecDpt.resize(1);
	pSlider->vecIsJoint.resize(1);

	//����״̬
	dptHit.clear();

	bShowTips = true;
	sTips = TEXT("����Խ����»���");
}

void TSliderTool::AddIntoShape()
{
		//�������
		pTreeViewContent->AddItem(pSlider, pShape->iNextId);
		TSlider *pSavedSlider = pShape->AddElement(pSlider);

		//�غ�Լ�����
		while (!stackpCoincide.empty())
		{
			stackpCoincide.top()->pElement[1] = pSavedSlider;

			pTreeViewContent->AddItem(stackpCoincide.top(), pShape->iNextId);
			pShape->AddElement(stackpCoincide.top());

			delete stackpCoincide.top();
			stackpCoincide.pop();
		}

		//����Լ�����
		while (!stackpColinear.empty())
		{
			stackpColinear.top()->pElement[1] = pSavedSlider;

			pTreeViewContent->AddItem(stackpColinear.top(), pShape->iNextId);
			pShape->AddElement(stackpColinear.top());

			delete stackpColinear.top();
			stackpColinear.pop();
		}

		//ˢ�·�����
		pSolver->RefreshEquations();
}

void TSliderTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	switch (dptHit.size())
	{
	case 0://�յ��Ҽ�
		//û�е������µ��Ҽ������ù���
		::PostMessage(hwndWin, WM_COMMAND, ID_SELECT, 0);
		break;
	case 1://�ջ����黹û����

		//������Ϊԭ��
		pSlider->vecDpt.resize(1);
		pSlider->vecIsJoint.resize(1);
		pSlider->vecLine.clear();


		//���
		AddIntoShape();

		//����״̬
		Reset();
		OnMouseMove(hWnd, nFlags, ptPos);

		pCanvas->Invalidate();

		break;
	}
}

void TSliderTool::OnMouseWheel(HWND hWnd, UINT nFlags, POINT ptPos)
{

}

void TSliderTool::Draw(HDC hdc)
{

	TDraw::DrawSlider(hdc, pSlider, pConfig);

	pAttach->Draw(hdc);
	
	if (bShowTips)
		TDraw::DrawTips(hdc, ptMouse, sTips.c_str(), pConfig);
}
