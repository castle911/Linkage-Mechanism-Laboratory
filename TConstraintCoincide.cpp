#pragma once

#include <tchar.h>
#include <stdio.h>

#include "TDraw.h"
#include "TConstraintCoincide.h"

#include "TListView.h"
#include "TRealLine.h"

TConstraintCoincide::TConstraintCoincide()
{
	eType = CONSTRAINT_COINCIDE;
	pElement1 = NULL;
	pElement2 = NULL;
	pDpt1 = NULL;
	pDpt2 = NULL;
	_tcscpy(Name, TEXT(""));
}


TConstraintCoincide::~TConstraintCoincide()
{
}

void TConstraintCoincide::RestorePointStyle()
{
	if (pElement1!=NULL)
	for (auto iter = pElement1->vecIsJoint.begin(); iter != pElement1->vecIsJoint.end(); ++iter)
		for (auto iterId = (*iter).begin(); iterId != (*iter).end(); ++iterId)
		{
			if ((*iterId) == id)
			{
				iter->erase(iterId);
				break;
			}
		}

	if (pElement2 != NULL)
	for (auto iter = pElement2->vecIsJoint.begin(); iter != pElement2->vecIsJoint.end(); ++iter)
		for (auto iterId = (*iter).begin(); iterId != (*iter).end(); ++iterId)
		{
			if ((*iterId) == id)
			{
				iter->erase(iterId);
				break;
			}
		}

}

void TConstraintCoincide::BuildpDpt()
{
	switch (Element1PointIndex)
	{
	case 0:
	case 1:
		pDpt1 = &(pElement1->dpt);
		pElement1->vecIsJoint[0].push_back(id);
		break;
	case 2:
		pDpt1 = &(((TRealLine *)this->pElement1)->ptEnd);
		pElement1->vecIsJoint[1].push_back(id);
		break;
	}

	switch (Element2PointIndex)
	{
	case 0:
	case 1:
		pDpt2 = &(pElement2->dpt);
		pElement2->vecIsJoint[0].push_back(id);
		break;
	case 2:
		pDpt2 = &(((TRealLine *)this->pElement2)->ptEnd);
		pElement2->vecIsJoint[1].push_back(id);
		break;
	}
}

void TConstraintCoincide::NoticeListView(TListView *pListView)
{
	pListView->DeleteAllItems();

	TCHAR buffer[32];

	pListView->id = id;
	pListView->AddAttributeItem(TEXT("ID"), CTRLTYPE_NULL,NULL, TEXT("%d"), id);
	pListView->AddAttributeItem(TEXT("����"), CTRLTYPE_EDIT,&Name, Name);
	pListView->AddAttributeItem(TEXT("����"), CTRLTYPE_NULL, NULL, TEXT("�غ�"));
	pListView->AddAttributeItem(TEXT("����"), CTRLTYPE_NULL, NULL, GetLineStyleName(this->logpenStyle.lopnStyle, buffer));
	pListView->AddAttributeItem(TEXT("�߿�"), CTRLTYPE_NULL, NULL, TEXT("%d"), this->logpenStyle.lopnWidth);
	pListView->AddAttributeItem(TEXT("��ɫ"), CTRLTYPE_NULL, NULL, TEXT("0x%X"), this->logpenStyle.lopnColor);

	wsprintf(buffer, TEXT("ID:%d.P%d = ID:%d.P%d"), pElement1->id, Element1PointIndex, pElement2->id, Element2PointIndex);

	pListView->AddAttributeItem(TEXT("Value"), CTRLTYPE_NULL, NULL, buffer);
}