#pragma once
#include <vector>
#include <Windows.h>

#include "DPOINT.h"

enum EnumElementType{ELEMENT_NULL,
	ELEMENT_REALLINE,
	ELEMENT_FRAMEPOINT,
	ELEMENT_BAR,
	ELEMENT_SLIDEWAY,
	ELEMENT_SLIDER,
	CONSTRAINT_COINCIDE,
	CONSTRAINT_COLINEAR};

class TListView;
class TElement
{
public:
	int id;
	bool available;//δ����
	EnumElementType eType;//����
	TCHAR Name[64];//����
	LOGPEN logpenStyleShow, logpenStyle;//��ʾ��ʽ��������ʽ
	//bool *pbIsJoint;
	std::vector<DPOINT> vecDpt;//�㼯
	std::vector<std::vector<int>> vecIsJoint;

	DPOINT dpt;
	double angle;

	TElement();
	virtual ~TElement();
	void TElement::SetStyle(const LOGPEN &logpen);//������ʽ
	void TElement::SetStyle(int iStyle, int iWidth, COLORREF crColor);//������ʽ
	TCHAR * TElement::GetLineStyleName(UINT linestyle, TCHAR name[]);//�õ���������
	TCHAR * TElement::GetElementTypeName(TCHAR name[]);//�õ���������
	virtual void TElement::NoticeListView(TListView *pListView){}//
};