#pragma once
#include "DetectMemoryLeak.h"

#include "TCanvas.h"
#include "TAttach.h"
#include "TSlider.h"

TAttach::TAttach(TCanvas *pCanvas, TShape *pShape, TConfiguration *pConfig)
{
	TAttach::pCanvas = pCanvas;
	TAttach::pShape = pShape;
	TAttach::pConfig = pConfig;

	iIvoryLine = 0;

	//���ü�����
	bShowXAssist = false;
	bShowYAssist = false;

	XAssistLine = new TRealLine;
	YAssistLine = new TRealLine;
	XAssistLine->SetStyle(pConfig->logpenAssistLine);
	YAssistLine->SetStyle(pConfig->logpenAssistLine);

	//�����ӳ���
	bShowExtensionLine = false;
	ExtensionLine = new TRealLine;
	ExtensionLine->SetStyle(pConfig->logpenAssistLine);

	//���ò�׽��
	bAttachedEndpoint = true;
	bShowAttachPoint = false;
	pAttachElement = NULL;
	iAttachElementPointIndex = -1;
}


TAttach::~TAttach()
{
	delete XAssistLine;
	delete YAssistLine;

	delete ExtensionLine;
}

void TAttach::InitialLine(POINT ptPos)
{
	XAssistLine->ptBegin = pConfig->ScreenToReal(ptPos);
	XAssistLine->ptEnd = pConfig->ScreenToReal(ptPos);
	YAssistLine->ptBegin = pConfig->ScreenToReal(ptPos);
	YAssistLine->ptEnd = pConfig->ScreenToReal(ptPos);
}

void TAttach::Draw(HDC hdc)
{

	if (bShowAttachPoint)
	{
		TDraw::DrawCross(hdc, pConfig->RealToScreen(dptAttach), pConfig->CROSS_SIZE, pConfig->logpenFront);
	}
	//��X������
	if (bShowXAssist)
	{
		TDraw::DrawRealLine(hdc, *XAssistLine, pConfig);//
		TDraw::DrawCross(hdc, pConfig->RealToScreen(dptAttach), pConfig->CROSS_SIZE, pConfig->logpenFront);
	}

	//��Y������
	if (bShowYAssist)
	{
		TDraw::DrawRealLine(hdc, *YAssistLine, pConfig);//
		TDraw::DrawCross(hdc, pConfig->RealToScreen(dptAttach), pConfig->CROSS_SIZE, pConfig->logpenFront);
	}

	if (bShowExtensionLine)
	{
		TDraw::DrawRealLine(hdc, *ExtensionLine, pConfig);//
	}

}

void TAttach::AttachAll(POINT ptNowPos, DPOINT dptCheckPos)
{
	DPOINT dptPos = pConfig->ScreenToReal(ptNowPos);
	dptAttach = dptPos;
	AttachAxis(dptPos, pConfig->ScreenToReal(pConfig->GetOrg()));//����ԭ��������
	AttachAxis(dptPos, dptCheckPos);
	if (!AttachPoint(dptPos))//�������߶˵���Ϊ�߶˵����Ҫ�����ǵ������ߵ�Ч����
		AttachLine(ptNowPos);
}

void TAttach::AttachAll(POINT ptNowPos)
{
	DPOINT dptPos = pConfig->ScreenToReal(ptNowPos);
	dptAttach = dptPos;
	AttachAxis(dptPos, pConfig->ScreenToReal(pConfig->GetOrg()));//����ԭ��������
	if (!AttachPoint(dptPos))//�������߶˵���Ϊ�߶˵����Ҫ�����ǵ������ߵ�Ч����
		AttachLine(ptNowPos);
}


bool TAttach::AttachLine(POINT ptNowPos)
{
	bShowExtensionLine = false;
	for (auto i = pShape->Element.begin(); i != pShape->Element.end(); ++i)
	{
		switch ((*i)->eType)
		{
		case ELEMENT_REALLINE:
		case ELEMENT_BAR:
		case ELEMENT_SLIDEWAY:
			int status = TDraw::PointInRealLineOrExtension(ptNowPos, dptAttach, (TRealLine *)*i, pConfig);
			switch (status)
			{
			case 1:
				bAttachedEndpoint = false;
				bShowAttachPoint = true;
				bShowExtensionLine = true;
				ExtensionLine->SetPoint(((TRealLine *)*i)->ptBegin, dptAttach);
				pAttachElement = *i;
				return true;
			case 2:
				bAttachedEndpoint = false;
				bShowAttachPoint = true;
				bShowExtensionLine = true;
				ExtensionLine->SetPoint(((TRealLine *)*i)->ptEnd, dptAttach);
				pAttachElement = *i;
				return true;
			case 0:
				bAttachedEndpoint = false;
				bShowAttachPoint = true;
				bShowExtensionLine = true;
				ExtensionLine->SetPoint({ 0, 0 }, { 0, 0 });
				pAttachElement = *i;
				return true;
			}
		}
	}
	return false;
}

