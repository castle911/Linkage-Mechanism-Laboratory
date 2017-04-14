#pragma once
#include "MyMath.h"

#include <vector>
#include <Windows.h>
#include "DPOINT.h"


enum units { UNITS_PX, UNITS_MM, UNITS_INCH };
class TConfiguration
{
private:

	double DPUX, DPUY;
	double DPMX, DPMY;
	double DPIX, DPIY;

	POINT Org;
	double dProportion = 1.0;
	int iStyle = PS_SOLID;
	int iWidth = 1;
	COLORREF PrevRandomColor;
	std::vector<COLORREF> vecColorLib;

	COLORREF crMouseLine;//��ɫ
	COLORREF crPen, crDash, crDot;

public:
	bool bDrawReal=true;
	bool bThemeDark = true;
	bool bRandomColor = true;
	bool bDrawGrid;
	int FRAMEPOINT_TRANS_R=5;//��͸��״̬�ڵ�Բ�뾶
	int FRAMEPOINT_R = 6;//�ڵ�Բ�뾶
	int FRAMEPOINT_H = 20;//��-Բ�ĵ����߾���
	int FRAMEPOINT_B = 30;//�ױ߳�
	int FRAMEPOINT_SECTION_H = 10;//����������߶�
	int FRAMEPOINT_ANGLE = int(DEG2REG(60.0));//�����λ���
	int SLIDER_B = 45;//�����
	int SLIDER_H = 30;//�����
	int BAR_R = 7;//�˼�Բ�ǰ뾶
	int CROSS_SIZE = 18;//���߳�

	double dAnglePrecision = 1e-1;
	units uUnits = UNITS_MM;
	LOGPEN logpen;//��ǰ����
	LOGPEN logpenFront;//���� �� 
	LOGPEN logpenAssist;//�����ߣ���ɫ����
	LOGPEN logpenMouseLine;//ͼ �������ߣ���ɫ����
	LOGPEN logpenGridSmall;
	LOGPEN logpenAssistLine;//�����ߣ���ɫ����

	COLORREF crFront;//ǰ��ɫ����ɫʵ��
	COLORREF crBackground, crCoordinate;
	COLORREF crGridBig, crGridSmall;

	COLORREF crLink;//FramePoint ��ɫ
	//Graph
	COLORREF crGraphBackground, crGraphGridBig, crGraphGridSmall;

	TConfiguration();
	~TConfiguration();
	void TConfiguration::SetTheme(bool bDark);

	LOGPEN TConfiguration::GetRandomColorLogpen();
	void TConfiguration::SetOrg(LONG x, LONG y)
	{
		Org.x = x;
		Org.y = y;
	}
	POINT TConfiguration::GetOrg()
	{
		return Org;
	}
	void TConfiguration::SetDPU(double Proportion);
	double TConfiguration::GetProportion();
	void TConfiguration::Initial(HWND hwnd);

	double TConfiguration::ScreenToRealX(LONG x);
	double TConfiguration::ScreenToRealY(LONG y);
	DPOINT TConfiguration::ScreenToReal(POINT pt);

	LONG TConfiguration::RealToScreenX(double x);
	LONG TConfiguration::RealToScreenY(double y);
	POINT TConfiguration::RealToScreen(DPOINT dpt) const;

	LONG TConfiguration::LengthToScreenX(double xlen);
	LONG TConfiguration::LengthToScreenY(double ylen);
	POINT TConfiguration::LengthToScreen(DPOINT dpt);

	double TConfiguration::ScreenToLengthX(LONG xpixel);
	double TConfiguration::ScreenToLengthY(LONG ypixel);
	DPOINT TConfiguration::ScreenToLength(POINT pt);
};

