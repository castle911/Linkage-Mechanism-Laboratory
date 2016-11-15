#pragma once
#include "TFramePoint.h"

#include "TListView.h"
TFramePoint::TFramePoint()
{
	dpt.x = 0;
	dpt.y = 0;
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_FRAMEPOINT;
}


TFramePoint::~TFramePoint()
{
}

void TFramePoint::NoticeListView(TListView *pListView)
{
	pListView->DeleteAllItems();

	TCHAR buffer[16];

	pListView->AddAttributeItem(TEXT("����"), Name);
	pListView->AddAttributeItem(TEXT("����"), TEXT("����"));
	pListView->AddAttributeItem(TEXT("����"), GetLineStyleName(this->logpenStyle.lopnStyle, buffer));
	pListView->AddAttributeItem(TEXT("�߿�"), TEXT("%d"), this->logpenStyle.lopnWidth);
	pListView->AddAttributeItem(TEXT("��ɫ"), TEXT("0x%X"), this->logpenStyle.lopnColor);
	pListView->AddAttributeItem(TEXT("P"), TEXT("%.3f,%.3f"),dpt.x, dpt.y);
}