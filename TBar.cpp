#pragma once
#include "tchar_head.h"
#include <stdio.h>

#include "TDraw.h"
#include "TBar.h"

TBar::TBar() 
{
	TBar::dLength = 0;
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_BAR;

	CanBeDragged = true;
}

TBar::~TBar()
{
}

const TCHAR * TBar::GetElementTypeName(TCHAR name[])//�õ���������
{
	return _tcscpy(name, TEXT("����"));
}

//��֪ͨlistview����realline֪ͨ

void TBar::Draw(HDC hdc, const TConfiguration* pConfig)
{
	TDraw::DrawBar(hdc, this, pConfig);
}