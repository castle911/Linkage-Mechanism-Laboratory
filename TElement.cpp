#include "TElement.h"

#include "TListView.h"
TElement::TElement()
{
}


TElement::~TElement()
{
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