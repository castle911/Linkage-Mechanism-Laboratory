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

const String TBar::GetElementTypeName()//�õ���������
{
	return TEXT("����");
}

//��֪ͨlistview����realline֪ͨ

void TBar::Draw(HDC hdc, const TConfiguration* pConfig)
{
	TDraw::DrawBar(hdc, this, pConfig);
	//if (bDrawSquare) DrawPickSquare(hdc, pConfig);
}

TBar& TBar::operator=(const TRealLine &RealLine)
{
	TRealLine::operator=(RealLine);
	return *this;
}