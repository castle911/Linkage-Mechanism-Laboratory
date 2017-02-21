#pragma once
#include "TAttach.h"


TAttach::TAttach(HWND hCanvas, TShape *shape, TConfiguration *config)
{
	TAttach::hCanvas = hCanvas;
	TAttach::Shape = shape;
	TAttach::Config = config;

	iIvoryLine = 0;

	bShowXAssist = false;
	bShowYAssist = false;

	XAssistLine = new TRealLine;
	YAssistLine = new TRealLine;
	XAssistLine->SetStyle(PS_DOT, 1, Config->crDot);
	YAssistLine->SetStyle(PS_DOT, 1, Config->crDot);

	pAttachElement = NULL;
	iAttachElementPointIndex = -1;
}


TAttach::~TAttach()
{
	delete XAssistLine;
	delete YAssistLine;
}

void TAttach::InitialLine(POINT ptPos)
{
	XAssistLine->ptBegin = Config->ScreenToReal(ptPos);
	XAssistLine->ptEnd = Config->ScreenToReal(ptPos);
	YAssistLine->ptBegin = Config->ScreenToReal(ptPos);
	YAssistLine->ptEnd = Config->ScreenToReal(ptPos);
}

void TAttach::Draw(HDC hdc)
{

	if (pAttachElement)
	{
		TDraw::DrawCross(hdc, Config->RealToScreen(dptAttach), 18, { PS_SOLID, { 1, 0 }, Config->crPen });
	}
	//��X������
	if (bShowXAssist)
	{
		TDraw::DrawRealLine(hdc, *XAssistLine, Config);//
		TDraw::DrawCross(hdc, Config->RealToScreen(dptAttach), 18, { PS_SOLID, { 1, 0 }, Config->crPen });
	}

	//��Y������
	if (bShowYAssist)
	{
		TDraw::DrawRealLine(hdc, *YAssistLine, Config);//
		TDraw::DrawCross(hdc, Config->RealToScreen(dptAttach), 18, { PS_SOLID, { 1, 0 }, Config->crPen });
	}

}

void TAttach::AttachAll(POINT ptNowPos, DPOINT dptCheckPos)
{
	DPOINT dptPos = Config->ScreenToReal(ptNowPos);
	dptAttach = dptPos;
	AttachAxis(dptPos, Config->ScreenToReal(Config->GetOrg()));
	AttachAxis(dptPos, dptCheckPos);
	AttachPoint(dptPos);//�������߶˵���Ϊ�߶˵����Ҫ�����ǵ������ߵ�Ч����
}

void TAttach::AttachAll(POINT ptNowPos)
{
	DPOINT dptPos = Config->ScreenToReal(ptNowPos);
	dptAttach = dptPos;
	AttachAxis(dptPos, Config->ScreenToReal(Config->GetOrg()));
	AttachPoint(dptPos);//�������߶˵���Ϊ�߶˵����Ҫ�����ǵ������ߵ�Ч����
}

//���NowPos�Ƿ񿿽�CheckPos�ļ��ᣬʹ��ǰӦ����dptAttach
bool TAttach::AttachAxis(DPOINT dptNowPos, DPOINT dptCheckPos)
{

	iIvoryLine = 0;
	RECT CanvasClientRect;
	::GetClientRect(hCanvas, &CanvasClientRect);
	//���X�Ḩ����
	XAssistLine->ptBegin = dptCheckPos;
	if (abs(Config->LengthToScreenY(dptNowPos.y - dptCheckPos.y)) < 10)//��ǰ�����ʼ��y��ƫ��10����
	{
		bShowXAssist = true;

		//���ø�����x,y����
		if (dptNowPos.x - XAssistLine->ptBegin.x > 0)//��ǰλ��λ�ڸ�������ʼ���ұ�
		{
			XAssistLine->ptEnd.x = Config->ScreenToRealX(CanvasClientRect.right);
			iIvoryLine = 1;
		}
		else
		{
			XAssistLine->ptEnd.x = Config->ScreenToRealX(CanvasClientRect.left);
			iIvoryLine = 3;
		}
		XAssistLine->ptEnd.y = XAssistLine->ptBegin.y;

		//dptAttach.x = dptCheckPos.x;
		dptAttach.y = XAssistLine->ptEnd.y;//����ǰ��������������
	}
	else
	{
		bShowXAssist = false;
	}

	//���Y�Ḩ����
	YAssistLine->ptBegin = dptCheckPos;
	if (abs(Config->LengthToScreenX(dptNowPos.x - dptCheckPos.x)) < 10)
	{
		bShowYAssist = true;
		if (dptNowPos.y - YAssistLine->ptBegin.y > 0)//��ǰλ��λ�ڸ�������ʼ���ϱ�
		{
			YAssistLine->ptEnd.y = Config->ScreenToRealY(CanvasClientRect.top);
			iIvoryLine = 2;
		}
		else
		{
			YAssistLine->ptEnd.y = Config->ScreenToRealY(CanvasClientRect.bottom);
			iIvoryLine = 4;
		}
		YAssistLine->ptEnd.x = YAssistLine->ptBegin.x;

		//dptAttach.y = dptCheckPos.y;
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

//��ȡShape�е�RealLine����������ʹ��ǰӦ����dptAttach
void TAttach::AttachPoint(DPOINT dptPos)
{
	pAttachElement = NULL;
	for (int i = 0; i < Shape->Element.size(); i++)
	{
		eAttachElementType = Shape->Element[i]->eType;
		switch (Shape->Element[i]->eType)
		{
		case ELEMENT_BAR:
		case ELEMENT_REALLINE:
		{
			TRealLine *pRealLine = (TRealLine *)(Shape->Element[i]);
			//�������
			if (DPTisApproached(dptPos, pRealLine->ptBegin, 10))
			{
				pAttachElement = pRealLine;
				iAttachElementPointIndex = 1;
				dptAttach = pRealLine->ptBegin;
				return;
			}

			//�����յ�
			if (DPTisApproached(dptPos, pRealLine->ptEnd, 10))
			{
				pAttachElement = pRealLine;
				iAttachElementPointIndex = 2;
				dptAttach = pRealLine->ptEnd;
				return;
			}
			break;
		}
		case ELEMENT_FRAMEPOINT:
		{
			TFramePoint *pFramePoint = (TFramePoint *)(Shape->Element[i]);
			//�������ܵ�
			if (DPTisApproached(dptPos, pFramePoint->dpt, 10))
			{
				pAttachElement = pFramePoint;
				iAttachElementPointIndex = 0;
				dptAttach = pFramePoint->dpt;
				return;
			}
			break;
		}
		}
	}
}

bool TAttach::DPTisApproached(DPOINT dpt1, DPOINT dpt2, int distance)
{
	if (abs(Config->LengthToScreenX(dpt1.x - dpt2.x)) < distance &&
		abs(Config->LengthToScreenY(dpt1.y - dpt2.y)) < distance)
		return true;
	else
		return false;
}