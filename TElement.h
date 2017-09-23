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

enum EnumElementClass{
	ELEMENT_CLASS_FRAME,
	ELEMENT_CLASS_NORMAL,
	ELEMENT_CLASS_CONSTRAINT,
	ELEMENT_CLASS_DRIVER,
	ELEMENT_CLASS_NONE
};

class TConfiguration;
class TShape;
class TListView;
class TElement
{
	friend void RefreshDOF(TElement *pElement, int &nb, int &iCoincideNum, int &iDriverNum, int &iFrameNum, bool isAdd);
private:
protected:
	virtual bool TElement::InSelWindow(RECT rect, const TConfiguration *pConfig){ return false; }
	virtual bool TElement::InSelCross(RECT rect, const TConfiguration *pConfig){ return false; }
public:
	int id;
	bool available;//δ����
	EnumElementType eType;//����
	EnumElementClass eClass;
	TCHAR Name[64];//����
	LOGPEN logpenStyleShow, logpenStyle;//��ʾ��ʽ��������ʽ

	std::vector<DPOINT> vecDpt;//�㼯
	std::vector<std::vector<int>> vecIsJoint;

	DPOINT dpt;
	double angle;

	bool bDrawSquare;
	unsigned char alpha;

	TElement();
	virtual ~TElement();
	virtual const String TElement::GetElementTypeName()=0;//�õ���������
	virtual bool TElement::WriteFile(HANDLE &hf, DWORD &now_pos)=0;
	virtual bool TElement::ReadFile(HANDLE &hf, DWORD &now_pos, TShape *pShape)=0;
	virtual void TElement::Draw(HDC hdc, const TConfiguration* pConfig) = 0;
	virtual void TElement::DrawPickSquare(HDC hdc, const TConfiguration* pConfig) = 0;
	virtual void TElement::NoticeListView(TListView *pListView) = 0;
	virtual bool TElement::IsAttached(DPOINT dptNowPos){ return false; };
	virtual bool TElement::Picked(const POINT &ptPos, const TConfiguration *pConfig){ return false; }
	virtual bool TElement::InSelect(RECT rect, bool bSelCross, const TConfiguration *pConfig);

	virtual const DPOINT TElement::GetRelativePointByIndex(int PointIndexOfElement) const;
	virtual DPOINT TElement::GetAbsolutePointByIndex(int PointIndexOfElement) const;
	virtual void TElement::ChangePos(DPOINT dptDelta);
	virtual void TElement::SetPhi(double phi);

	TElement& TElement::operator=(const TElement &element);

	//Element���к���
	virtual void TElement::SetX(double x) final;
	virtual void TElement::SetY(double y) final;
	virtual void TElement::SetStateNormal() final;
	virtual void TElement::SetStateHover() final;
	virtual void TElement::SetStateUnHover() final;
	virtual void TElement::SetStateChosen() final;

	virtual bool TElement::IsConstraint()const final{ return eClass == ELEMENT_CLASS_CONSTRAINT; }
	virtual bool TElement::CanBeDragged()const final{ return eClass == ELEMENT_CLASS_NORMAL; }

	virtual void TElement::SetStyle(const LOGPEN &logpen) final;//������ʽ
	virtual void TElement::SetStyle(TConfiguration *pConfig) final;//������ʽ
	virtual void TElement::SetColor(COLORREF cr) final;
	virtual void TElement::SetLineWidth(LONG width) final;
};

	const String GetLineStyleName(UINT linestyle);//�õ���������
	DPOINT GetAbsolute(const DPOINT &dpt, const DPOINT &Org, double angle);
	void RegularRect(RECT &rect);
	bool RectInRect(const RECT &rcBig, const RECT &rcSmall);
	bool RectCrossRect(const RECT &rcBig, const RECT &rcSmall);
	bool APointInRect(const RECT &rect, POINT *apt, int count);
	bool APointCrossRect(const RECT &rect, POINT *apt, int count);
	bool VecPointInRect(const RECT &rect, std::vector<POINT> &vecpt);
	bool VecPointCrossRect(const RECT &rect, std::vector<POINT> &vecpt);
	void RefreshDOF(TElement *pElement, int &nb,int &iCoincideNum,int &iDriverNum, int &iFrameNum,bool isAdd);