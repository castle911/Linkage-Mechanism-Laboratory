#pragma once
#include "DetectMemoryLeak.h"

#include "TSlider.h"

#include "TShape.h"
#include "TConfiguration.h"

#include "TConstraintCoincide.h"

TShape::TShape()
{
	iNextId = 0;
	nb = 0;
	hasFrame = false;
	iCoincideNum = 0;
}

TShape::~TShape()
{
	ReleaseAll();
}

TElement * TShape::GetElementById(int id)
{
	for (int i = 0; i < Element.size(); i++)
	{
		if (Element[i]->id == id)
			return Element[i];
	}
}

int TShape::CalcFrameNum()
{
	int num = 0;
	for (int i = 0; i < Element.size(); i++)
	{
		if (Element[i]->eType == ELEMENT_FRAMEPOINT)
			num++;
	}
	return num;
}

std::vector<int> TShape::DeleteElement(int index)
{
	//ˢ�¹�������������
	switch (Element[index]->eType)
	{
	case ELEMENT_BAR:
		nb--;
		break;
	case ELEMENT_FRAMEPOINT:
		if (CalcFrameNum() == 1)//���һ������
		{
			hasFrame = false;
			nb--;
		}
		break;
	}

	std::vector<int> DeletedId = { Element[index]->id };

	TElement *pDeleted = Element[index];//�õ���ɾԪ�صĵ�ַ
	if (pDeleted->eType == CONSTRAINT_COINCIDE)
		((TConstraintCoincide *)pDeleted)->RestorePointStyle();

	//���αȶ� Լ�� ���Ƿ��б�ɾԪ�ص�ַ������ɾ��Լ��
	for (auto iter = Element.begin(); iter != Element.end();)
	{
		if ((*iter)->eType==CONSTRAINT_COINCIDE)//�����Լ��
			if (((TConstraintCoincide *)*iter)->pElement[0] == pDeleted ||
				((TConstraintCoincide *)*iter)->pElement[1] == pDeleted
				)//���漰����ɾ��Ԫ�أ���ɾ����Լ��
			{
				iCoincideNum--;
				DeletedId.push_back((*iter)->id);

				((TConstraintCoincide *)(*iter))->RestorePointStyle();
				delete *iter;
				iter=Element.erase(iter);
				continue;
			}
		iter++;
	}

	delete pDeleted;
	auto iter = Element.begin() + index;
	Element.erase(iter);

	return DeletedId;
}

void TShape::ReleaseAll()
{
	for (int i = 0; i < Element.size(); i++)
		delete Element[i];
	Element.swap(std::vector<TElement *>());
	iNextId = 0;
	nb = 0;
	hasFrame = false;
	iCoincideNum = 0;
}

int TShape::nc()
{
	return nb * 3;
}

int TShape::nh()
{
	return iCoincideNum * 2 + (hasFrame ? 3 : 0);
}

int TShape::DOF()
{
	return nc() - nh();
}

void TShape::GetCoordinateById(int id, double *x, double *y, double *theta)
{
	TElement *temp = GetElementById(id);
	GetCoordinateByElement(temp, x, y, theta);
}

void TShape::ChangePos(int index, DPOINT dptDelta)
{
	TElement *temp = Element[index];
	switch (temp->eType)
	{
	case ELEMENT_SLIDER:
	case ELEMENT_FRAMEPOINT:
		((TFramePoint *)temp)->dpt.x += dptDelta.x;
		((TFramePoint *)temp)->dpt.y += dptDelta.y;
		break;
	case ELEMENT_BAR:
	case ELEMENT_REALLINE:
	case ELEMENT_SLIDEWAY:
		((TRealLine *)temp)->ptBegin.x += dptDelta.x;
		((TRealLine *)temp)->ptBegin.y += dptDelta.y;
		((TRealLine *)temp)->ptEnd.x += dptDelta.x;
		((TRealLine *)temp)->ptEnd.y += dptDelta.y;
		break;
	default:
		assert(0);
		break;
	}
}

void TShape::GetCoordinateByIndex(int index, double *x, double *y, double *theta)
{
	GetCoordinateByElement(Element[index], x, y, theta);
}

void TShape::GetCoordinateByElement(TElement *element, double *x, double *y, double *theta)
{
	switch (element->eType)
	{
	case ELEMENT_BAR:
	case ELEMENT_REALLINE:
		*x = ((TBar *)element)->ptBegin.x;
		*y = ((TBar *)element)->ptBegin.y;
		*theta = ((TBar *)element)->dAngle;
		break;
	case ELEMENT_FRAMEPOINT:
		*x = element->dpt.x;
		*y = element->dpt.y;
		*theta = 0;
		break;
	case ELEMENT_SLIDER:
		*x = element->dpt.x;
		*y = element->dpt.y;
		*theta = element->angle;
		break;
	default:
		assert(0);
		break;
	}
}

//SiP={xi'P,yi'P}
void TShape::GetSijP(TElement *element,DPOINT *SiP,DPOINT *SjP,int *i,int *j)
{
	TConstraintCoincide *pCoincide = (TConstraintCoincide *)element;
	*i = pCoincide->pElement[0]->id;
	*j = pCoincide->pElement[1]->id;
	//�ڵ�i
	switch (pCoincide->pElement[0]->eType)
	{
	case ELEMENT_BAR:
	case ELEMENT_REALLINE:
		if (pCoincide->PointIndexOfElement[0] == 1)//ptBegin
			*SiP = { 0, 0 };
		else
			*SiP = { ((TBar *)GetElementById(*i))->dLength, 0 };
		break;
	case ELEMENT_FRAMEPOINT:
		*SiP = { 0,0 };
		break;
	case ELEMENT_SLIDER:
		*SiP = ((TSlider *)pCoincide->pElement[0])->vecDpt[pCoincide->PointIndexOfElement[0]];
		break;
	default:
		assert(0);
		break;
	}

	//�ڵ�j
	switch (pCoincide->pElement[1]->eType)
	{
	case ELEMENT_BAR:
	case ELEMENT_REALLINE:
		if (pCoincide->PointIndexOfElement[1] == 1)//ptBegin
			*SjP = { 0, 0 };
		else
			*SjP = { ((TBar *)GetElementById(*j))->dLength, 0 };
		break;
	case ELEMENT_FRAMEPOINT:
		*SjP = { 0,0 };
		break;
	case ELEMENT_SLIDER:
		*SjP = ((TSlider *)pCoincide->pElement[1])->vecDpt[pCoincide->PointIndexOfElement[1]];
		break;
	default:
		assert(0);
		break;
	}
}

DWORD TShape::GetSizeOfElement(EnumElementType eType)
{
	switch (eType)
	{
	case ELEMENT_BAR:
		return sizeof(TBar);
	case ELEMENT_REALLINE:
		return sizeof(TRealLine);
	case ELEMENT_FRAMEPOINT:
		return sizeof(TFramePoint);
	case ELEMENT_SLIDEWAY:
		return sizeof(TSlideway);
	case CONSTRAINT_COINCIDE:
		return sizeof(TConstraintCoincide);
	case ELEMENT_SLIDER:
		return sizeof(TSlider);
	default:
		assert(0);
		break;
	}
}