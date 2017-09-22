#pragma once
#include "tchar_head.h"
#include <stdio.h>

#include "TDraw.h"
#include "TLine.h"

TLine::TLine() :dAngle(angle)
{
	TLine::dLength = 0;
	_tcscpy(Name, TEXT("TLine"));
}

TLine::~TLine()
{
}

const String TLine::GetElementTypeName()//�õ���������
{
	return TEXT("��Ļ��");
}

TLine& TLine::operator=(const TLine &Line)
{
	TElement::operator=(Line);
	ptBegin = Line.ptBegin;
	ptEnd = Line.ptEnd;
	dLength = Line.dLength;
	return *this;
}