#pragma once
#include "tchar_head.h"
#include <stdio.h>

#include "TBar.h"

TBar::TBar()
{
	TBar::dLength = 0;
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_BAR;
}

TBar::~TBar()
{
}

//��֪ͨlistview����realline֪ͨ