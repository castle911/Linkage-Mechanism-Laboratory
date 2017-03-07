#pragma once
#include <tchar.h>

#include "TSlider.h"
#include "TListView.h"
#include "MyMath.h"

TSlider::TSlider()
{
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_SLIDER;
	
}


TSlider::~TSlider()
{
}


void TSlider::NoticeListView(TListView *pListView)
{
	pListView->DeleteAllItems();

	TCHAR buffer[16];

	pListView->id = id;
	pListView->AddAttributeItem(TEXT("ID"), CTRLTYPE_NULL, NULL, TEXT("%d"), id);
	pListView->AddAttributeItem(TEXT("����"), CTRLTYPE_EDIT,&Name, Name);// stringToTCHAR(&Name,buffer)
	pListView->AddAttributeItem(TEXT("����"), CTRLTYPE_NULL, NULL, GetElementTypeName(buffer));
	pListView->AddAttributeItem(TEXT("����"), CTRLTYPE_NULL, NULL, GetLineStyleName(this->logpenStyle.lopnStyle, buffer));
	pListView->AddAttributeItem(TEXT("�߿�"), CTRLTYPE_NULL, NULL, TEXT("%d"), this->logpenStyle.lopnWidth);
	pListView->AddAttributeItem(TEXT("��ɫ"), CTRLTYPE_NULL, NULL, TEXT("0x%X"), this->logpenStyle.lopnColor);
	pListView->AddAttributeItem(TEXT("ԭ��"), CTRLTYPE_COOR_EDIT, &dpt, TEXT("%.3f,%.3f"), dpt.x,dpt.y);
	pListView->AddAttributeItem(TEXT("�Ƕ�"), CTRLTYPE_ANGLE_VALUE_EDIT, &angle, TEXT("%f"), REG2DEG(angle));
	for (auto i = 0; i < vecDpt.size(); ++i)
	{
		_stprintf(buffer, TEXT("P%d"), i);
		pListView->AddAttributeItem(buffer, CTRLTYPE_COOR_EDIT, &(vecDpt[i]), TEXT("%.3f,%.3f"), vecDpt[i].x, vecDpt[i].y);
	}
	//pListView->AddAttributeItem(TEXT("P2"), CTRLTYPE_COOR_P2_EDIT, &ptEnd, TEXT("%.3f,%.3f"), ptEnd.x, ptEnd.y);
	//pListView->AddAttributeItem(TEXT("����"), CTRLTYPE_LEN_EDIT, NULL, TEXT("%f"), dLength);
}