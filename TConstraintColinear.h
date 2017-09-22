#pragma once
#include "TElement.h"
class TConstraintColinear :
	public TElement
{
public:
	TConstraintColinear();
	~TConstraintColinear();
	TElement *pElement[2];
	int PointBeginIndexOfElement[2];//Pi�� Pj�� ��ţ�vecDpt��ţ�
	int PointEndIndexOfElement[2];//Qi�� Qj�� ��ţ�vecDpt��ţ�

	virtual const String TConstraintColinear::GetElementTypeName() override;//�õ���������
	virtual void TConstraintColinear::Draw(HDC hdc, const TConfiguration* pConfig) override;
	virtual void TConstraintColinear::DrawPickSquare(HDC hdc, const TConfiguration* pConfig) override;
	virtual void TConstraintColinear::NoticeListView(TListView *pListView) override;
	virtual bool TConstraintColinear::WriteFile(HANDLE &hf, DWORD &now_pos)override;
	virtual bool TConstraintColinear::ReadFile(HANDLE &hf, DWORD &now_pos, TShape *pShape)override;
	virtual void TConstraintColinear::ChangePos(DPOINT dptDelta)override{}
	virtual bool TConstraintColinear::Picked(const POINT &ptPos, const TConfiguration *pConfig)override;

	void TConstraintColinear::GetLinkDpt(int iLinkIndex,DPOINT &dpt1,DPOINT &dpt2)const;
};

