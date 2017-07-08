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
	//pDpt[0] = NULL;
	//pDpt[1] = NULL;
	_tcscpy(Name, TEXT(""));
}


TConstraintCoincide::~TConstraintCoincide()
{
}

const TCHAR * TConstraintCoincide::GetElementTypeName(TCHAR name[])//�õ���������
{
	return _tcscpy(name, TEXT("�غ�"));
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
	//pDpt[i] = &(pElement[i]->GetRelativePointByIndex(PointIndexOfElement[i]));
	pElement[i]->vecIsJoint[PointIndexOfElement[i]].push_back(id);
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
	return pElement[index]->GetAbsolutePointByIndex(PointIndexOfElement[index]);
}

void TConstraintCoincide::NoticeListView(TListView *pListView)
{
	pListView->DeleteAllItems();

	TCHAR buffer[32];

	pListView->id = id;
	pListView->AddAttributeItem(TEXT("ID"), CTRLTYPE_NULL, NULL, TEXT("%d"), id);
	pListView->AddAttributeItem(TEXT("����"), CTRLTYPE_EDIT, &Name, Name);
	pListView->AddAttributeItem(TEXT("����"), CTRLTYPE_NULL, NULL,GetElementTypeName(buffer));
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