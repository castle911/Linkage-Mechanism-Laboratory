#pragma once
#include "TRealLine.h"

class TBar :public TRealLine
{
public:
	TBar();
	~TBar();
	virtual const TCHAR * TBar::GetElementTypeName(TCHAR name[]) override;//�õ���������
};