//���NowPos�Ƿ񿿽���CheckPosΪԭ��ļ��ᣬʹ��ǰӦ����dptAttachΪ��ǰ��
bool TAttach::AttachAxis(DPOINT dptNowPos, DPOINT dptCheckPos)
{

	iIvoryLine = 0;
	RECT CanvasClientRect;
	::GetClientRect(pCanvas->m_hWnd, &CanvasClientRect);
	//���X�Ḩ����
	XAssistLine->ptBegin = dptCheckPos;
	if (abs(pConfig->LengthToScreenY(dptNowPos.y - dptCheckPos.y)) < 10)//��ǰ�����ʼ��y��ƫ��10����
	{
		bShowXAssist = true;

		//���ø�����x,y����
		if (dptNowPos.x - XAssistLine->ptBegin.x > 0)//��ǰλ��λ�ڸ�������ʼ���ұ�
		{
			XAssistLine->ptEnd.x = pConfig->ScreenToRealX(CanvasClientRect.right);
			iIvoryLine = 1;
		}
		else
		{
			XAssistLine->ptEnd.x = pConfig->ScreenToRealX(CanvasClientRect.left);
			iIvoryLine = 3;
		}
		XAssistLine->ptEnd.y = XAssistLine->ptBegin.y;

		//dptAttach.x = dptNowPos.x;
		dptAttach.y = XAssistLine->ptEnd.y;//����ǰ��������������
	}
	else
	{
		bShowXAssist = false;
	}

	//���Y�Ḩ����
	YAssistLine->ptBegin = dptCheckPos;
	if (abs(pConfig->LengthToScreenX(dptNowPos.x - dptCheckPos.x)) < 10)
	{
		bShowYAssist = true;
		if (dptNowPos.y - YAssistLine->ptBegin.y > 0)//��ǰλ��λ�ڸ�������ʼ���ϱ�
		{
			YAssistLine->ptEnd.y = pConfig->ScreenToRealY(CanvasClientRect.top);
			iIvoryLine = 2;
		}
		else
		{
			YAssistLine->ptEnd.y = pConfig->ScreenToRealY(CanvasClientRect.bottom);
			iIvoryLine = 4;
		}
		YAssistLine->ptEnd.x = YAssistLine->ptBegin.x;

		//dptAttach.y = dptNowPos.y;
		dptAttach.x = YAssistLine->ptEnd.x;//����
	}
	else
	{
		bShowYAssist = false;
	}

	if (bShowXAssist || bShowYAssist)
		return true;
	else
		return false;
}

//��ȡpShape�е�RealLine����������ʹ��ǰӦ����dptAttachΪ��ǰ��
bool TAttach::AttachPoint(DPOINT dptPos)
{
	bAttachedEndpoint = false;
	bShowAttachPoint = false;
	pAttachElement = NULL;
	for (int i = 0; i < pShape->Element.size(); i++)
	{
		eAttachElementType = pShape->Element[i]->eType;
		switch (pShape->Element[i]->eType)
		{
		case ELEMENT_BAR:
		case ELEMENT_REALLINE:
		case ELEMENT_SLIDEWAY:
		{
			TRealLine *pRealLine = (TRealLine *)(pShape->Element[i]);
			//�������
			if (DPTisApproached(dptPos, pRealLine->ptBegin, 10))
			{
				bAttachedEndpoint = true;
				bShowAttachPoint = true;
				pAttachElement = pRealLine;
				iAttachElementPointIndex = 0;
				dptAttach = pRealLine->ptBegin;
				return true;
			}

			//�����յ�
			if (DPTisApproached(dptPos, pRealLine->ptEnd, 10))
			{
				bAttachedEndpoint = true;
				bShowAttachPoint = true;
				pAttachElement = pRealLine;
				iAttachElementPointIndex = 1;
				dptAttach = pRealLine->ptEnd;
				return true;
			}
			break;
		}
		case ELEMENT_FRAMEPOINT:
		{
			TFramePoint *pFramePoint = (TFramePoint *)(pShape->Element[i]);
			//�������ܵ�
			if (DPTisApproached(dptPos, pFramePoint->dpt, 10))
			{
				bAttachedEndpoint = true;
				bShowAttachPoint = true;
				pAttachElement = pFramePoint;
				iAttachElementPointIndex = 0;
				dptAttach = pFramePoint->dpt;
				return true;
			}
			break;
		}
		case ELEMENT_SLIDER:
		{
			TSlider *pSlider = (TSlider *)(pShape->Element[i]);
			for (auto iter = pSlider->vecDpt.begin(); iter != pSlider->vecDpt.end(); ++iter)
			{
				if (DPTisApproached(dptPos, TDraw::GetAbsolute(*iter, pSlider->dpt, pSlider->angle), 10))
				{
					bAttachedEndpoint = true;
					bShowAttachPoint = true;
					pAttachElement = pSlider;
					iAttachElementPointIndex = iter - pSlider->vecDpt.begin();
					dptAttach = TDraw::GetAbsolute(*iter, pSlider->dpt, pSlider->angle);
					return true;
				}
			}
			break;
		}
		//�������˵��Ԫ��
		case CONSTRAINT_COLINEAR:
		case CONSTRAINT_COINCIDE:
			break;
		default:
			assert(0);
			break;
		}
	}
	return false;
}

//������Ļ����С��distance
bool TAttach::DPTisApproached(DPOINT dpt1, DPOINT dpt2, int distance)
{
	if (abs(pConfig->LengthToScreenX(dpt1.x - dpt2.x)) < distance &&
		abs(pConfig->LengthToScreenY(dpt1.y - dpt2.y)) < distance)
		return true;
	else
		return false;
}