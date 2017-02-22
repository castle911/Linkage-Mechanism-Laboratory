#pragma once
#include "DetectMemoryLeak.h"

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

void TShape::AddCoincide(TConstraintCoincide &coincide,TConfiguration *pConfig)
{
	TConstraintCoincide *tempcoincide = new TConstraintCoincide;
	*tempcoincide = coincide;
	if (tempcoincide->id == -1)
		tempcoincide->id = iNextId;
	else
		iNextId = tempcoincide->id;

	tempcoincide->BuildpDpt();//����2��ָ���
	//tempcoincide->SetStyle(PS_DOT, 1, pConfig->crPen);

	Element.push_back(tempcoincide);
	iNextId++;
	iCoincideNum++;
}

TElement * TShape::AddRealLine(TRealLine &realline)
{
	TRealLine *tempRealLine = new TRealLine;
	*tempRealLine = realline;
	if (tempRealLine->id == -1)
		tempRealLine->id = iNextId;
	else
		iNextId = tempRealLine->id;
	Element.push_back(tempRealLine);
	iNextId++;

	return tempRealLine;
}

TElement * TShape::AddBar(TBar *bar)
{
	TBar *tempBar = new TBar;
	*tempBar = *bar;
	if (tempBar->id == -1)
		tempBar->id = iNextId;
	else
		iNextId = tempBar->id;
	Element.push_back(tempBar);
	iNextId++;

	nb++;

	return tempBar;
}

TElement * TShape::AddFramePoint(TFramePoint &framepoint)
{
	TFramePoint *tempFramePoint = new TFramePoint;
	*tempFramePoint = framepoint;
	if (tempFramePoint->id == -1)
		tempFramePoint->id = iNextId;
	else
		iNextId = tempFramePoint->id;
	Element.push_back(tempFramePoint);
	iNextId++;
	if (hasFrame == 0)//ֻ��һ��
	{
		hasFrame = 1;
		nb++;
	}

	return tempFramePoint;
}

TElement * TShape::AddSlideway(TSlideway *slideway)
{
	TSlideway *tempSlideway = new TSlideway;
	*tempSlideway = *slideway;
	if (tempSlideway->id == -1)
		tempSlideway->id = iNextId;
	else
		iNextId = tempSlideway->id;
	Element.push_back(tempSlideway);
	iNextId++;
	if (hasFrame == 0)//ֻ��һ��
	{
		hasFrame = 1;
		nb++;
	}

	return tempSlideway;
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
	delete Element[index];
	auto iter = Element.begin() + index;
	Element.erase(iter);

	//���αȶ� Լ�� ���Ƿ��б�ɾԪ�ص�ַ������ɾ��Լ��
	for (auto iter = Element.begin(); iter != Element.end();)
	{
		if ((*iter)->eType==CONSTRAINT_COINCIDE)
			if (((TConstraintCoincide *)*iter)->pElement1 == pDeleted ||
				((TConstraintCoincide *)*iter)->pElement2 == pDeleted
				)
			{
				iCoincideNum--;
				DeletedId.push_back((*iter)->id);

				delete *iter;
				iter=Element.erase(iter);
				continue;
			}
		iter++;
	}

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
	TElement *temp = element;
	switch (temp->eType)
	{
	case ELEMENT_BAR:
	case ELEMENT_REALLINE:
		*x = ((TBar *)temp)->ptBegin.x;
		*y = ((TBar *)temp)->ptBegin.y;
		*theta = ((TBar *)temp)->dAngle;
		break;
	case ELEMENT_FRAMEPOINT:
		*x = ((TFramePoint *)temp)->dpt.x;
		*y = ((TFramePoint *)temp)->dpt.y;
		*theta = 0;
		break;
	}
}

//SiP={xi'P,yi'P}
void TShape::GetSijP(TElement *element,DPOINT *SiP,DPOINT *SjP,int *i,int *j)
{
	TConstraintCoincide *pCoincide = (TConstraintCoincide *)element;
	*i = pCoincide->pElement1->id;
	*j = pCoincide->pElement2->id;
	//�ڵ�i
	switch (pCoincide->pElement1->eType)
	{
	case ELEMENT_BAR:
	case ELEMENT_REALLINE:
		if (pCoincide->Element1PointIndex == 1)//ptBegin
			*SiP = { 0, 0 };
		else
			*SiP = { ((TBar *)GetElementById(*i))->dLength, 0 };
		break;
	case ELEMENT_FRAMEPOINT:
		*SiP = { 0,0 };
		break;
	}

	//�ڵ�j
	switch (pCoincide->pElement2->eType)
	{
	case ELEMENT_BAR:
		if (pCoincide->Element2PointIndex == 1)//ptBegin
			*SjP = { 0, 0 };
		else
			*SjP = { ((TBar *)GetElementById(*j))->dLength, 0 };
		break;
	case ELEMENT_FRAMEPOINT:
		*SjP = { 0,0 };
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
	default:
		assert(0);
		break;
	}
}