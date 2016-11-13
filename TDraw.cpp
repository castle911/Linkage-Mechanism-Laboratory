#pragma once
#include "TDraw.h"

#include "TFramePoint.h"

TDraw::TDraw()
{
}


TDraw::~TDraw()
{
}

//��ֱ��
void TDraw::DrawLine(HDC hdc, POINT ptFirstPos, POINT ptSecondPos)
{
	::MoveToEx(hdc, ptFirstPos.x, ptFirstPos.y, NULL);
	::LineTo(hdc, ptSecondPos.x, ptSecondPos.y);
}

void TDraw::DrawLine(HDC hdc, TLine Line)
{
	HPEN hPen;
	hPen = ::CreatePenIndirect(&Line.logpenStyle);
	::SelectObject(hdc, hPen);
	DrawLine(hdc, Line.ptBegin, Line.ptEnd);

	::DeleteObject(hPen);
}

void TDraw::DrawRealLine(HDC hdc, TRealLine RealLine,TConfiguration *Config)
{
	HPEN hPen;

	hPen = ::CreatePenIndirect(&RealLine.logpenStyle);
	::SelectObject(hdc, hPen);
	DrawLine(hdc, Config->RealToScreen(RealLine.ptBegin), Config->RealToScreen(RealLine.ptEnd));

	::DeleteObject(hPen);
}

