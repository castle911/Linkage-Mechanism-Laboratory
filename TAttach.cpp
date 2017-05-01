#pragma once
#include "DetectMemoryLeak.h"

#include "TCanvas.h"
#include "TAttach.h"
#include "TSlider.h"

#include "TFramePoint.h"
#include "TRealLine.h"
#include "TPolylineBar.h"

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

	bAttachedEndpointSelf = false;
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
	dptAttach = pConfig->ScreenToReal(ptNowPos);

	AttachAxis(dptAttach, pConfig->ScreenToReal(pConfig->GetOrg()));//����ԭ��������
	AttachAxis(dptAttach, dptCheckPos);

//�������߶˵���Ϊ�߶˵����Ҫ�����ǵ������ߵ�Ч����

	//Խ����Խ����
	if (!AttachPointSelf(dptAttach))
		if (!AttachPoint(dptAttach))
			if (!AttachLine_inner(dptAttach))
				if (!AttachLineSelf(dptAttach))
				return;
}

void TAttach::AttachAll(POINT ptNowPos)
{
	dptAttach = pConfig->ScreenToReal(ptNowPos);
	AttachAxis(dptAttach, pConfig->ScreenToReal(pConfig->GetOrg()));//����ԭ��������

//�������߶˵���Ϊ�߶˵����Ҫ�����ǵ������ߵ�Ч����

	//Խ����Խ����
	if (!AttachPointSelf(dptAttach))
		if (!AttachPoint(dptAttach))
			if (!AttachLine_inner(dptAttach))
				if (!AttachLineSelf(dptAttach))
			return;
}

//�Ծ�������vecdptAbsolute˳�򹹳ɵ��߶Σ����ptNowPos�Ƿ�λ�����ϣ������ò�����
//bAttachedEnepoint
//bShowAttachPoint
//bShowExtensionLine  ExtensionLine
//iAttachLinePointIndex[0] iAttachLinePointIndex[1]
bool TAttach::AttachLine_Element(DPOINT dptNowPos,const std::vector<DPOINT> vecdptAbsolute)
{
	if (vecdptAbsolute.empty()) return false;

	bShowExtensionLine = false;
	pAttachElement = NULL;

	for (auto pt = vecdptAbsolute.begin(); pt != vecdptAbsolute.end() - 1; ++pt)
	{
		int status = TDraw::PointInRealLineOrExtension(dptNowPos,dptAttach, *pt, *(pt + 1), pConfig);
		if (status == -1)
			continue;

		//�Ѳ�׽
		bAttachedEndpoint = false;
		bShowAttachPoint = true;
		bShowExtensionLine = true;

		iAttachLinePointIndex[0] = pt - vecdptAbsolute.begin();
		iAttachLinePointIndex[1] = pt + 1 - vecdptAbsolute.begin();

		switch (status)
		{
		case 1://-1������ 0���߶��� 1��pt1һ���ӳ��� 2��pt2һ���ӳ���
			ExtensionLine->SetPoint(*pt, dptAttach);
			return true;
		case 2:
			ExtensionLine->SetPoint(*(pt + 1), dptAttach);
			return true;
		case 0:
			ExtensionLine->SetPoint({ 0, 0 }, { 0, 0 });
			return true;
		}
	}
	return false;
}

void TAttach::AttachLine(POINT ptNowPos)
{
	dptAttach = pConfig->ScreenToReal(ptNowPos);
	AttachLine_inner(dptAttach);
}

