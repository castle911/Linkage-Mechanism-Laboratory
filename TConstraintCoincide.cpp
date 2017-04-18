#pragma once

#include "tchar_head.h"
#include <stdio.h>

#include "TDraw.h"

#include "TShape.h"
#include "TDraw.h"
#include "TConstraintCoincide.h"

#include "TListView.h"
#include "TRealLine.h"

TConstraintCoincide::TConstraintCoincide()
{
	eType = CONSTRAINT_COINCIDE;
	pElement[0] = NULL;
	pElement[1] = NULL;
	pDpt[0] = NULL;
	pDpt[1] = NULL;
	_tcscpy(Name, TEXT(""));
}


TConstraintCoincide::~TConstraintCoincide()
{
}

void TConstraintCoincide::RestorePointStyle()
{
	for (int i = 0; i < 2; i++)
		if (pElement[i] != NULL)
			for (auto iter = pElement[i]->vecIsJoint.begin(); iter != pElement[i]->vecIsJoint.end(); ++iter)
				for (auto iterId = (*iter).begin(); iterId != (*iter).end(); ++iterId)
				{
					if ((*iterId) == id)
					{
						iter->erase(iterId);
						break;
					}
				}

}

void TConstraintCoincide::BuildpDpt_inner(int i)
{
	switch (pElement[i]->eType)
	{
	case ELEMENT_SLIDER:
	case ELEMENT_POLYLINEBAR:
		pDpt[i] = &(pElement[i]->vecDpt[PointIndexOfElement[i]]);
		pElement[i]->vecIsJoint[PointIndexOfElement[i]].push_back(id);
		break;
	case ELEMENT_BAR:
	case ELEMENT_SLIDEWAY:
	case ELEMENT_FRAMEPOINT:
		switch (PointIndexOfElement[i])
		{
		case 0:
			pDpt[i] = &(pElement[i]->dpt);
			pElement[i]->vecIsJoint[0].push_back(id);
			break;
		case 1:
			pDpt[i] = &(((TRealLine *)this->pElement[i])->ptEnd);
			pElement[i]->vecIsJoint[1].push_back(id);
			break;
		}
		break;
	default:
		assert(0);
		break;
	}
}

//����pElement��PointIndexOfElement�õ�pDpt��������vecIsJoint
void TConstraintCoincide::BuildpDpt()
{
	for (int i = 0; i < 2; i++)//0-1
	{
		BuildpDpt_inner(i);
	}
}

DPOINT TConstraintCoincide::GetLinkDpt(int index)
{
	switch (pElement[index]->eType)
	{
	case ELEMENT_SLIDER:
	case ELEMENT_POLYLINEBAR:
		return TDraw::GetAbsolute(*pDpt[index], pElement[index]->dpt, pElement[index]->angle);
		break;
	case ELEMENT_BAR:
	case ELEMENT_SLIDEWAY:
	case ELEMENT_FRAMEPOINT:
		switch (PointIndexOfElement[index])
		{
		case 0:
			return pElement[index]->dpt;
		case 1:
			return ((TRealLine *)pElement)->ptEnd;
		}
		break;
	default:
		assert(0);
		break;
	}
}

void TConstraintCoincide::NoticeListView(TListView *pListView)
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

	wsprintf(buffer, TEXT("ID:%d.P%d = ID:%d.P%d"), pElement[0]->id, PointIndexOfElement[0], pElement[1]->id, PointIndexOfElement[1]);

	pListView->AddAttributeItem(TEXT("Value"), CTRLTYPE_NULL, NULL, buffer);
}

//����ʱpElement����id
bool TConstraintCoincide::WriteFile(HANDLE &hf, DWORD &now_pos)
{
	if (!TElement::WriteFile(hf, now_pos))
		return false;

	::WriteFile(hf, &(pElement[0]->id), sizeof(pElement[0]->id), &now_pos, NULL);
	now_pos += sizeof(pElement[0]->id);
	::WriteFile(hf, &(pElement[1]->id), sizeof(pElement[1]->id), &now_pos, NULL);
	now_pos += sizeof(pElement[1]->id);

	::WriteFile(hf, &(PointIndexOfElement[0]), sizeof(PointIndexOfElement[0]), &now_pos, NULL);
	now_pos += sizeof(PointIndexOfElement[0]);
	::WriteFile(hf, &(PointIndexOfElement[1]), sizeof(PointIndexOfElement[1]), &now_pos, NULL);
	now_pos += sizeof(PointIndexOfElement[1]);

	if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != 0)
		return false;
	else
		return true;
}

//����ʱ����pElement�б����id����ָ���Ӧ��Ԫ��
bool TConstraintCoincide::ReadFile(HANDLE &hf, DWORD &now_pos, TShape *pShape)
{
	TElement::ReadFile(hf, now_pos, pShape);

	int id0, id1;
	::ReadFile(hf, &id0, sizeof(id0), &now_pos, NULL);
	now_pos += sizeof(id0);
	::ReadFile(hf, &id1, sizeof(id1), &now_pos, NULL);
	now_pos += sizeof(id1);

	pElement[0] = pShape->GetElementById(id0);
	pElement[1] = pShape->GetElementById(id1);

	::ReadFile(hf, &(PointIndexOfElement[0]), sizeof(PointIndexOfElement[0]), &now_pos, NULL);
	now_pos += sizeof(PointIndexOfElement[0]);
	::ReadFile(hf, &(PointIndexOfElement[1]), sizeof(PointIndexOfElement[1]), &now_pos, NULL);
	now_pos += sizeof(PointIndexOfElement[1]);

	BuildpDpt();

	if (GetLastError() != 0)
		return false;
	else
		return true;
}