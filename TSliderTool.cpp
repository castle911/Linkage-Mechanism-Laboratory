#pragma once
#include "TSlider.h"

#include "TCanvas.h"
#include "resource.h"
#include "TAttach.h"

#include "TSliderTool.h"

TSliderTool::TSliderTool()
{
	pAttach = new TAttach(pCanvas, pShape, pConfig);
	pSlider = NULL;


	pSlider = new TSlider;
	pSlider->SetStyle(pConfig->logpen);
	pSlider->vecDpt.resize(1);
	pSlider->vecIsJoint.resize(1);
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
	case 1://��һ�ε㣬���û���

		//������ʱ����
		pSlider->dpt = pAttach->dptAttach;
		pSlider->vecDpt[0] = TDraw::GetRelative(pSlider->dpt, pSlider->dpt, pSlider->angle);
		pSlider->vecDpt.resize(2);
		pSlider->vecIsJoint.resize(2);
		pSlider->vecLine.resize(1);
		if (pAttach->bShowExtensionLine)//��ʰȡֱ��
		{
			//������ʱֱ��Լ��

			pSlider->angle = pAttach->pAttachElement->angle;
		}
		else
		{
			pSlider->angle = 0.0;
		}
		break;
	case 2://�ڶ��ε� ���

		//�������
		pShape->AddSlider(pSlider);

		//����Լ�����

		if (pAttach->bAttachedEndpoint)//��׽���˵� ������غ�Լ��
		{
			//����غ�Լ��

			//�غ�Լ�����
		}

		//����״̬
		Reset();
		OnMouseMove(hWnd, nFlags, ptPos);

		break;
	}
}

void TSliderTool::Reset()
{
	//������ʱ��
	if (pSlider!=NULL)
		delete pSlider;
	pSlider = new TSlider;
	pSlider->SetStyle(pConfig->logpen);
	pSlider->vecDpt.resize(1);
	pSlider->vecIsJoint.resize(1);

	//����״̬
	dptHit.clear();

	//delete pAttach;
	//pAttach = new TAttach(pCanvas, pShape, pConfig);

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

		//�������
		pShape->AddSlider(pSlider);

		//����Լ�����

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
}
