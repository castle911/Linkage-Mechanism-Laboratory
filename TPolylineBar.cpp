#include "tchar_head.h"
#include "MyMath.h"
#include "TPolylineBar.h"

#include "TConfiguration.h"
#include "TDraw.h"
#include "TListView.h"

TPolylineBar::TPolylineBar()
{
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_POLYLINEBAR;

	CanBeDragged = true;
}


TPolylineBar::~TPolylineBar()
{
}

const TCHAR * TPolylineBar::GetElementTypeName(TCHAR name[])//�õ���������
{
	return _tcscpy(name, TEXT("��θ�"));
}

void TPolylineBar::NoticeListView(TListView *pListView)
{
	TElement::NoticeListView(pListView);

	TCHAR buffer[16];

	pListView->AddAttributeItem(TEXT("ԭ��"), CTRLTYPE_COOR_EDIT, &dpt, TEXT("%.3f,%.3f"), dpt.x, dpt.y);
	pListView->AddAttributeItem(TEXT("�Ƕ�"), CTRLTYPE_ANGLE_VALUE_EDIT, &angle, TEXT("%f"), REG2DEG(angle));

	for (size_t i = 0; i < vecDpt.size(); ++i)
	{
		_stprintf(buffer, TEXT("P%d"), i);
		pListView->AddAttributeItem(buffer, CTRLTYPE_COOR_EDIT, &(vecDpt[i]), TEXT("%.3f,%.3f"), vecDpt[i].x, vecDpt[i].y);
	}
}

void TPolylineBar::Draw(HDC hdc, const TConfiguration* pConfig)
{
	TDraw::DrawPolylineBar(hdc, this, pConfig);
}

void TPolylineBar::DrawPickSquare(HDC hdc, const TConfiguration* pConfig)
{
	for (auto &Dpt: vecDpt)
		//��ʰȡ����
		TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(TDraw::GetAbsolute(Dpt,dpt, angle)));
}

bool TPolylineBar::Picked(const POINT &ptPos, const TConfiguration *pConfig)
{
	return TDraw::PointInPolylineBar(ptPos, this, pConfig);
}