bool TAttach::AttachLine_inner(DPOINT dptNowPos)
{
	bShowExtensionLine = false;
	pAttachElement = NULL;

	for (auto pElement:pShape->Element)
	{
		switch (pElement->eType)
		{
		case ELEMENT_SLIDER:
		case ELEMENT_POLYLINEBAR:
		{
			std::vector<DPOINT> vecdptAbsolute;
			TDraw::GetAbsoluteReal(vecdptAbsolute, pElement->vecDpt, pElement->dpt, pElement->angle);

			if (AttachLine_Element(dptNowPos, vecdptAbsolute))
			{
				pAttachElement = pElement;
				return true;
			}

			//����Slider
			if (pElement->eType == ELEMENT_SLIDER)
			{
				std::vector<DPOINT> vecdptAbsoluteSlider;
				vecdptAbsoluteSlider.push_back(pElement->GetAbsolutePointByIndex(-1));
				vecdptAbsoluteSlider.push_back(pElement->GetAbsolutePointByIndex(0));

				if (AttachLine_Element(dptNowPos, vecdptAbsoluteSlider))
				{
					iAttachLinePointIndex[0] = -1;
					iAttachLinePointIndex[1] = 0;
					pAttachElement = pElement;
					return true;
				}
			}

			break;
		}
		case ELEMENT_REALLINE:
		case ELEMENT_BAR:
		case ELEMENT_SLIDEWAY:
		{
			TRealLine *pRealLine = (TRealLine *)pElement;
			int status = TDraw::PointInRealLineOrExtension(dptNowPos, dptAttach, pRealLine->ptBegin,pRealLine->ptEnd, pConfig);
			if (status == -1)
				break;

				bAttachedEndpoint = false;
				bShowAttachPoint = true;
				bShowExtensionLine = true;
				pAttachElement = pElement;

				iAttachLinePointIndex[0] = 0;
				iAttachLinePointIndex[1] = 1;

			switch (status)
			{
			case 1://-1������ 0���߶��� 1��pt1һ���ӳ��� 2��pt2һ���ӳ���
				ExtensionLine->SetPoint(((TRealLine *)pElement)->ptBegin, dptAttach);
				return true;
			case 2:
				ExtensionLine->SetPoint(((TRealLine *)pElement)->ptEnd, dptAttach);
				return true;
			case 0:
				ExtensionLine->SetPoint({ 0, 0 }, { 0, 0 });
				return true;
			}
		}
		case ELEMENT_FRAMEPOINT:
		case CONSTRAINT_COINCIDE:
		case CONSTRAINT_COLINEAR:
			break;
		default:
			assert(0);
			break;
		}
	}
	return false;
}

bool TAttach::AttachLineSelf(DPOINT dptNowPos)
{
	if (AttachLine_Element(dptNowPos, vecdpt))
	{
		//pAttachElement = pElement;
		return true;
	}
	else
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
	for (auto pElement: pShape->Element)
	{
		eAttachElementType = pElement->eType;
		switch (pElement->eType)
		{
		case ELEMENT_BAR:
		case ELEMENT_REALLINE:
		case ELEMENT_SLIDEWAY:
		{
			TRealLine *pRealLine = (TRealLine *)pElement;
			//�������
			if (DPTisApproached(dptPos, pRealLine->ptBegin))
			{
				bAttachedEndpoint = true;
				bShowAttachPoint = true;
				pAttachElement = pRealLine;
				iAttachElementPointIndex = 0;
				dptAttach = pRealLine->ptBegin;
				return true;
			}

			//�����յ�
			if (DPTisApproached(dptPos, pRealLine->ptEnd))
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
			TFramePoint *pFramePoint = (TFramePoint *)pElement;
			//�������ܵ�
			if (DPTisApproached(dptPos, pFramePoint->dpt))
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
		case ELEMENT_POLYLINEBAR:
		case ELEMENT_SLIDER:
		{
			for (auto iter = pElement->vecDpt.begin(); iter != pElement->vecDpt.end(); ++iter)
			{
				if (DPTisApproached(dptPos, TDraw::GetAbsolute(*iter, pElement->dpt, pElement->angle)))
				{
					bAttachedEndpoint = true;
					bShowAttachPoint = true;
					pAttachElement = pElement;
					iAttachElementPointIndex = iter - pElement->vecDpt.begin();
					dptAttach = TDraw::GetAbsolute(*iter, pElement->dpt, pElement->angle);
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

//����vecdpt��ʹ��ǰӦ����dptAttachΪ��ǰ��
bool TAttach::AttachPointSelf(DPOINT dptPos)
{
	bAttachedEndpointSelf = false;
	bShowAttachPoint = false;
	for (auto dpt : vecdpt)
	{
		if (DPTisApproached(dptPos, dpt))
		{
			bAttachedEndpointSelf = true;
			bShowAttachPoint = true;
			dptAttach = dpt;
			return true;
		}
	}
	return false;
}

//������Ļ����С��distance
bool TAttach::DPTisApproached(DPOINT dpt1, DPOINT dpt2)
{
	if (abs(pConfig->LengthToScreenX(dpt1.x - dpt2.x)) < iAttachPixel &&
		abs(pConfig->LengthToScreenY(dpt1.y - dpt2.y)) < iAttachPixel)
		return true;
	else
		return false;
}