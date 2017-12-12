#pragma once
#include "TSlider.h"

#include "resource.h"
#include "TAttach.h"

#include "TConstraintCoincide.h"
#include "TConstraintColinear.h"
#include "TSliderTool.h"


TSliderTool::TSliderTool()
{
	pSlider = NULL;
	pAttach = NULL;

	Reset();
}


TSliderTool::~TSliderTool()
{
	Reset();

	if (pAttach!=NULL)
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
		sTips = TEXT("����Խ����»���");

		//������ʱ����
		pSlider->dpt = pAttach->dptAttach;
		if (pAttach->bShowExtensionLine)//��ʰȡֱ��
		{
			//����pSlider��angle
			switch (pAttach->pAttachElement->eType)
			{
			case ELEMENT_REALLINE:
			case ELEMENT_BAR:
			case ELEMENT_SLIDEWAY:
				pSlider->angle = pAttach->pAttachElement->angle;
				break;
			case ELEMENT_SLIDER:
			case ELEMENT_POLYLINEBAR:
			{
				TElement *pElement = pAttach->pAttachElement;
				int ptIndex0 = pAttach->iAttachLinePointIndex[0];
				int ptIndex1 = pAttach->iAttachLinePointIndex[1];
				DPOINT pt0 = pElement->GetAbsolutePointByIndex(ptIndex0);// TDraw::GetAbsolute(pElement->vecDpt[ptIndex0], pElement->dpt, pElement->angle);
				DPOINT pt1 = pElement->GetAbsolutePointByIndex(ptIndex1);// TDraw::GetAbsolute(pElement->vecDpt[ptIndex1], pElement->dpt, pElement->angle);
				double angle = TDraw::GetAngleFromPointReal(pt0, pt1);
				pSlider->angle = angle;
					break;
			}
			default:
				assert(0);
				break;
			}

			sTips += TEXT("\r\n�������߶Σ���������Լ��");
		}
		else
		{
			pSlider->angle = 0.0;
		}


		break;
	case 1://���п�λ��
		//������λ��

		pSlider->vecDpt[1] = TDraw::GetRelative(pAttach->dptAttach, pSlider->dpt, pSlider->angle);
		pSlider->vecLine[0] = { 0, 1 };

		sTips = TEXT("�����ȷ�������λ��\n�����Ҽ��ɲ����û����");
		break;
	}

	if (pAttach->bAttachedEndpoint)//��׽���˵�
	{
		sTips += TEXT("\r\n�������˵㣺�����غ�Լ��");
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
			TConstraintCoincide *pCoincide = new TConstraintCoincide;
			pCoincide->SetStyle(pConfig->logpen);
			pCoincide->pElement[0] = pAttach->pAttachElement;
			pCoincide->PointIndexOfElement[0] = pAttach->iAttachElementPointIndex;

			pCoincide->pElement[1] = pSlider;
			pCoincide->PointIndexOfElement[1] = 0;

			stackpCoincide.push(pCoincide);
		}

		if (pAttach->bShowExtensionLine)//��ʰȡֱ��
		{
			//���ù���Լ��
			TConstraintColinear *pColinear = new TConstraintColinear;
			pColinear->SetStyle(pConfig->logpen);
			pColinear->pElement[0] = pAttach->pAttachElement;
			pColinear->PointBeginIndexOfElement[0] = pAttach->iAttachLinePointIndex[0];
			pColinear->PointEndIndexOfElement[0] = pAttach->iAttachLinePointIndex[1];

			pColinear->pElement[1] = pSlider;
			pColinear->PointBeginIndexOfElement[1] = 0;
			pColinear->PointEndIndexOfElement[1] = -1;//{1,0}��

			stackpColinear.push(pColinear);
		}

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
	if (pAttach != NULL)
		delete pAttach;
	pAttach = new TAttach(pCanvas, pShape, pConfig);

	//������ʱ��
	if (pSlider != NULL)
		delete pSlider;
	pSlider = new TSlider;
	pSlider->SetStyle(pConfig);
	pSlider->vecDpt.resize(1);
	pSlider->vecIsJoint.resize(1);

	//����״̬
	dptHit.clear();

	bShowTips = true;


	while (!stackpCoincide.empty())
	{
		delete stackpCoincide.top();
		stackpCoincide.pop();
	}

	while (!stackpColinear.empty())
	{
		delete stackpColinear.top();
		stackpColinear.pop();
	}
}

void TSliderTool::AddIntoShape()
{
	//�������
	AddTreeViewItem(pSlider, pShape->iNextId);
	TSlider *pSavedSlider = dynamic_cast<TSlider *>(pShape->AddElement(pSlider));

	//�غ�Լ�����
	while (!stackpCoincide.empty())
	{
		stackpCoincide.top()->pElement[1] = pSavedSlider;

		AddTreeViewItem(stackpCoincide.top(), pShape->iNextId);
		pShape->AddElement(stackpCoincide.top());

		delete stackpCoincide.top();
		stackpCoincide.pop();
	}

	//����Լ�����
	while (!stackpColinear.empty())
	{
		stackpColinear.top()->pElement[1] = pSavedSlider;//����pElement��ָ��ձ����Slider

		AddTreeViewItem(stackpColinear.top(), pShape->iNextId);
		pShape->AddElement(stackpColinear.top());

		delete stackpColinear.top();
		stackpColinear.pop();
	}

	//ˢ�·�����
	RefreshEquations();
}

void TSliderTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	switch (dptHit.size())
	{
	case 0://�յ��Ҽ�
		//û�е������µ��Ҽ������ù���
		ResetTool();
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

		RefreshCanvas();

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
		TDraw::DrawTips(hdc, ptMouse,ClientRect, sTips.c_str(), pConfig);

}
