#pragma once
#include <vector>
#include <Windows.h>

#include "String.h"

#include "DPOINT.h"

enum EnumElementType{
	ELEMENT_NULL,
	ELEMENT_REALLINE,
	ELEMENT_FRAMEPOINT,
	ELEMENT_BAR,
	ELEMENT_SLIDEWAY,
	ELEMENT_SLIDER,
	CONSTRAINT_COINCIDE,
	CONSTRAINT_COLINEAR,
	ELEMENT_POLYLINEBAR,
	DRIVER
};

class TConfiguration;
class TShape;
class TListView;
class TElement
{
private:
protected:
public:
	int id;
	bool available;//δ����
	EnumElementType eType;//����
	TCHAR Name[64];//����
	LOGPEN logpenStyleShow, logpenStyle;//��ʾ��ʽ��������ʽ

	std::vector<DPOINT> vecDpt;//�㼯
	std::vector<std::vector<int>> vecIsJoint;

	DPOINT dpt;
	double angle;

	unsigned char alpha;
	bool CanBeDragged;

	TElement();
	virtual ~TElement();
	void TElement::SetStyle(const LOGPEN &logpen);//������ʽ
	void TElement::SetStyle(TConfiguration *pConfig);//������ʽ
	TCHAR * TElement::GetLineStyleName(UINT linestyle, TCHAR name[]);//�õ���������
	virtual const TCHAR * TElement::GetElementTypeName(TCHAR name[]);//�õ���������
	TElement& TElement::operator=(const TElement &element);
	virtual void TElement::BuildpDpt();
	virtual bool TElement::WriteFile(HANDLE &hf, DWORD &now_pos);
	virtual bool TElement::ReadFile(HANDLE &hf, DWORD &now_pos,TShape *pShape);
	virtual void TElement::NoticeListView(TListView *pListView);
	virtual const DPOINT TElement::GetRelativePointByIndex(int PointIndexOfElement) const;
	virtual DPOINT TElement::GetAbsolutePointByIndex(int PointIndexOfElement) const;
	virtual void TElement::SetX(double x);
	virtual void TElement::SetY(double y);
	virtual void TElement::SetPhi(double phi);
	void TElement::SetColor(COLORREF cr);
	void TElement::SetLineWidth(LONG width);
};

DPOINT GetAbsolute(const DPOINT &dpt, const DPOINT &Org, double angle);