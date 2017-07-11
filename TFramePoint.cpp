#pragma once
#include "DetectMemoryLeak.h"
#include "TFramePoint.h"

#include "TConfiguration.h"
#include "TDraw.h"
#include "TListView.h"
TFramePoint::TFramePoint()
{
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_FRAMEPOINT;

	vecIsJoint.resize(1);
}


TFramePoint::~TFramePoint()
{
}

const TCHAR * TFramePoint::GetElementTypeName(TCHAR name[])//�õ���������
{
	return _tcscpy(name, TEXT("����"));
}

void TFramePoint::NoticeListView(TListView *pListView)
{
	TElement::NoticeListView(pListView);

	pListView->AddAttributeItem(TEXT("P0"), CTRLTYPE_COOR_EDIT, &dpt, TEXT("%.3f,%.3f"), dpt.x, dpt.y);
}

void TFramePoint::Draw(HDC hdc, const TConfiguration* pConfig)
{
	TDraw::DrawFramePoint(hdc, this, pConfig);
}

void TFramePoint::DrawPickSquare(HDC hdc, const TConfiguration* pConfig)
{
	//��ʰȡ����
	TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(dpt));
}

bool TFramePoint::Picked(const POINT &ptPos, const TConfiguration *pConfig)
{
	return TDraw::PointInFramePoint(pConfig->RealToScreen(dpt), ptPos, pConfig);
}