#pragma once
#include "TElement.h"
class TPolylineBar :
	public TElement
{
public:
	TPolylineBar();
	~TPolylineBar();
	virtual const TCHAR * TPolylineBar::GetElementTypeName(TCHAR name[]) override;//�õ���������
	void TPolylineBar::NoticeListView(TListView *pListView) override;
};

