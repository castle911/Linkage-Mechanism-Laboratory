#include <stdio.h>

#include "TElement.h"

#include "TListView.h"
TElement::TElement()
{
	eType = ELEMENT_ELEMENT;
	_tcscpy(Name,TEXT("undefined"));
}

TElement::~TElement()
{
	
}

void TElement::SetStyle(int iStyle, int iWidth, COLORREF crColor)
{
	logpenStyle.lopnStyle = iStyle;
	logpenStyle.lopnWidth.x = iWidth;
	logpenStyle.lopnColor = crColor;

	logpenStyleShow = logpenStyle;
}

TCHAR * TElement::GetLineStyleName(UINT linestyle, TCHAR name[])
{
	switch (linestyle)
	{
	case PS_SOLID:
		_tcscpy(name, TEXT("ʵ��"));
		break;
	case PS_DASH:
		_tcscpy(name, TEXT("�߶�"));
		break;
	case PS_DOT:
		_tcscpy(name, TEXT("����"));
		break;
	case PS_DASHDOT:
		_tcscpy(name, TEXT("�㻮��"));
		break;
	case PS_DASHDOTDOT:
		_tcscpy(name, TEXT("˫�㻮��"));
		break;
	}
	return name;
}