//�����ܵ�
void TDraw::DrawFramePoint(HDC hdc, TFramePoint FramePoint, TConfiguration *Config)
{
	HPEN hPen;
	HBRUSH hBrush;
	hPen = ::CreatePenIndirect(&FramePoint.logpenStyle);
	hBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
	::SelectObject(hdc, hPen);
	::SelectObject(hdc, hBrush);

	const int r = 6;//�뾶
	const int h = 20;//��
	const int b = 30;//�ױ߳�
	const double angle = 60.0 / 180.0 * M_PI;
	POINT ptO = Config->RealToScreen(FramePoint.dpt);
	//��Բ
	::Ellipse(hdc, ptO.x - r, ptO.y - r, ptO.x + r, ptO.y + r);

	//����Բ�·���������
	POINT ptA1, ptB1;
	ptA1 = { ptO.x-r*sin(angle / 2), ptO.y+r*cos(angle / 2) };
	ptB1 = { ptO.x + r*sin(angle / 2), ptA1.y };

	//���������εĵױ�������
	POINT ptA2, ptB2;
	ptA2 = { ptO.x - h*tan(angle / 2), ptO.y + h };
	ptB2 = { ptO.x + h*tan(angle / 2), ptA2.y };
	DrawLine(hdc, ptA1, ptA2);
	DrawLine(hdc, ptB1, ptB2);

	//���õ�����
	POINT ptH1, ptH2;
	ptH1 = { ptO.x - b / 2, ptO.y + h };
	ptH2 = { ptO.x + b / 2, ptH1.y };
	DrawLine(hdc, ptH1, ptH2);

	//��������
	DrawSection(hdc, ptH1.x, ptH1.y, ptH2.x, ptH1.y + 10,6,45);

	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

//�������ߣ�dΪ��ֱ��࣬angleDEGΪ�Ƕ�
void TDraw::DrawSection(HDC hdc, int x1,int y1,int x2,int y2,int d, int angleDEG)
{
	double angle = angleDEG / 180.0*M_PI;
	int dx= d / sin(angle);
	int h = y2-y1;
	POINT pt1,pt2;
	pt1 = { x1 + dx, y1 };
	pt2 = { pt1.x - h / tan(angle), y2 };

	HRGN hRgn;
	hRgn = ::CreateRectRgn(x1,y1,x2,y2);
	::SelectClipRgn(hdc, hRgn);

	while (pt2.x < x2)
	{
		DrawLine(hdc, pt1, pt2);
		pt1.x += dx;
		pt2.x += dx;
	}
	
	::SelectClipRgn(hdc, NULL);
	::DeleteObject(hRgn);
}

//���㼯��ָ��ԭ����תtheta��Y��������Ϊ��
void TDraw::Rotate(POINT apt[], int apt_num, int Ox, int Oy, double theta)
{
	int x, y;
	for (int i = 0; i < apt_num; i++)
	{
		x = apt[i].x;
		y = apt[i].y;

		//��ƽ�Ƶ�ԭ��
		x -= Ox;
		y -= Oy;

		//��ת����ƽ�ƻ�ԭλ��
		apt[i].x =(LONG)( x*cos(theta) - y*sin(theta) + Ox);
		apt[i].y =(LONG)( x*sin(theta) + y*cos(theta) + Oy);
	}
}

//����pt�����ԭ��ptO�ĽǶȣ�Y��������Ϊ��
double GetAngleFromPoint(POINT ptO, POINT pt)
{
	double angle;
	if (((pt.x - ptO.x) > 0) && ((pt.y - ptO.y) >= 0))//��1����[0,PI/2)
		angle = atan(double(pt.y - ptO.y) / (pt.x - ptO.x));
	if (((pt.x - ptO.x) <= 0) && ((pt.y - ptO.y) > 0)) //��2����[PI/2,PI)
		angle = M_PI / 2 + atan(double(ptO.x - pt.x) / (pt.y - ptO.y));
	if (((pt.x - ptO.x) < 0) && ((pt.y - ptO.y) <= 0))//��3����[PI,PI*3/2)
		angle = M_PI + atan(double(ptO.y - pt.y) / (ptO.x - pt.x));
	if (((pt.x - ptO.x) >= 0) && ((pt.y - ptO.y) < 0))//��4����[PI*3/2,2*PI)
		angle = 3 / 2 * M_PI + atan(double(pt.x - ptO.x) / (ptO.y - pt.y));
	return angle;
}

//����pt�����ԭ��ptO�ĽǶȣ�Y��������Ϊ��
double TDraw::GetAngleFromPointReal(DPOINT ptO, DPOINT pt)
{
	double angle;
	if (((pt.x - ptO.x) >= 0) && ((pt.y - ptO.y) >= 0))//��1����[0,PI/2)
		angle = atan(double(pt.y - ptO.y) / (pt.x - ptO.x));
	if (((pt.x - ptO.x) <= 0) && ((pt.y - ptO.y) > 0)) //��2����[PI/2,PI)
		angle = M_PI / 2 + atan(double(ptO.x - pt.x) / (pt.y - ptO.y));
	if (((pt.x - ptO.x) < 0) && ((pt.y - ptO.y) <= 0))//��3����[PI,PI*3/2)
		angle = M_PI + atan(double(ptO.y - pt.y) / (ptO.x - pt.x));
	if (((pt.x - ptO.x) >= 0) && ((pt.y - ptO.y) < 0))//��4����[PI*3/2,2*PI)
		angle = 3.0 / 2.0 * M_PI + atan(double(pt.x - ptO.x) / (ptO.y - pt.y));
	return angle;
}

//����Ļ�㷵�ؽǶȣ�Y��������Ϊ��
double TDraw::GetAngleFromPointScreen(POINT pt0, POINT pt)
{
	pt0.y = -pt0.y;
	pt.y = -pt.y;
	return GetAngleFromPoint(pt0, pt);
}

//��X����Ϊ�Գ��᾵��
void TDraw::MirrorX(POINT apt[], int apt_num, int Oy)
{
	for (int i = 0; i < apt_num; i++)
	{
		apt[i].y = Oy - (apt[i].y - Oy);
	}
}

//���㼯apt��angle�����ƶ�dist���룬aptΪ��Ļ����
void TDraw::Move(POINT apt[],int apt_num, double angle, double dist)
{
	for (int i = 0; i < apt_num; i++)
	{
		apt[i].y = -apt[i].y;
		apt[i].x += dist*cos(angle);
		apt[i].y += dist*sin(angle);
		apt[i].y = -apt[i].y;
	}
}

//����ͷ��û����ʽ
//ʼĩ���Ϊ��Ļ����
void TDraw::DrawArrow(HDC hdc, POINT ptBegin, POINT ptEnd, int length, int width)
{
	POINT apt[3];
	int aptNum= 3;
	//�ȼ���ָ���յ㣬����ΪX�����ͷ���꣬����תtheta�ȡ�
	apt[0] = ptEnd;
	apt[1].x = ptEnd.x - length;
	apt[1].y = ptEnd.y + width / 2;
	apt[2].x = ptEnd.x - length;
	apt[2].y = ptEnd.y - width / 2;

	//���ڻ�ȡ�Ǽ���ת����ʹ�õѿ�������ϵ�����ȷ�ת��Ļ���꣬��������ٷ�ת��ȥ��
	TDraw::MirrorX(&ptBegin, 1, 0);
	TDraw::MirrorX(&ptEnd, 1, 0);
	TDraw::MirrorX(apt, aptNum, 0);

	TDraw::Rotate(apt, aptNum, ptEnd.x, ptEnd.y, GetAngleFromPoint(ptBegin, ptEnd));

	TDraw::MirrorX(&ptBegin, 1, 0);
	TDraw::MirrorX(&ptEnd, 1, 0);
	TDraw::MirrorX(apt, aptNum, 0);

	Polygon(hdc, apt, aptNum);
}

//�������᣺������ʽ
void TDraw::DrawAxes(HDC hdc, int Ox, int Oy, COLORREF crColor)
{
	int AxesLen = 40;
	POINT ptO;
	POINT ptXEnd, ptYEnd;
	ptO.x = Ox; ptO.y = Oy;
	ptXEnd.x = Ox + AxesLen; ptXEnd.y = Oy;
	ptYEnd.x = Ox; ptYEnd.y = Oy - AxesLen;

	HBRUSH hBrush = CreateSolidBrush(crColor);
	HPEN hPen = CreatePen(PS_SOLID, 1, crColor);
	SelectObject(hdc, hBrush);
	SelectObject(hdc, hPen);
	TDraw::DrawArrow(hdc, ptO, ptXEnd, 20, 6);
	TDraw::DrawLine(hdc, ptO, ptXEnd);
	TDraw::DrawArrow(hdc, ptO, ptYEnd, 20, 6);
	TDraw::DrawLine(hdc, ptO, ptYEnd);
	DeleteObject(hBrush);
	DeleteObject(hPen);
}

//��X��
void TDraw::DrawCross(HDC hdc, POINT pt, int size, LOGPEN Style)
{
	HPEN hPen;

	hPen = CreatePenIndirect(&Style);
	SelectObject(hdc, hPen);

	::MoveToEx(hdc, pt.x - size / 2, pt.y - size / 2, NULL);
	::LineTo(hdc, pt.x + size / 2, pt.y + size / 2);
	::MoveToEx(hdc, pt.x + size / 2, pt.y - size / 2, NULL);
	::LineTo(hdc, pt.x - size / 2, pt.y + size / 2);

	::DeleteObject(hPen);
}

//������
void TDraw::FillRect(HDC hdc, RECT *rect, COLORREF crColor)
{
	HBRUSH hBrush;
	hBrush = ::CreateSolidBrush(crColor);
	::SelectObject(hdc, hBrush);
	::FillRect(hdc, rect, hBrush);
	::DeleteObject(hBrush);
}

//������
void TDraw::DrawGrid(HDC hdc, RECT rect, TConfiguration *Config)
{
	int minGrid = 10;
	int maxGrid = 40;

	DPOINT realGrid = {2,2};//��ʵ�����С2mm
	POINT screenGrid = Config->LengthToScreen(realGrid);

	while (screenGrid.x<minGrid || screenGrid.x>maxGrid)
	{
		if (screenGrid.x < minGrid)
		{
			realGrid.x *= 2;
			realGrid.y *= 2;
		}
		if (screenGrid.x > maxGrid)
		{
			realGrid.x /= 2;
			realGrid.y /= 2;
		}
		screenGrid = Config->LengthToScreen(realGrid);
	}

	//ԭ���С���Сȡ�࣬�õ������ʼƫ����
	int xOffset = Config->GetOrg().x % screenGrid.x;
	int yOffset = Config->GetOrg().y % screenGrid.y;

	//ԭ���ȥ����ƫ���������Ը��Ӵ�С�õ���ԭ��ĸ������ٶ�5ȡ�࣬�õ�ԭ�㵽��������������
	//�ø���ȡ�����õ�xBigOffset��Ӧ�����
	int xBigOffset = ((Config->GetOrg().x - xOffset) / screenGrid.x) % 5;
	if (xBigOffset < 0) xBigOffset = 5 + xBigOffset;
	int yBigOffset = ((Config->GetOrg().y - yOffset) / screenGrid.y) % 5;
	if (yBigOffset < 0) yBigOffset = 5 + yBigOffset;

	int x, y;
	x = xOffset;
	y = yOffset;
	TLine xLine;
	TLine yLine;
	xLine.SetStyle(PS_SOLID, 1, Config->crGridSmall);
	yLine.SetStyle(PS_SOLID, 1, Config->crGridSmall);
	for (int i = 0; i < rect.right / screenGrid.x; i++)
	{
		xLine.ptBegin = { x, rect.top };
		xLine.ptEnd = { x, rect.bottom };
		if (i % 5 == xBigOffset)
			xLine.logpenStyle.lopnColor = Config->crGridBig;
		else
			xLine.logpenStyle.lopnColor = Config->crGridSmall;
		TDraw::DrawLine(hdc, xLine);
		x += screenGrid.x;
	}

	for (int i = 0; i < rect.bottom / screenGrid.y; i++)
	{
		yLine.ptBegin = { rect.left,y  };
		yLine.ptEnd = { rect.right, y };
		if (i % 5 == yBigOffset)
			yLine.logpenStyle.lopnColor = Config->crGridBig;
		else
			yLine.logpenStyle.lopnColor = Config->crGridSmall;
		TDraw::DrawLine(hdc, yLine);
		y += screenGrid.y;
	}
}

double TDraw::Distance(POINT pt1, POINT pt2)
{
	return sqrt(double((pt1.x - pt2.x)*(pt1.x - pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y)));
}

void TDraw::DrawRect(HDC hdc, RECT &rect,LOGPEN &logpen)
{
	HPEN hPen = ::CreatePenIndirect(&logpen);
	::SelectObject(hdc, hPen);
	::Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
	::DeleteObject(hPen);
}

void TDraw::DrawPickSquare(HDC hdc,POINT pt)
{
	LOGPEN logpen;
	logpen.lopnStyle = PS_SOLID;
	logpen.lopnWidth = { 1, 0 };
	logpen.lopnColor = RGB(101, 101, 101);

	const int halfsize=5;
	RECT rect = { pt.x - halfsize, pt.y - halfsize, pt.x + halfsize, pt.y + halfsize };
	DrawRect(hdc, rect, logpen);
	rect = { rect.left + 1, rect.top + 1, rect.right - 1, rect.bottom - 1 };
	FillRect(hdc, &rect, RGB(0, 127, 255));
}

void TDraw::ClientPosToScreen(HWND hWnd, POINT *pt)
{
	RECT rect;
	::GetWindowRect(hWnd, &rect);
	pt->x += rect.left;
	pt->y += rect.top;
}