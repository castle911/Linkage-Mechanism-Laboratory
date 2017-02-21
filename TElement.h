#pragma once
#include <Windows.h>

#include "DPOINT.h"

enum EnumElementType{ELEMENT_NULL,
	ELEMENT_REALLINE,
	ELEMENT_FRAMEPOINT,
	ELEMENT_BAR,
	ELEMENT_SLIDEWAY,
	CONSTRAINT_COINCIDE};

class TListView;
class TElement
{
public:
	int id;
	bool available;//δ����
	EnumElementType eType;//����
	TCHAR Name[64];//����
	LOGPEN logpenStyleShow, logpenStyle;//��ʾ��ʽ��������ʽ

	DPOINT dpt;
	double angle;

	TElement();
	~TElement();
	void TElement::SetStyle(int iStyle, int iWidth, COLORREF crColor);//������ʽ
	virtual void TElement::NoticeListView(TListView *pListView){}//
	TCHAR * TElement::GetLineStyleName(UINT linestyle, TCHAR name[]);//�õ���������
	TCHAR * TElement::GetElementTypeName(TCHAR name[]);//�õ���������
};