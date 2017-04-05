#pragma once
#include "MyMath.h"

#include <Windows.h>
#include "DPOINT.h"


enum units { UNITS_PX, UNITS_MM, UNITS_INCH };
class TConfiguration
{
private:
	POINT Org;
	double dProportion = 1.0;;
public:
	int FRAMEPOINT_TRANS_R=5;//��͸��״̬�ڵ�Բ�뾶
	int FRAMEPOINT_R = 6;//�ڵ�Բ�뾶
	int FRAMEPOINT_H = 20;//��-Բ�ĵ����߾���
	int FRAMEPOINT_B = 30;//�ױ߳�
	int FRAMEPOINT_SECTION_H = 10;//����������߶�
	int FRAMEPOINT_ANGLE = int(DEG2REG(60.0));//�����λ���
	int SLIDER_B = 45;//�����
	int SLIDER_H = 30;//�����
	int BAR_R = 7;//�˼�Բ�ǰ뾶
	bool bDrawReal=true;
	double dAnglePrecision = 1e-1;
	units uUnits = UNITS_MM;
	LOGPEN logpen;
	int iStyle = PS_SOLID;
	int iWidth = 1;
	LOGPEN logpenSystem;
	COLORREF crBackground, crPen,crDash,crDot,crCoordinate;
	COLORREF crGridBig, crGridSmall;
	COLORREF crLink;

	//Graph
	COLORREF crGraphBackground,crGraphGridBig, crGraphGridSmall;
	double DPUX,DPUY;
	double DPMX, DPMY;
	double DPIX, DPIY;
	TConfiguration();
	~TConfiguration();
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

