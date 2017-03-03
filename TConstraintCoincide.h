#pragma once
#include "TElement.h"

class TListView;
class TConstraintCoincide :
	public TElement
{
public:
	TElement *pElement1, *pElement2;
	DPOINT *pDpt1, *pDpt2;
	int Element1PointIndex, Element2PointIndex;//0:���� 1,2:��
	TConstraintCoincide();
	~TConstraintCoincide();
	void TConstraintCoincide::NoticeListView(TListView *pListView);
	void TConstraintCoincide::BuildpDpt();
	void TConstraintCoincide::RestorePointStyle();
};

