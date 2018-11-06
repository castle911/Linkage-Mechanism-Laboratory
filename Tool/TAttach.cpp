#pragma once
#include "..\Common\DetectMemoryLeak.h"

#include "..\Control\TCanvas.h"
#include "..\Tool\TAttach.h"
#include "..\Element\TSlider.h"

#include "..\Element\TFramePoint.h"
#include "..\Element\TRealLine.h"
#include "..\Element\TPolylineBar.h"

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


bool TAttach::AttachLine_Element_inner(DPOINT dptNowPos, DPOINT dptAbsolute1, DPOINT dptAbsolute2, const TElement *pElement, int PointIndex[2],
	const TConfiguration *pConfig)
{

	//����dptNowPos��dpt1-dpt2���ϵ�λ��
	int status = TDraw::PointInRealLineOrExtension(dptNowPos, dptAttach, dptAbsolute1,dptAbsolute2, pConfig);
	if (status == -1)
		return false;

	//�Ѳ�׽
	bAttachedEndpoint = false;
	bShowAttachPoint = true;
	bShowExtensionLine = true;
	pAttachElement = const_cast<TElement *>(pElement);

	iAttachLinePointIndex[0] = PointIndex[0];
	iAttachLinePointIndex[1] = PointIndex[1];

	switch (status)
	{
	case 1://-1������ 0���߶��� 1��pt1һ���ӳ��� 2��pt2һ���ӳ���
		ExtensionLine->SetPoint(dptAbsolute1, dptAttach);
		return true;
	case 2:
		ExtensionLine->SetPoint(dptAbsolute2, dptAttach);
		return true;
	case 0:
		ExtensionLine->SetPoint({ 0, 0 }, { 0, 0 });
		return true;
	}
	return false;
}

bool TAttach::AttachLineByRelativeVecPt(DPOINT dptNowPos, TElement *pElement, const std::vector<DPOINT> &vecdptRelative, const TConfiguration *pConfig)
{
	std::vector<DPOINT> vecdptAbsolute;
	TDraw::GetAbsoluteReal(vecdptAbsolute, vecdptRelative, pElement->dpt, pElement->angle);

	return AttachLineByAbsoluteVecPt(dptNowPos, pElement,vecdptAbsolute);
}

//�Ծ�������vecdptAbsolute˳�򹹳ɵ��߶Σ����ptNowPos�Ƿ�λ�����ϣ������ò�����
//bAttachedEnepoint
//bShowAttachPoint
//bShowExtensionLine  ExtensionLine
//iAttachLinePointIndex[0] iAttachLinePointIndex[1]
bool TAttach::AttachLineByAbsoluteVecPt(DPOINT dptNowPos,TElement *pElement,const std::vector<DPOINT> &vecdptAbsolute)
{
	if (vecdptAbsolute.empty()) return false;

	bShowExtensionLine = false;
	pAttachElement = NULL;

	for (auto pt = vecdptAbsolute.begin(); pt != vecdptAbsolute.end() - 1; ++pt)
	{
		int iPointIndex[2] = { pt - vecdptAbsolute.begin(), pt + 1 - vecdptAbsolute.begin() };
		if (AttachLine_Element_inner(dptNowPos, *pt, *(pt + 1),pElement, iPointIndex, pConfig))
		{
			pAttachElement = pElement;
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
		if (pElement->IsAttached(dptNowPos,this,pConfig))
			return true;
	}
	return false;
}

bool TAttach::AttachLineSelf(DPOINT dptNowPos)
{
	return AttachLineByAbsoluteVecPt(dptNowPos, nullptr, vecdpt);
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

bool TAttach::AttachPointByElement(DPOINT dptNowPos,DPOINT &dpt,int iPointIndex,const TElement *pElement, const TConfiguration *pConfig)
{
	if (DPTisApproached(dptNowPos,dpt))
	{
		bAttachedEndpoint = true;
		bShowAttachPoint = true;
		pAttachElement =const_cast<TElement *>(pElement);
		iAttachElementPointIndex = iPointIndex;
		dptAttach = dpt;
		return true;
	}
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
		if (pElement->PointIsAttached(dptPos, this, pConfig))
			return true;
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