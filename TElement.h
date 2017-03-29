#pragma once
#include "DetectMemoryLeak.h"
#include <vector>
#include <Windows.h>

#include "String.h"

#include "DPOINT.h"

enum EnumElementType{ELEMENT_NULL,
	ELEMENT_REALLINE,
	ELEMENT_FRAMEPOINT,
	ELEMENT_BAR,
	ELEMENT_SLIDEWAY,
	ELEMENT_SLIDER,
	CONSTRAINT_COINCIDE,
	CONSTRAINT_COLINEAR};

class TShape;
class TListView;
class TElement
{
private:
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

	TElement();
	virtual ~TElement();
	void TElement::SetStyle(const LOGPEN &logpen);//������ʽ
	void TElement::SetStyle(int iStyle, int iWidth, COLORREF crColor);//������ʽ
	TCHAR * TElement::GetLineStyleName(UINT linestyle, TCHAR name[]);//�õ���������
	TCHAR * TElement::GetElementTypeName(TCHAR name[]);//�õ���������
	TElement& TElement::operator=(const TElement &element);
	virtual void TElement::NoticeListView(TListView *pListView){}//
	virtual void TElement::BuildpDpt(){ assert(0); }
	virtual bool TElement::WriteFile(HANDLE &hf, DWORD &now_pos);
	virtual bool TElement::ReadFile(HANDLE &hf, DWORD &now_pos,TShape *pShape);
};