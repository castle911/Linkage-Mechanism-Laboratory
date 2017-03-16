#pragma once

#include <tchar.h>

#include "TListView.h"
#include "TConstraintColinear.h"


TConstraintColinear::TConstraintColinear()
{
	eType = CONSTRAINT_COLINEAR;
	pElement[0] = NULL;
	pElement[1] = NULL;
	_tcscpy(Name, TEXT(""));
}


TConstraintColinear::~TConstraintColinear()
{
}

void TConstraintColinear::NoticeListView(TListView *pListView)
{
	pListView->DeleteAllItems();

	TCHAR buffer[32];

	pListView->id = id;
	pListView->AddAttributeItem(TEXT("ID"), CTRLTYPE_NULL, NULL, TEXT("%d"), id);
	pListView->AddAttributeItem(TEXT("����"), CTRLTYPE_EDIT, &Name, Name);
	pListView->AddAttributeItem(TEXT("����"), CTRLTYPE_NULL, NULL, TEXT("�غ�"));
	pListView->AddAttributeItem(TEXT("����"), CTRLTYPE_NULL, NULL, GetLineStyleName(this->logpenStyle.lopnStyle, buffer));
	pListView->AddAttributeItem(TEXT("�߿�"), CTRLTYPE_NULL, NULL, TEXT("%d"), this->logpenStyle.lopnWidth);
	pListView->AddAttributeItem(TEXT("��ɫ"), CTRLTYPE_NULL, NULL, TEXT("0x%X"), this->logpenStyle.lopnColor);

	wsprintf(buffer, TEXT("ID:%d = ID:%d"), pElement[0]->id, pElement[1]->id);

	pListView->AddAttributeItem(TEXT("Value"), CTRLTYPE_NULL, NULL, buffer);
}