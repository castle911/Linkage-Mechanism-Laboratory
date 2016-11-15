#pragma once
#include <tchar.h>
#include <stdio.h>

#include "TListView.h"
#include "TBar.h"

TBar::TBar()
{
	TBar::dLength = 0;
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_BAR;
}

TBar::~TBar()
{
}

void TBar::NoticeListView(TListView *pListView)
{
	pListView->DeleteAllItems();

	TCHAR buffer[16];

	pListView->AddAttributeItem(TEXT("����"), Name);// stringToTCHAR(&Name,buffer)
	pListView->AddAttributeItem(TEXT("����"), TEXT("����"));
	pListView->AddAttributeItem(TEXT("����"), GetLineStyleName(this->logpenStyle.lopnStyle, buffer));
	pListView->AddAttributeItem(TEXT("�߿�"), TEXT("%d"), this->logpenStyle.lopnWidth);
	pListView->AddAttributeItem(TEXT("��ɫ"), TEXT("0x%X"), this->logpenStyle.lopnColor);
	pListView->AddAttributeItem(TEXT("P1"), TEXT("%.3f,%.3f"), ptBegin.x, ptBegin.y);
	pListView->AddAttributeItem(TEXT("P2"), TEXT("%.3f,%.3f"), ptEnd.x, ptEnd.y);
	pListView->AddAttributeItem(TEXT("����"), TEXT("%f"), dLength);
}