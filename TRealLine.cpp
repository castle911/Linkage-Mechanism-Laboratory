#include "TRealLine.h"

#include "TListView.h"
TRealLine::TRealLine()
{
	TRealLine::dLength = 0;
}

TRealLine::~TRealLine()
{

}

void TRealLine::NoticeListView(TListView *pListView)
{
	pListView->DeleteAllItems();

	TCHAR buffer[16];
	pListView->AddAttributeItem(TEXT("����"), TEXT("1"));
	pListView->AddAttributeItem(TEXT("����"), TEXT("��"));
	pListView->AddAttributeItem(TEXT("����"), GetLineStyleName(this->logpenStyle.lopnStyle, buffer));
	pListView->AddAttributeItem(TEXT("�߿�"), TEXT("%d"), this->logpenStyle.lopnWidth);
	pListView->AddAttributeItem(TEXT("��ɫ"), TEXT("0x%X"), this->logpenStyle.lopnColor);
	pListView->AddAttributeItem(TEXT("P1"), TEXT("%.3f,%.3f"), ptBegin.x, ptBegin.y);
	pListView->AddAttributeItem(TEXT("P2"), TEXT("%.3f,%.3f"), ptEnd.x, ptEnd.y);
	pListView->AddAttributeItem(TEXT("����"), TEXT("%f"), dLength);
}

void TRealLine::CalcLength()
{
	dLength = sqrt((ptEnd.x - ptBegin.x)*(ptEnd.x - ptBegin.x) + (ptEnd.y - ptBegin.y)*(ptEnd.y - ptBegin.y));
}

void TRealLine::SetPoint(DPOINT ptBegin, DPOINT ptEnd)
{
	TRealLine::ptBegin = ptBegin;
	TRealLine::ptEnd = ptEnd;
	CalcLength();
}

void TRealLine::SetPoint(DPOINT ptBegin, double dLength, double dAngle)
{
	TRealLine::ptBegin = ptBegin;
	TRealLine::ptEnd.x = ptBegin.x+dLength*cos(dAngle);
	TRealLine::ptEnd.y = ptBegin.y+dLength*sin(dAngle);
	this->dLength = dLength;
}