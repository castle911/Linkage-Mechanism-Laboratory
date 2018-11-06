#pragma once
#include "..\Common\DetectMemoryLeak.h"
#include "..\Common\MyMath.h"
#include "..\Common\tchar_head.h"

#include <set>
#include <map>
#include <algorithm>

#include <stdexcept>

#include "..\tompng.h"


#include "..\Draw\TDraw.h"

#include "..\Draw\TImage.h"

#include "TDrawTranslucent.h"
#include "..\TConfiguration.h"

#include "..\Element\TBar.h"
#include "..\Element\TSlider.h"
#include "..\Element\TSlideway.h"
#include "..\Element\TFramePoint.h"
#include "..\Element\TConstraintCoincide.h"
#include "..\Element\TConstraintColinear.h"
#include "..\Element\TPolylineBar.h"

#include "TOctree.h"

TDraw::TDraw()
{
	Gdiplus::GdiplusStartupInput gdiInput;
	Gdiplus::GdiplusStartup(&gdiplusStartupToken, &gdiInput, NULL);
}


TDraw::~TDraw()
{
	Gdiplus::GdiplusShutdown(gdiplusStartupToken);
}

//�÷���
//Image * pImage = NULL;
//ImageFromIDResource(IDR_PNG_NO_PIC, L"png", pImage);
//delete pImage;
BOOL TDraw::ImageFromIDResource(HINSTANCE hInst, UINT nID, LPCTSTR sType, Gdiplus::Image *&pImg)
{
	HRSRC hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(nID), sType); // type  
	if (!hRsrc)
		return FALSE;
	// load resource into memory  
	DWORD len = SizeofResource(hInst, hRsrc);
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
	if (!lpRsrc)
		return FALSE;
	// Allocate global memory on which to create stream  
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
	memcpy(pmem, lpRsrc, len);
	IStream* pstm;
	CreateStreamOnHGlobal(m_hMem, FALSE, &pstm);
	// load from stream  
	pImg = Gdiplus::Image::FromStream(pstm);
	// free/release stuff  
	GlobalUnlock(m_hMem);
	pstm->Release();
	FreeResource(lpRsrc);
	return TRUE;
}

void TDraw::DrawLogo(HINSTANCE hInst, UINT nID, LPCTSTR sType, HDC hdc, const RECT &ClientRect)
{
	/********************************************
	* step 1.
	* Using Gdiplus to load the image
	********************************************/

	Gdiplus::Image *image = NULL;
	if (ImageFromIDResource(hInst, nID, sType, image))
	{
		UINT height = image->GetHeight();
		UINT width = image->GetWidth();

		HDC memDC = ::CreateCompatibleDC(hdc);
		HBITMAP memBitmap = ::CreateCompatibleBitmap(hdc, width, height);
		::SelectObject(memDC, memBitmap);

		Gdiplus::Graphics graphics(memDC);
		graphics.DrawImage(image, 0, 0, width, height);
		///*********************************************
		//* step 3.
		//* Use UpdateLayeredWindow to Draw the Window
		//*
		//*********************************************/
		BLENDFUNCTION blendFunction;
		blendFunction.AlphaFormat = AC_SRC_ALPHA;
		blendFunction.BlendFlags = 0;
		blendFunction.BlendOp = AC_SRC_OVER;
		blendFunction.SourceConstantAlpha = 0x40;
		AlphaBlend(hdc, ClientRect.right - width, 0, width, height, memDC, 0, 0, width, height, blendFunction);
		::DeleteDC(memDC);
		::DeleteObject(memBitmap);

		delete image;
	}
}

//��ֱ�ߣ�������ʽ
void TDraw::DrawLine(HDC hdc, POINT ptFirstPos, POINT ptSecondPos)
{
	::MoveToEx(hdc, ptFirstPos.x, ptFirstPos.y, NULL);
	::LineTo(hdc, ptSecondPos.x, ptSecondPos.y);
}

//��ֱ�ߣ�����ʽ
void TDraw::DrawLine(HDC hdc, POINT ptFirstPos, POINT ptSecondPos, const LOGPEN &logpen)
{
	HPEN hPen;
	hPen = ::CreatePenIndirect(&logpen);
	::SelectObject(hdc, hPen);
	DrawLine(hdc, ptFirstPos, ptSecondPos);

	::DeleteObject(hPen);
}

//��ֱ�ߣ�����ʽ����logpenStyleΪ׼
void TDraw::DrawLine(HDC hdc, TLine Line)
{
	HPEN hPen;
	hPen = ::CreatePenIndirect(&Line.logpenStyle);
	::SelectObject(hdc, hPen);
	DrawLine(hdc, Line.ptBegin, Line.ptEnd);

	::DeleteObject(hPen);
}

//��ֱ�ߣ�����ʽ
void TDraw::DrawPolyline(HDC hdc, const POINT *apt, int count, LOGPEN &logpen)
{
	HPEN hPen;
	hPen = ::CreatePenIndirect(&logpen);
	::SelectObject(hdc, hPen);
	Polyline(hdc, apt, count);

	::DeleteObject(hPen);
}

void TDraw::DrawBar(HDC hdc, TBar *pBar, const TConfiguration *pConfig)
{
	if (pConfig->bDrawReal)
		DrawBarTranslucent(hdc, pBar, pConfig);
	else
		DrawBarSimple(hdc, pBar, pConfig);
}

//������������˵��ڲ�dist��λ���������ԭ�˵�
void TDraw::CalcBarLineEndpoint(POINT &ptBegin, POINT &ptEnd, int distBegin, int distEnd)
{
	double theta1 = GetAngleFromPointScreen(ptBegin, ptEnd);
	double theta2 = GetAngleFromPointScreen(ptEnd, ptBegin);

	ptBegin = { LONG(ptBegin.x + distBegin*cos(theta1)), LONG(ptBegin.y - distBegin*sin(theta1)) };
	ptEnd = { LONG(ptEnd.x + distEnd*cos(theta2)), LONG(ptEnd.y - distEnd*sin(theta2)) };
}

void TDraw::DrawBarSimple(HDC hdc, TBar *Bar, const TConfiguration *pConfig)
{
	HPEN hPen;
	HBRUSH hBrush;
	hPen = ::CreatePenIndirect(&(Bar->logpenStyleShow));
	hBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
	::SelectObject(hdc, hPen);
	::SelectObject(hdc, hBrush);

	POINT ptBegin, ptEnd;
	ptBegin = pConfig->RealToScreen(Bar->ptBegin);
	ptEnd = pConfig->RealToScreen(Bar->ptEnd);

	CalcBarLineEndpoint(ptBegin, ptEnd, Bar->vecIsJoint[0].size() > 0 ? pConfig->FRAMEPOINT_R : 0, Bar->vecIsJoint[1].size() > 0 ? pConfig->FRAMEPOINT_R : 0);
	DrawLine(hdc, ptBegin, ptEnd);

	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

void TDraw::DrawRealLine(HDC hdc, TRealLine &RealLine, const TConfiguration *Config)
{
	DrawRealLine(hdc, RealLine.ptBegin, RealLine.ptEnd, RealLine.logpenStyleShow, Config);
}

void TDraw::DrawRealLine(HDC hdc, DPOINT ptBegin, DPOINT ptEnd, LOGPEN logpen, const TConfiguration *Config)
{
	HPEN hPen;

	hPen = ::CreatePenIndirect(&logpen);
	::SelectObject(hdc, hPen);
	DrawLine(hdc, Config->RealToScreen(ptBegin), Config->RealToScreen(ptEnd));

	::DeleteObject(hPen);
}

void TDraw::DrawBarTranslucent(HDC hdc, TBar *pBar, const TConfiguration *pConfig)
{
	DrawBarTranslucent(hdc, pConfig->RealToScreen(pBar->ptBegin), pConfig->RealToScreen(pBar->ptEnd), pBar->angle, pBar->alpha, pBar->logpenStyleShow, pConfig);
}


	//�����ɫͼ������ѵ�ɫ��
/*ֱ�Ӵ���ָ�룬�����ڽ�new�����ݡ�delete�ɵ����߸���
���أ�λ��
e.g.
unsigned char *palette;
GetGifPaletteByHwnd(hWnd,palette);
delete[] palette;
*/
void TDraw::Create8TreePal(HWND hWnd, unsigned char *&palette, int &color_num, int &depth)
{   
	using namespace TOctree;
	int  i, j;
	BYTE *pbBits;
	BYTE r, g, b;
	NODE *pTree;
	UINT nLeafCount, nIndex;
	NODE *pReducibleNodes[9];

	TImage Image(hWnd);

	// Initialize octree variables
	pTree = NULL;                                       //�˲�����0����ʼ����
	nLeafCount = 0;
	for (i = 0; i <= (int)depth; i++) pReducibleNodes[i] = NULL;

	UINT32 data;
	for (int y = 0; y < Image.height; y++)
		for (int x = 0; x < Image.width; x++)
		{
			data = *((UINT32*)(Image.pvBits) + y*Image.width + x);
			b = (BYTE)data;
			g = (BYTE)(data >> 8);
			r = (BYTE)(data >> 16);
			AddColor(&pTree, r, g, b, depth, 0, &nLeafCount,
				pReducibleNodes);                 //��˲���������һ����ɫ
			while (nLeafCount > color_num)              //���ڵ���������ɫ�嵥Ԫ��
				ReduceTree(depth, &nLeafCount, pReducibleNodes); //���ٽڵ�
		}

	palette = new unsigned char[color_num * 3];

	nIndex = 0;
	GetPaletteColors(pTree, palette, &nIndex);
	DeleteTree(&pTree);
}

/*ֱ�Ӵ���ָ�룬�����ڽ�new�����ݡ�delete�ɵ����߸���
���أ�λ��
e.g. 
unsigned char *palette;
GetGifPaletteByHwnd(hWnd,palette);
delete[] palette;
*/
void TDraw::GetGifPaletteByHwnd(HWND hWnd, unsigned char *&palette,int &color_num,int &depth) 
{
	TImage Image(hWnd);

	typedef std::pair<UINT32, int> PAIR;

	std::map<UINT32,int> palette_32;

	UINT32 data;
	for (int y = 0; y < Image.height; y++)
		for (int x = 0; x < Image.width; x++)
		{
			data = *((UINT32*)(Image.pvBits) + y*Image.width + x);
			palette_32[data]++;
		}

	//����Ƶ������
	std::vector<PAIR> palette_sorted(palette_32.begin(), palette_32.end());//���Ƶ�vector
	std::sort(palette_sorted.begin(), palette_sorted.end(), [](const PAIR &lhs, const PAIR &rhs){return lhs.second > rhs.second; });
	
	//����λ��
	color_num = 256;
	depth = 8;
	while (palette_sorted.size() < color_num && depth!=1)
	{
		color_num /= 2;
		depth --;
	}

	//�ص�������ɫ
	if (palette_sorted.size() > color_num)
		palette_sorted.erase(palette_sorted.begin() + color_num, palette_sorted.end());
	else
		color_num = palette_sorted.size();

	//����RGBֵ����
	std::sort(palette_sorted.begin(), palette_sorted.end(), [](const PAIR &lhs, const PAIR &rhs){return lhs.first < rhs.first; });

	//д���ɫ��
		palette = new unsigned char[color_num*3];
		for (int i = 0; i < color_num; ++i)
		{
			palette[i * 3] = palette_sorted[i].first>>16;//r
			palette[i * 3+1] = palette_sorted[i].first>>8;//g
			palette[i * 3+2] = palette_sorted[i].first;//b
		}
}

//����palette�õ�����
//��palette���ֲ��ң��ҳ���data��ӽ���
unsigned char TDraw::GetIndexFromPalette(const UINT32 &data, const unsigned char *palette, const int &color_num)
{
	unsigned char low, high, mid(0);
	low = 0;
	high = color_num - 1;
	mid = (low + high) / 2;
	UINT32 palette_value;
	while (low < high)
	{
		palette_value = palette[mid * 3 + 2] & 0x000000FF;
		palette_value |= palette[mid * 3 + 1] << 8 & 0x0000FF00;
		palette_value |= palette[mid * 3] << 16;
		if (palette_value == data)
			return mid;
		if (data>palette_value)
			low = mid + 1;
		if (data<palette_value)
			high = mid - 1;
		mid = (low + high) / 2;
	}
	return mid;
}
//����palette�õ�����
//��palette���ֲ��ң��ҳ���data��ӽ���
unsigned char TDraw::GetIndexFromPalette(const UINT32 &data, const std::vector<unsigned int> &palette, const int &color_num)
{
	unsigned char low, high, mid(0);
	low = 0;
	high = color_num - 1;
	mid = (low + high) / 2;
	UINT32 palette_value;
	while (low < high)
	{
		palette_value = palette[mid];
		if (palette_value == data)
			return mid;
		if (data>palette_value)
			low = mid + 1;
		if (data<palette_value)
			high = mid - 1;
		mid = (low + high) / 2;
	}
	return mid;
}


//����palette�õ�����
unsigned char TDraw::GetIndexFromPalette_Slow(const UINT32 &data, const std::vector<unsigned int> &palette, const int &color_num)
{
	unsigned char index=0;
	int value=0x7fffffff,dist;
	int dist_r, dist_g, dist_b;
	for (int i = 0; i < color_num; ++i)
	{
		dist_b=int(data & 0xff) - (palette[i] & 0xff);
		dist_g= int(data >> 8 & 0xff) - (palette[i] >> 8 & 0xff);
		dist_r= int(data >> 16 & 0xff) - (palette[i] >> 16 & 0xff);
		dist = dist_r*dist_r + dist_g*dist_g + dist_b*dist_b;
		if (dist<value)
		{
			value=dist;
			index = i;
		}
	}
	return index;
}

bool TDraw::CaptureWindowToFile(HWND hWnd, TCHAR szFileName[])
{
	TImage Image(hWnd);

	//��hBitmap�����ݴ��
	unsigned char *rgb = new unsigned char[Image.width*Image.height * 3];
	unsigned char *p = rgb;
	unsigned x, y;
	FILE *fp = _wfopen(szFileName, TEXT("wb"));
	UINT32 data;
	for (y = 0; y < Image.height; y++)
		for (x = 0; x < Image.width; x++)
		{
			data = *((UINT32*)(Image.pvBits) + y*Image.width + x);//(height - 1 - y)
			*p++ = data >> 16;//R
			*p++ = data >> 8;//G
			*p++ = data;//B
		}
	tompng(fp, Image.width, Image.height, rgb, 0);
	fclose(fp);
	delete rgb;

	return true;
}

TDraw::HSB TDraw::RGB2HSB(int rgbR, int rgbG, int rgbB)
{
	int max = max(max(rgbR, rgbG), rgbB);
	int min = min(min(rgbR, rgbG), rgbB);

	float hsbB = max / 255.0f;
	float hsbS = max == 0 ? 0 : (max - min) / (float)max;

	float hsbH = 0;
	if (max == rgbR && rgbG >= rgbB) {
		hsbH = (rgbG - rgbB) * 60.0 / (max - min) + 0;
	}
	else if (max == rgbR && rgbG < rgbB) {
		hsbH = (rgbG - rgbB) * 60.0 / (max - min) + 360;
	}
	else if (max == rgbG) {
		hsbH = (rgbB - rgbR) * 60.0 / (max - min) + 120;
	}
	else if (max == rgbB) {
		hsbH = (rgbR - rgbG) * 60.0 / (max - min) + 240;
	}

	return{ hsbH, hsbS, hsbB };
}

COLORREF TDraw::HSB2RGB(float h, float s, float v)
{
	//assert Float.compare(h, 0.0f) >= 0 && Float.compare(h, 360.0f) <= 0;
	//assert Float.compare(s, 0.0f) >= 0 && Float.compare(s, 1.0f) <= 0;
	//assert Float.compare(v, 0.0f) >= 0 && Float.compare(v, 1.0f) <= 0;

	float r = 0, g = 0, b = 0;
	int i = ((int)(h / 60) % 6);
	float f = (h / 60) - i;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);
	switch (i) {
	case 0:
		r = v;
		g = t;
		b = p;
		break;
	case 1:
		r = q;
		g = v;
		b = p;
		break;
	case 2:
		r = p;
		g = v;
		b = t;
		break;
	case 3:
		r = p;
		g = q;
		b = v;
		break;
	case 4:
		r = t;
		g = p;
		b = v;
		break;
	case 5:
		r = v;
		g = p;
		b = q;
		break;
	default:
		break;
	}
	return RGB(r * 255.0, g * 255.0, b * 255.0);
}

//����ITU - R BT 601
//����ֵY = 0.299r + 0.587g + 0.114b
COLORREF TDraw::GetBrighterColor(COLORREF cr)
{
#define PLUS 0.2
	HSB hsb = RGB2HSB(GetRValue(cr), GetGValue(cr), GetBValue(cr));
	if (hsb.B + PLUS < 1.0)
		hsb.B += PLUS;
	else
		hsb.B = 1.0;

	return HSB2RGB(hsb.H, hsb.S, hsb.B);

	//byte y = (byte)(0.299*GetRValue(cr) + 0.587*GetGValue(cr) + 0.114*GetBValue(cr));
	//if (y + PLUS > 0xff)
	//	y = 0xff;
	//else
	//	y += PLUS;

	//return RGB(0.299 *  y / 256 * GetRValue(cr), 0.587 * y / 256 * GetGValue(cr), 0.114 * y / 256 * GetBValue(cr));
#undef PLUS
}

void TDraw::DrawBarTranslucent(HDC hdc, POINT &ptBegin, POINT &ptEnd, double angle, unsigned char alpha, LOGPEN logpen, const TConfiguration *pConfig)
{
	POINT pt[4];
	CalcBarRectCoor(pt, ptBegin, ptEnd, angle, pConfig->BAR_R * 2);
	//�������

	HPEN hPen = ::CreatePenIndirect(&logpen);
	::SelectObject(hdc, hPen);
	HBRUSH hBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
	::SelectObject(hdc, hBrush);

	DrawLine(hdc, pt[0], pt[1]);
	DrawLine(hdc, pt[2], pt[3]);
	Arc(hdc, ptBegin.x - pConfig->BAR_R, ptBegin.y - pConfig->BAR_R, ptBegin.x + pConfig->BAR_R, ptBegin.y + pConfig->BAR_R,
		pt[3].x, pt[3].y, pt[0].x, pt[0].y);
	Arc(hdc, ptEnd.x - pConfig->BAR_R, ptEnd.y - pConfig->BAR_R, ptEnd.x + pConfig->BAR_R, ptEnd.y + pConfig->BAR_R,
		pt[1].x, pt[1].y, pt[2].x, pt[2].y);

	::DeleteObject(hPen);
	::DeleteObject(hBrush);

	//�������Ҹ��Ӱ뾶
	int left = min(ptBegin.x, ptEnd.x) - pConfig->BAR_R, top = min(ptBegin.y, ptEnd.y) - pConfig->BAR_R;
	int width = abs(ptBegin.x - ptEnd.x) + 2 * pConfig->BAR_R, height = abs(ptBegin.y - ptEnd.y) + 2 * pConfig->BAR_R;

	TDrawTranslucent Translucent;
	Translucent.Start(hdc, alpha, left, top, width, height, logpen.lopnColor == 0);
	Translucent.Input(&ptBegin).Input(&ptEnd).Input(pt, 4);

	//��ʼ��
	hPen = (HPEN)::GetStockObject(NULL_PEN);
	::SelectObject(hdc, hPen);
	hBrush = CreateSolidBrush(logpen.lopnColor);
	::SelectObject(hdc, hBrush);

	//���������Բ
	::Ellipse(hdc, ptBegin.x - pConfig->BAR_R, ptBegin.y - pConfig->BAR_R, ptBegin.x + pConfig->BAR_R, ptBegin.y + pConfig->BAR_R);
	::Ellipse(hdc, ptEnd.x - pConfig->BAR_R, ptEnd.y - pConfig->BAR_R, ptEnd.x + pConfig->BAR_R, ptEnd.y + pConfig->BAR_R);

	Polygon(hdc, pt, 4);//���

	::DeleteObject(hPen);
	::DeleteObject(hBrush);
	//����

	Translucent.End();

}

void TDraw::CalcPolylineBarRgn(HRGN &hRgn, TPolylineBar *pPolylineBar, const TConfiguration *pConfig)
{
	CalcPolylineBarRgn(hRgn, pPolylineBar->vecDpt, pPolylineBar->dpt, pPolylineBar->angle, pConfig);
}


void TDraw::CalcPolylineBarRgn(HRGN &hRgn, const std::vector<DPOINT> &vecDpt, DPOINT dpt, double angle, const TConfiguration *pConfig)
{
	if (vecDpt.empty())
	{
		hRgn = NULL;
		return;
	}

	//�������תΪ��������
	std::vector<POINT> vecpt;
	GetAbsoluteScreen(vecpt, vecDpt, dpt, angle, pConfig);

	//��������
	HRGN hRgnTemp;
	hRgn = CreateEllipticRgn(vecpt[0].x - pConfig->BAR_R, vecpt[0].y - pConfig->BAR_R, vecpt[0].x + pConfig->BAR_R, vecpt[0].y + pConfig->BAR_R);

	POINT ptBar[4];
	for (auto iter = vecpt.begin() + 1; iter != vecpt.end(); ++iter)
	{
		hRgnTemp = CreateEllipticRgn(iter->x - pConfig->BAR_R, iter->y - pConfig->BAR_R, iter->x + pConfig->BAR_R, iter->y + pConfig->BAR_R);
		CombineRgn(hRgn, hRgn, hRgnTemp, RGN_OR);
		DeleteObject(hRgnTemp);

		CalcBarRectCoor(ptBar, *(iter - 1), *iter, GetAngleFromPointScreen(*(iter - 1), *iter), pConfig->BAR_R * 2);
		hRgnTemp = CreatePolygonRgn(ptBar, 4, ALTERNATE);
		CombineRgn(hRgn, hRgn, hRgnTemp, RGN_OR);
		DeleteObject(hRgnTemp);
	}
}

bool TDraw::PointInPolylineBar(POINT ptPos, TPolylineBar *pPolylineBar, const TConfiguration *pConfig)
{
	HRGN hRgn;
	CalcPolylineBarRgn(hRgn, pPolylineBar, pConfig);

	return (bool)PtInRegion(hRgn, ptPos.x, ptPos.y);
}

void TDraw::DrawPolylineBarSimple(HDC hdc, TPolylineBar *pPolylineBar, const TConfiguration *pConfig)
{
	if (pPolylineBar->vecDpt.empty())
	{
		return;
	}

	//�������תΪ��������
	std::vector<POINT> vecpt;
	GetAbsoluteScreen(vecpt, pPolylineBar->vecDpt, pPolylineBar->dpt, pPolylineBar->angle, pConfig);

	HPEN hPen = CreatePenIndirect(&(pPolylineBar->logpenStyleShow));
	SelectObject(hdc, hPen);

	//����
	size_t i = 0;
	POINT ptBegin, ptEnd;
	int distBegin, distEnd;
	int dist = 4;//����Բ�İ뾶��
	for (size_t i = 0; i < vecpt.size() - 1; ++i)//��1��-������2��
	{
		ptBegin = vecpt[i];
		ptEnd = vecpt[i + 1];
		if (!pPolylineBar->vecIsJoint[i].empty())
			if (i == 0)
				distBegin = pConfig->FRAMEPOINT_R;
			else
				distBegin = pConfig->FRAMEPOINT_R + dist;
		else
			distBegin = 0;
		if (!pPolylineBar->vecIsJoint[i + 1].empty())
			if (i == vecpt.size() - 2)//������2��
				distEnd = pConfig->FRAMEPOINT_R;
			else
				distEnd = pConfig->FRAMEPOINT_R + dist;
		else
			distEnd = 0;

		CalcBarLineEndpoint(ptBegin, ptEnd, distBegin, distEnd);

		DrawLine(hdc, ptBegin, ptEnd);
	}

	//����
	HBRUSH hBrush = CreateSolidBrush(pPolylineBar->logpenStyleShow.lopnColor);
	SelectObject(hdc, hBrush);

	for (size_t i = 1; i < vecpt.size() - 1; ++i)//��2��-������2��
	{
		if (!pPolylineBar->vecIsJoint[i].empty())//�н���
		{
			DrawArc(hdc, vecpt[i], pConfig->FRAMEPOINT_R + dist, vecpt[i - 1], vecpt[i + 1], true);
		}
		else
		{
			DrawPie(hdc, vecpt[i], pConfig->FRAMEPOINT_R + dist, vecpt[i - 1], vecpt[i + 1], true);
		}
	}

	DeleteObject(hBrush);

	DeleteObject(hPen);
}

void TDraw::DrawPolylineBar(HDC hdc, TPolylineBar *pPolylineBar, const TConfiguration *pConfig)
{
	if (pConfig->bDrawReal)
		DrawPolylineBarTranslucent(hdc, pPolylineBar, pConfig);
	else
		DrawPolylineBarSimple(hdc, pPolylineBar, pConfig);
}

void TDraw::DrawPolylineBarTranslucent(HDC hdc, TPolylineBar *pPolylineBar, const TConfiguration *pConfig)
{
	HRGN hRgn;
	CalcPolylineBarRgn(hRgn, pPolylineBar, pConfig);

	if (hRgn == NULL)
		return;

	HBRUSH hBrush;
	hBrush = CreateSolidBrush(pPolylineBar->logpenStyleShow.lopnColor);
	::SelectObject(hdc, hBrush);

	//���Ʊ���
	FrameRgn(hdc, hRgn, hBrush, 1, 1);

	//�õ���Χ��
	RECT rc;
	GetRgnBox(hRgn, &rc);
	SetMarginRect(&rc, -(pConfig->BAR_R + 1));

	//�����������Ƶ����Ͻ�λ��
	OffsetRgn(hRgn, -rc.left, -rc.top);

	TDrawTranslucent Translucent;
	Translucent.Start(hdc, pPolylineBar->alpha, rc, pPolylineBar->logpenStyleShow.lopnColor == 0);

	//�������
	FillRgn(hdc, hRgn, hBrush);

	DeleteObject(hRgn);
	::DeleteObject(hBrush);

	Translucent.End();
}

bool TDraw::PointInRgn(POINT *ptRgn, int RgnCount, POINT pt)
{
	HRGN hRgn = CreatePolygonRgn(ptRgn, RgnCount, ALTERNATE);
	bool bInRgn = (bool)PtInRegion(hRgn, pt.x, pt.y);

	DeleteObject(hRgn);

	return bInRgn;
	/*
		int   i, j = RgnCount - 1;
		bool  oddNodes = false;

		for (i = 0; i < RgnCount; i++)
		{
		if ((ptRgn[i].y < pt.y && ptRgn[j].y >= pt.y
		|| ptRgn[j].y < pt.y && ptRgn[i].y >= pt.y)
		&& (ptRgn[i].x <= pt.x || ptRgn[j].x <= pt.x))
		{
		oddNodes ^= (ptRgn[i].x + (pt.y - ptRgn[i].y) / (ptRgn[j].y - ptRgn[i].y)*(ptRgn[j].x - ptRgn[i].x) < pt.x);
		}
		j = i;
		}
		return oddNodes;*/
}

bool TDraw::PointInFramePoint(POINT ptFramePoint, POINT pt, const TConfiguration *pConfig)
{
	POINT FramePointRgn[6];
	FramePointRgn[0] = { ptFramePoint.x - pConfig->FRAMEPOINT_R, ptFramePoint.y - pConfig->FRAMEPOINT_R };
	FramePointRgn[1] = { ptFramePoint.x + pConfig->FRAMEPOINT_R, ptFramePoint.y - pConfig->FRAMEPOINT_R };
	FramePointRgn[5] = { ptFramePoint.x - pConfig->FRAMEPOINT_B / 2, ptFramePoint.y + pConfig->FRAMEPOINT_H };
	FramePointRgn[2] = { ptFramePoint.x + pConfig->FRAMEPOINT_B / 2, ptFramePoint.y + pConfig->FRAMEPOINT_H };
	FramePointRgn[4] = { ptFramePoint.x - pConfig->FRAMEPOINT_B / 2, ptFramePoint.y + pConfig->FRAMEPOINT_H + pConfig->FRAMEPOINT_SECTION_H };
	FramePointRgn[3] = { ptFramePoint.x + pConfig->FRAMEPOINT_B / 2, ptFramePoint.y + pConfig->FRAMEPOINT_H + pConfig->FRAMEPOINT_SECTION_H };

	return PointInRgn(FramePointRgn, 6, pt);
}

//������Բ ����ʽ
void TDraw::DrawCircle(HDC hdc, POINT pt, int r, LOGPEN logpen)
{
	HPEN hPen;
	HBRUSH hBrush;
	hPen = ::CreatePenIndirect(&logpen);
	hBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
	::SelectObject(hdc, hPen);
	::SelectObject(hdc, hBrush);

	::Ellipse(hdc, pt.x - r, pt.y - r, pt.x + r, pt.y + r);

	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

//��Բ û����ʽ
void TDraw::DrawCircle(HDC hdc, POINT pt, int r)
{
	::Ellipse(hdc, pt.x - r, pt.y - r, pt.x + r + 1, pt.y + r + 1);
}

//�����ܵ�
void TDraw::DrawFramePoint(HDC hdc, TFramePoint *pFramePoint, const TConfiguration *pConfig)
{
	POINT ptO = pConfig->RealToScreen(pFramePoint->dpt);

	//����Բ�·���������
	POINT ptA1, ptB1;
	if (!pFramePoint->vecIsJoint.empty() && pFramePoint->vecIsJoint[0].size() > 0)
	{
		ptA1 = { LONG(ptO.x - pConfig->FRAMEPOINT_R*sin(pConfig->FRAMEPOINT_ANGLE / 2.0)), LONG(ptO.y + pConfig->FRAMEPOINT_R*cos(pConfig->FRAMEPOINT_ANGLE / 2.0)) };
		ptB1 = { LONG(ptO.x + pConfig->FRAMEPOINT_R*sin(pConfig->FRAMEPOINT_ANGLE / 2.0)), LONG(ptA1.y) };
	}
	else
	{
		ptA1 = ptO;
		ptB1 = ptO;
	}

	//���������εĵױ�������
	POINT ptA2, ptB2;
	ptA2 = { LONG(ptO.x - pConfig->FRAMEPOINT_H*tan(pConfig->FRAMEPOINT_ANGLE / 2.0)), LONG(ptO.y + pConfig->FRAMEPOINT_H) };
	ptB2 = { LONG(ptO.x + pConfig->FRAMEPOINT_H*tan(pConfig->FRAMEPOINT_ANGLE / 2.0)), LONG(ptA2.y) };

	//���õ�����
	POINT ptH1, ptH2;
	ptH1 = { LONG(ptO.x - pConfig->FRAMEPOINT_B / 2.0), LONG(ptO.y + pConfig->FRAMEPOINT_H) };
	ptH2 = { LONG(ptO.x + pConfig->FRAMEPOINT_B / 2.0), LONG(ptH1.y) };


	if (pConfig->bDrawReal)//������ʾ
	{
		HPEN hPen = (HPEN)::GetStockObject(NULL_PEN);
		HBRUSH hBrush = CreateSolidBrush(pConfig->crLink);
		::SelectObject(hdc, hPen);
		::SelectObject(hdc, hBrush);

		POINT apt[3];
		apt[0] = ptO;
		apt[1] = ptA2;
		apt[2] = ptB2;
		Polygon(hdc, apt, 3);

		::DeleteObject(hPen);

		hPen = ::CreatePen(PS_SOLID, 2, pConfig->crLink);
		::SelectObject(hdc, hPen);
		DrawLine(hdc, ptH1, ptH2);

		::DeleteObject(hPen);
		::DeleteObject(hBrush);
	}
	else
	{
		HPEN hPen = ::CreatePenIndirect(&(pFramePoint->logpenStyleShow));
		HBRUSH hBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
		::SelectObject(hdc, hPen);
		::SelectObject(hdc, hBrush);

		DrawLine(hdc, ptA1, ptA2);
		DrawLine(hdc, ptB1, ptB2);

		DrawLine(hdc, ptH1, ptH2);

		//��������
		DrawSection(hdc, ptH1.x, ptH1.y, ptH2.x, ptH1.y + pConfig->FRAMEPOINT_SECTION_H, 6, 45);

		::DeleteObject(hPen);
		::DeleteObject(hBrush);
	}
}

void TDraw::DrawSlideway(HDC hdc, TSlideway *Slideway, const TConfiguration *pConfig)
{
	if (Slideway->SlidewayStyle == 0)
		DrawSlidewaySingle(hdc, Slideway->logpenStyleShow, Slideway->ptBegin, Slideway->ptEnd, Slideway->dAngle, Slideway->ShadowQuadrant, Slideway->ShadowLength, pConfig);
	else
	{
		DPOINT dptBegin[2], dptEnd[2];
		int ShadowQuadrant[2];
		int d_screen = pConfig->bDrawReal ? (pConfig->BAR_R + 4) : 6;
		double d = pConfig->ScreenToLengthX(d_screen);
		dptBegin[0] = { Slideway->ptBegin.x + d*sin(Slideway->dAngle), Slideway->ptBegin.y - d*cos(Slideway->dAngle) };
		dptEnd[0] = { Slideway->ptEnd.x + d*sin(Slideway->dAngle), Slideway->ptEnd.y - d*cos(Slideway->dAngle) };
		dptBegin[1] = { Slideway->ptBegin.x - d*sin(Slideway->dAngle), Slideway->ptBegin.y + d*cos(Slideway->dAngle) };
		dptEnd[1] = { Slideway->ptEnd.x - d*sin(Slideway->dAngle), Slideway->ptEnd.y + d*cos(Slideway->dAngle) };
		switch (Slideway->ShadowQuadrant)
		{
		case 1:ShadowQuadrant[0] = 4; ShadowQuadrant[1] = 1; break;
		case 2:ShadowQuadrant[0] = 3; ShadowQuadrant[1] = 2; break;
		case 3:ShadowQuadrant[0] = 3; ShadowQuadrant[1] = 2; break;
		case 4:ShadowQuadrant[0] = 4; ShadowQuadrant[1] = 1; break;
		}
		DrawSlidewaySingle(hdc, Slideway->logpenStyleShow, dptBegin[0], dptEnd[0], Slideway->dAngle, ShadowQuadrant[0], Slideway->ShadowLength, pConfig);
		DrawSlidewaySingle(hdc, Slideway->logpenStyleShow, dptBegin[1], dptEnd[1], Slideway->dAngle, ShadowQuadrant[1], Slideway->ShadowLength, pConfig);

	}
}

void TDraw::DrawSlidewaySingle(HDC hdc, const LOGPEN &logpen, const DPOINT &dptBegin, const DPOINT &dptEnd, double dAngle, int ShadowQuadrant, int ShadowLength, const TConfiguration *pConfig)
{
	HPEN hPen;
	HBRUSH hBrush;

	//���ʲ�ͬ
	if (pConfig->bDrawReal)
		hPen = ::CreatePen(PS_SOLID, 2, 0);
	else
		hPen = ::CreatePenIndirect(&logpen);

	hBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
	::SelectObject(hdc, hPen);
	::SelectObject(hdc, hBrush);

	POINT ptBegin = pConfig->RealToScreen(dptBegin);
	POINT ptEnd = pConfig->RealToScreen(dptEnd);

	double angleDEG = RAD2DEG(dAngle) + 45;
	POINT pt[4];
	switch (ShadowQuadrant)
	{
	case 1:
		pt[0] = ptEnd;
		pt[1] = { pt[0].x, pt[0].y + pConfig->FRAMEPOINT_SECTION_H };
		pt[2] = { pt[1].x - ShadowLength, pt[1].y };
		pt[3] = { pt[2].x, pt[0].y };
		break;
	case 2:
		pt[0] = ptBegin;
		pt[1] = { pt[0].x, pt[0].y + pConfig->FRAMEPOINT_SECTION_H };
		pt[2] = { pt[1].x + ShadowLength, pt[1].y };
		pt[3] = { pt[2].x, pt[0].y };
		break;
	case 3:
		pt[0] = ptBegin;
		pt[1] = { pt[0].x, pt[0].y - pConfig->FRAMEPOINT_SECTION_H };
		pt[2] = { pt[1].x + ShadowLength, pt[1].y };
		pt[3] = { pt[2].x, pt[0].y };
		break;
	case 4:
		pt[0] = ptEnd;
		pt[1] = { pt[0].x, pt[0].y - pConfig->FRAMEPOINT_SECTION_H };
		pt[2] = { pt[1].x - ShadowLength, pt[1].y };
		pt[3] = { pt[2].x, pt[0].y };
		break;
	default:
		assert(0);
		break;
	}

	DrawLine(hdc, ptBegin, ptEnd);
	Rotate(pt, 4, pt[0].x, pt[0].y, dAngle);
	MirrorX(pt, 4, pt[0].y);

	DrawSection(hdc, pt, 4, 10, angleDEG);

	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

//�õ��㼯�İ�Χ�� rectֵ�����������
void TDraw::GetBoundingBox(std::vector<POINT> &vecpt, RECT *rect)
{
	LONG xmin, ymin, xmax, ymax;
	if (vecpt.empty() == false)
	{
		xmax = xmin = vecpt[0].x;
		ymax = ymin = vecpt[0].y;
	}

	for (auto iter = vecpt.begin() + 1; iter != vecpt.end(); ++iter)
	{
		if (iter->x > xmax) xmax = iter->x;
		if (iter->x < xmin) xmin = iter->x;
		if (iter->y > ymax) ymax = iter->y;
		if (iter->y < ymin) ymin = iter->y;
	}
	rect->left = xmin;
	rect->right = xmax;
	rect->top = ymin;
	rect->bottom = ymax;
}

//�õ��㼯�İ�Χ�� rectֵ�����������
void TDraw::GetBoundingBox(std::vector<DPOINT> &vecdpt, RECT *rect)
{
	double xmin, ymin, xmax, ymax;
	if (vecdpt.empty() == false)
	{
		xmax = xmin = vecdpt[0].x;
		ymax = ymin = vecdpt[0].y;
	}

	for (auto iter = vecdpt.begin() + 1; iter != vecdpt.end(); ++iter)
	{
		if (iter->x > xmax) xmax = iter->x;
		if (iter->x < xmin) xmin = iter->x;
		if (iter->y > ymax) ymax = iter->y;
		if (iter->y < ymin) ymin = iter->y;
	}
	rect->left = (LONG)xmin;
	rect->right = (LONG)xmax;
	rect->top = (LONG)ymax;
	rect->bottom = (LONG)ymin;
}

//�õ��㼯�İ�Χ�� rectֵ�����������
void TDraw::GetBoundingBox(POINT apt[], int apt_num, RECT *rect, bool YPlusIsUP)
{
	int xmin, ymin, xmax, ymax;
	if (apt_num > 0)
	{
		xmax = xmin = apt[0].x;
		ymax = ymin = apt[0].y;
	}

	for (int i = 1; i < apt_num; i++)
	{
		if (apt[i].x > xmax) xmax = apt[i].x;
		if (apt[i].x < xmin) xmin = apt[i].x;
		if (apt[i].y > ymax) ymax = apt[i].y;
		if (apt[i].y < ymin) ymin = apt[i].y;
	}
	rect->left = xmin;
	rect->right = xmax;
	if (YPlusIsUP)
	{
		rect->top = ymax;
		rect->bottom = ymin;
	}
	else
	{
		rect->top = ymin;
		rect->bottom = ymax;
	}
}

//�������ߣ�dΪ��ֱ��࣬angleDEGΪ�Ƕȣ����������Y��������Ϊ��
void TDraw::DrawSection(HDC hdc, POINT apt[], int apt_num, int d, double angleDEG)
{
	HRGN hRgn;
	hRgn = ::CreatePolygonRgn(apt, apt_num, ALTERNATE);
	::SelectClipRgn(hdc, hRgn);

	RECT rect;
	GetBoundingBox(apt, apt_num, &rect, true);

	//��Χ�����Բֱ��
	double r = sqrt(pow(double(rect.right - rect.left), 2) + pow(double(rect.top - rect.bottom), 2)) / 2;

	POINT center = { (rect.right + rect.left) / 2, (rect.top + rect.bottom) / 2 };

	RECT nrect;
	nrect.left = LONG(center.x - r);
	nrect.right = LONG(center.x + r);
	nrect.top = LONG(center.y + r);
	nrect.bottom = LONG(center.y - r);

	double angle = DEG2RAD(angleDEG);

	POINT *pt1, *pt2;
	int pt_num = int(2 * r / d);
	pt1 = new POINT[pt_num];
	pt2 = new POINT[pt_num];

	for (int i = 0; i < pt_num; i++)
	{
		pt1[i].x = nrect.left;
		pt1[i].y = nrect.bottom + i*d;
		pt2[i].x = nrect.right;
		pt2[i].y = pt1[i].y;
	}

	Rotate(pt1, pt_num, center.x, center.y, angle);
	Rotate(pt2, pt_num, center.x, center.y, angle);
	MirrorX(pt1, pt_num, center.y);
	MirrorX(pt2, pt_num, center.y);

	for (int i = 0; i < pt_num; i++)
	{
		DrawLine(hdc, pt1[i], pt2[i]);
	}

	delete[] pt1, pt2;

	::SelectClipRgn(hdc, NULL);
	::DeleteObject(hRgn);
}

//�������ߣ�dΪ��ֱ��࣬angleDEGΪ�Ƕ�
void TDraw::DrawSection(HDC hdc, int x1, int y1, int x2, int y2, int d, double angleDEG)
{
	double angle = DEG2RAD(angleDEG);
	int dx = int(d / sin(angle));
	int h = y2 - y1;
	POINT pt1, pt2;
	pt1 = { x1 + dx, y1 };
	pt2 = { LONG(pt1.x - h / tan(angle)), y2 };

	HRGN hRgn;
	hRgn = ::CreateRectRgn(x1, y1, x2, y2);
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

//���㼯��ָ��ԭ����תtheta���������Y��������Ϊ��
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
		apt[i].x = (LONG)(x*cos(theta) - y*sin(theta) + Ox);
		apt[i].y = (LONG)(x*sin(theta) + y*cos(theta) + Oy);
	}
}

//����pt�����ԭ��ptO�ĽǶȣ��������Y��������Ϊ��
double GetAngleFromPoint(POINT ptO, POINT pt)
{
	double angle = 0;
	if (((pt.x - ptO.x) > 0) && ((pt.y - ptO.y) >= 0))//��1����[0,PI/2)
		angle = atan(double(pt.y - ptO.y) / (pt.x - ptO.x));
	if (((pt.x - ptO.x) <= 0) && ((pt.y - ptO.y) > 0)) //��2����[PI/2,PI)
		angle = M_PI / 2 + atan(double(ptO.x - pt.x) / (pt.y - ptO.y));
	if (((pt.x - ptO.x) < 0) && ((pt.y - ptO.y) <= 0))//��3����[PI,PI*3/2)
		angle = M_PI + atan(double(ptO.y - pt.y) / (ptO.x - pt.x));
	if (((pt.x - ptO.x) >= 0) && ((pt.y - ptO.y) < 0))//��4����[PI*3/2,2*PI)
		angle = 3.0 / 2.0 * M_PI + atan(double(pt.x - ptO.x) / (ptO.y - pt.y));
	return angle;
}

//���ؼнǣ���Ϊ��
double TDraw::GetAngleBetweenPointReal(const DPOINT &pt1, const DPOINT &ptO, const DPOINT &pt2)
{
	double angle = abs(GetAngleFromPointReal(ptO, pt1) - GetAngleFromPointReal(ptO, pt2));
	return angle > M_PI ? 2 * M_PI - angle : angle;
}

//����pt1-pt2�н�,��ʱ�뷽��
double TDraw::GetAngleBetweenPointScreen(const POINT &pt1, const POINT &ptO, const POINT &pt2)
{
	double angle = GetAngleFromPointScreen(ptO, pt2) - GetAngleFromPointScreen(ptO, pt1);
	return angle < 0 ? 2 * M_PI + angle : angle;
	//return GetAngleFromPointScreen(ptO, pt2) - GetAngleFromPointScreen(ptO, pt1);
}

//����pt�����ԭ��ptO�ĽǶȣ��������Y��������Ϊ��
double TDraw::GetAngleFromPointReal(DPOINT ptO, DPOINT pt)
{
	double angle = 0;
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

//����Ļ�㷵�ؽǶȣ��������Y��������Ϊ��
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

void TDraw::MoveByDelta(std::vector<POINT> &vecpt, long dx, long dy)
{
	for (auto &pt : vecpt)
	{
		pt.x += dx;
		pt.y += dy;
	}
}

void TDraw::MoveByDelta(POINT apt[], int apt_num, long dx, long dy)
{
	for (int i = 0; i < apt_num; i++)
	{
		apt[i].x += dx;
		apt[i].y += dy;
	}
}

void TDraw::MoveRect(RECT &rc, int left, int top)
{
	rc.left -= left;
	rc.right -= left;
	rc.top -= top;
	rc.bottom -= top;
}

//���㼯apt��angle�����ƶ�dist���룬aptΪ��Ļ����
void TDraw::Move(POINT apt[], int apt_num, double angle, double dist)
{
	for (int i = 0; i < apt_num; i++)
	{
		apt[i].y = -apt[i].y;
		apt[i].x += LONG(dist*cos(angle));
		apt[i].y += LONG(dist*sin(angle));
		apt[i].y = -apt[i].y;
	}
}

//����ͷ��û����ʽ
//ʼĩ���Ϊ��Ļ����
void TDraw::DrawArrow(HDC hdc, POINT ptBegin, POINT ptEnd, int length, int width)
{
	POINT apt[3];
	int aptNum = 3;
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
void TDraw::DrawGrid(HDC hdc, const RECT &rect, POINT ptOrg, COLORREF crGridBig, COLORREF crGridSmall, const TConfiguration *pConfig)
{
	int minGrid = 10;
	int maxGrid = 40;

	DPOINT realGrid = { 2, 2 };//��ʵ�����С2mm
	POINT screenGrid = pConfig->LengthToScreen(realGrid);

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
		screenGrid = pConfig->LengthToScreen(realGrid);
	}

	//ԭ���С���Сȡ�࣬�õ������ʼƫ����
	int xOffset = ptOrg.x % screenGrid.x;
	int yOffset = ptOrg.y % screenGrid.y;

	//ԭ���ȥ����ƫ���������Ը��Ӵ�С�õ���ԭ��ĸ������ٶ�5ȡ�࣬�õ�ԭ�㵽��������������
	//�ø���ȡ�����õ�xBigOffset��Ӧ�����
	int xBigOffset = ((ptOrg.x - xOffset) / screenGrid.x) % 5;
	if (xBigOffset < 0) xBigOffset = 5 + xBigOffset;
	int yBigOffset = ((ptOrg.y - yOffset) / screenGrid.y) % 5;
	if (yBigOffset < 0) yBigOffset = 5 + yBigOffset;

	int x, y;
	x = xOffset;
	y = yOffset;
	TLine xLine;
	TLine yLine;
	xLine.SetStyle(pConfig->logpenGridSmall);
	yLine.SetStyle(pConfig->logpenGridSmall);
	for (int i = 0; i < rect.right / screenGrid.x; i++)
	{
		xLine.ptBegin = { x, rect.top };
		xLine.ptEnd = { x, rect.bottom };
		if (i % 5 == xBigOffset)
			xLine.logpenStyle.lopnColor = crGridBig;
		else
			xLine.logpenStyle.lopnColor = crGridSmall;
		TDraw::DrawLine(hdc, xLine);
		x += screenGrid.x;
	}

	for (int i = 0; i < rect.bottom / screenGrid.y; i++)
	{
		yLine.ptBegin = { rect.left, y };
		yLine.ptEnd = { rect.right, y };
		if (i % 5 == yBigOffset)
			yLine.logpenStyle.lopnColor = crGridBig;
		else
			yLine.logpenStyle.lopnColor = crGridSmall;
		TDraw::DrawLine(hdc, yLine);
		y += screenGrid.y;
	}
}

double TDraw::Distance(POINT pt1, POINT pt2)
{
	return sqrt(double((pt1.x - pt2.x)*(pt1.x - pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y)));
}

double TDraw::Distance(DPOINT pt1, DPOINT pt2)
{
	return sqrt(double((pt1.x - pt2.x)*(pt1.x - pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y)));
}

double TDraw::DistanceScreen(const DPOINT &dpt1, const DPOINT &dpt2, const TConfiguration *pConfig)
{
	POINT p1 = pConfig->RealToScreen(dpt1);
	POINT p2 = pConfig->RealToScreen(dpt2);
	return Distance(p1, p2);
}

DPOINT TDraw::GetCenter(const DPOINT &pt1, const DPOINT &pt2)
{
	return{ (pt1.x + pt2.x) / 2, (pt1.y + pt2.y) / 2 };
}

POINT TDraw::GetCenter(const POINT &pt1, const POINT &pt2)
{
	return{ (pt1.x + pt2.x) / 2, (pt1.y + pt2.y) / 2 };
}

bool TDraw::ShowConstraintCoincideDotLine(TElement *element, const TConfiguration *pConfig)
{
	TConstraintCoincide *temp = (TConstraintCoincide *)element;

	//�Ƚ�p1,p2�������������
	if (DistanceScreen(temp->GetLinkDpt(0), temp->GetLinkDpt(1), pConfig) > 4)
		return true;
	else
		return false;
}

//��ʽ������������
void TDraw::DrawConstraintColinear_inner(HDC hdc, const POINT &pt1, const POINT &pt2, LOGPEN logpen, const TConfiguration *pConfig)
{

	POINT ptCenter1 = pt1, ptCenter2 = pt2;

	HPEN hPen = CreatePenIndirect(&(pConfig->logpenConstraintLine));
	SelectObject(hdc, hPen);

	DrawLine(hdc, pt1, ptCenter2);

	POINT ptBold1 = GetCenter(ptCenter1, ptCenter2);
	POINT ptBold2 = ptBold1;
	CalcBarLineEndpoint(ptCenter1, ptBold1, 0, pConfig->COLINEAR_SYMBOL_L / 2);
	CalcBarLineEndpoint(ptCenter2, ptBold2, 0, pConfig->COLINEAR_SYMBOL_L / 2);

	DeleteObject(hPen);

	hPen = CreatePenIndirect(&(pConfig->logpenColinearSymbol));
	SelectObject(hdc, hPen);

	DrawLine(hdc, ptBold1, ptBold2);

	DeleteObject(hPen);
}

//�Ƿ���ʾ����Լ���ߣ�����Լ��λ�ò����㣩
//ptCenter1,ptCenter2:����Ӧ����ʾ����Լ���ߵĵ�
bool TDraw::ShowConstraintColinearDotLine(const TConstraintColinear *pColinear, POINT &ptCenter1, POINT &ptCenter2, const TConfiguration *pConfig)
{
	DPOINT P1, Q1, P2, Q2;
	pColinear->GetLinkDpt(0, P1, Q1);
	pColinear->GetLinkDpt(1, P2, Q2);

	ptCenter1 = pConfig->RealToScreen(GetCenter(P1, Q1));
	ptCenter2 = pConfig->RealToScreen(GetCenter(P2, Q2));

	DPOINT dptIntersection;
	if (PointInRealLineOrExtension(P1, dptIntersection, P2, Q2, pConfig) != -1)//P1��P2 Q2��
		if (PointInRealLineOrExtension(Q1, dptIntersection, P2, Q2, pConfig) != -1)//Q1Ҳ��P2 Q2��
			return false;//P1Q1 P2Q2���ߣ�������

	return true;

}

void TDraw::DrawConstraintColinear(HDC hdc, TConstraintColinear *pColinear, const TConfiguration *pConfig)
{
	POINT ptCenter1, ptCenter2;

	if (ShowConstraintColinearDotLine(pColinear, ptCenter1, ptCenter2, pConfig))
		DrawConstraintColinear_inner(hdc, ptCenter1, ptCenter2, pColinear->logpenStyleShow, pConfig);
}

//�������غϻ��ƺ��� ��ʽ������������
void TDraw::DrawConstraintCoincide(HDC hdc, DPOINT dpt0, DPOINT dpt1, const LOGPEN &logpen, const TConfiguration *pConfig)
{
	if (DistanceScreen(dpt0, dpt1, pConfig) > 4)
	{
		//LOGPEN logpenNow = logpen;
		//logpenNow.lopnStyle = PS_DOT;
		DrawRealLine(hdc, dpt0, dpt1, pConfig->logpenConstraintLine, pConfig);
	}

	if (pConfig->bDrawReal)
	{
		HPEN hPen = (HPEN)::GetStockObject(NULL_PEN);
		HBRUSH hBrush = ::CreateSolidBrush(pConfig->crLink);//ʼ���Ǻ�ɫ
		::SelectObject(hdc, hPen);
		::SelectObject(hdc, hBrush);

		DrawCircle(hdc, GetCenter(pConfig->RealToScreen(dpt0), pConfig->RealToScreen(dpt1)), pConfig->FRAMEPOINT_TRANS_R);

		::DeleteObject(hPen);
		::DeleteObject(hBrush);
	}
	else
		DrawCircle(hdc, GetCenter(pConfig->RealToScreen(dpt0), pConfig->RealToScreen(dpt1)), pConfig->FRAMEPOINT_R, pConfig->logpenFront);
}

//�Դ���ʵ��Ⱦ
void TDraw::DrawConstraintCoincide(HDC hdc, TConstraintCoincide *pCoincide, const TConfiguration *pConfig)
{
	//�Ƚ�p1,p2�������������
	DPOINT dpt[2];
	dpt[0] = pCoincide->GetLinkDpt(0);
	dpt[1] = pCoincide->GetLinkDpt(1);

	DrawConstraintCoincide(hdc, dpt[0], dpt[1], pCoincide->logpenStyleShow, pConfig);
}

void TDraw::DrawRect(HDC hdc, const RECT &rect, const LOGPEN &logpen)
{
	HPEN hPen = ::CreatePenIndirect(&logpen);
	HBRUSH hBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
	::SelectObject(hdc, hBrush);
	::SelectObject(hdc, hPen);
	::Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

void TDraw::DrawRect(HDC hdc, const RECT &rect, const LOGPEN &logpen, COLORREF crBk)
{
	HPEN hPen = ::CreatePenIndirect(&logpen);
	HBRUSH hBrush = ::CreateSolidBrush(crBk);
	::SelectObject(hdc, hPen);
	::SelectObject(hdc, hBrush);
	::Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

void TDraw::DrawPickSquare(HDC hdc, POINT pt)
{
	LOGPEN logpen;
	logpen.lopnStyle = PS_SOLID;
	logpen.lopnWidth = { 1, 0 };
	logpen.lopnColor = RGB(101, 101, 101);

	int size = 10;
	RECT rect = { pt.x - size / 2, pt.y - size / 2, pt.x + size / 2, pt.y + size / 2 };
	DrawRect(hdc, rect, logpen, RGB(0, 127, 255));
}

void TDraw::ClientPosToScreen(HWND hWnd, POINT *pt)
{
	RECT rect;
	::GetWindowRect(hWnd, &rect);
	pt->x += rect.left;
	pt->y += rect.top;
}

//���ݱ�Ե������rect marginΪ������С
void TDraw::SetMarginRect(RECT *rect, int margin)
{
	rect->left += margin;
	rect->top += margin;
	rect->right -= margin;
	rect->bottom -= margin;
}

//���ݱ�Ե������rect��������ԭ����rect marginΪ������С
RECT TDraw::GetMarginRect(RECT rect, int margin)
{
	RECT rc = rect;
	SetMarginRect(&rc, margin);
	return rc;
}

//���ݱ�Ե������rect��������ԭ����rect marginΪ������С
//right,bottom��ʾ���� ��������
RECT TDraw::GetMarginCtrlRect(const RECT &rect, int margin)
{
	return{ rect.left + margin, rect.top + margin, rect.right - 2 * margin, rect.bottom - 2 * margin };
}

//���ݱ�Ե������rect��������ԭ����rect,marginΪ�� ����С
RECT TDraw::GetMarginRect(RECT rect, LONG margin_left, LONG margin_top, LONG margin_right, LONG margin_bottom)
{
	return{ rect.left + margin_left, rect.top + margin_top, rect.right - margin_right, rect.bottom - margin_bottom };
}

int TDraw::DPOINT2POINTXLEN(double x1, double x2, double x_min, double x_max, const RECT &rect)
{
	return DPOINT2POINTX(x2, x_min, x_max, rect) - DPOINT2POINTX(x1, x_min, x_max, rect);
}

int TDraw::DPOINT2POINTYLEN(double y1, double y2, double y_min, double y_max, const RECT &rect)
{
	return DPOINT2POINTY(y1, y_min, y_max, rect) - DPOINT2POINTY(y2, y_min, y_max, rect);
}

int TDraw::DPOINT2POINTX(double x, double x_min, double x_max, const RECT &rect)
{
	return int(rect.left + (rect.right - rect.left)*(x - x_min) / (x_max - x_min));
}

int TDraw::DPOINT2POINTY(double y, double y_min, double y_max, const RECT &rect)
{
	return int(rect.top + (rect.bottom - rect.top)*(y_max - y) / (y_max - y_min));
}

POINT TDraw::DPOINT2POINT(DPOINT &dpt, double x_min, double x_max, double y_min, double y_max, const RECT &rect)
{
	//int X = int(rect.left + (rect.right - rect.left)*(dpt.x - x_min) / (x_max - x_min));
	//int Y = int(rect.top + (rect.bottom - rect.top)*(y_max - dpt.y) / (y_max - y_min));
	return{ DPOINT2POINTX(dpt.x, x_min, x_max, rect), DPOINT2POINTY(dpt.y, y_min, y_max, rect) };
}

DPOINT TDraw::POINT2DPOINT(POINT &pt, double x_min, double x_max, double y_min, double y_max, const RECT &rect)
{
	double x = x_min + (x_max - x_min)*(pt.x - rect.left) / (rect.right - rect.left);
	double y = y_max - (y_max - y_min)*(pt.y - rect.top) / (rect.bottom - rect.top);
	return{ x, y };
}

void TDraw::MakeRect(RECT &rcResult, double x_min, double x_max, double y_min, double y_max, const TConfiguration *pConfig)
{
	rcResult.top = pConfig->RealToScreenY(y_max);
	rcResult.bottom = pConfig->RealToScreenY(y_min);
	rcResult.left = pConfig->RealToScreenX(x_min);
	rcResult.right = pConfig->RealToScreenX(x_max);
}

void TDraw::GetCenter(POINT &ptResult, const RECT &rect)
{
	ptResult.x = (rect.right + rect.left) / 2;
	ptResult.y = (rect.top + rect.bottom) / 2;
}

void TDraw::DrawTextAdvance(HDC hdc, const TCHAR text[], RECT *rect, long FontSize, int FontWeight, unsigned long color, const TCHAR FontName[], UINT format, int cEscapement, int cOrientation)
{
	long lfHeight;
	HFONT hf;
	lfHeight = -MulDiv(FontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	hf = CreateFont(lfHeight, 0, cEscapement, cOrientation, FontWeight, 0, 0, 0, 0, 0, 0, 0, 0, FontName);
	SelectObject(hdc, hf);
	SetTextColor(hdc, color);
	DrawText(hdc, text, -1, rect, format);
	DeleteObject(hf);
}

void TDraw::DrawTips(HDC hdc, POINT &ptMouse, const RECT &rcLimited, const TCHAR text[], TConfiguration *pConfig)
{
	DrawAdjustedText(hdc, ptMouse, rcLimited, text, 22, true, pConfig);
}


void TDraw::DrawAdjustedText(HDC hdc, POINT &ptMouse, const RECT &rcLimited, const TCHAR text[], LONG dist, bool DrawBorder, TConfiguration *pConfig)
{
	if (_tcslen(text) == 0)
		return;
	RECT rc, rcBk;
	rc.left = ptMouse.x + dist;
	rc.top = ptMouse.y + dist;

	DrawSystemFontText(hdc, text, rc, pConfig->logpenFront.lopnColor, DT_CALCRECT);//���ử������ֻ������ˢ��rc��
	LONG width = rc.right - rc.left;
	LONG height = rc.bottom - rc.top;
	rcBk = rc;
	SetMarginRect(&rcBk, -5);//����5px

	if (rcBk.right > rcLimited.right)
	{
		MoveRect(rc, dist * 2 + width, 0);
		MoveRect(rcBk, dist * 2 + width, 0);
	}
	if (rcBk.bottom > rcLimited.bottom)
	{
		MoveRect(rc, 0, dist * 2 + height);
		MoveRect(rcBk, 0, dist * 2 + height);
	}

	if (DrawBorder) DrawRect(hdc, rcBk, pConfig->logpenFront, pConfig->crBackground);
	DrawSystemFontText(hdc, text, rc, pConfig->logpenFront.lopnColor, DT_LEFT | DT_TOP);//DT_CALCRECT
}

POINT TDraw::GetSystemFontSize(HDC hdc, const TCHAR text[])
{
	RECT rc = { 0, 0, 0, 0 };
	DrawSystemFontText(hdc, text, rc, 0, DT_CALCRECT);//���ử������ֻ������ˢ��rc��
	return{ rc.right, rc.bottom };
}

void TDraw::DrawSystemFontText(HDC hdc, const TCHAR text[], RECT &rect, COLORREF color, UINT format)
{
	DrawTextAdvance(hdc, text, &rect, 9, 400, color, TEXT("����"), format);
}

void TDraw::DrawSystemFontTextVertical(HDC hdc, const TCHAR text[], RECT &rect, COLORREF color, UINT format)
{
	DrawTextAdvance(hdc, text, &rect, 9, 400, color, TEXT("����"), format, 900, 900);
}

//�жϵ��Ƿ�λ��ֱ�߻�ֱ�ߵ��ӳ����� 
//����ֵ��-1������ 0���߶��� 1��pt1һ���ӳ��� 2��pt2һ���ӳ���
//������ݲ�������dptIntersection��Ϊ�������꣬���򲻱�
//�ݲ����pConfig������
int TDraw::PointInRealLineOrExtension(const DPOINT &dptPos, DPOINT &dptIntersection, const DPOINT dptBegin, const DPOINT dptEnd, const TConfiguration *pConfig)
{
	if (Distance(dptBegin, dptEnd) < precision)//�����غ�
	{
		return -1;
	}

	//���㴹��
	DPOINT dptCalc;

	double x, y;
	double angle = TDraw::GetAngleFromPointReal(dptBegin, dptEnd);
	if (CanMod(angle + M_PI_2, M_PI, 1e-6))
	{
		x = dptBegin.x;
		y = dptPos.y;
	}
	else
	{
		double k1 = tan(angle);
		double k2 = (abs(k1) < 1e-6) ? M_PI_2 : -1.0 / k1;
		double x1 = dptBegin.x;
		double y1 = dptBegin.y;
		double x2 = dptEnd.x;
		double y2 = dptEnd.y;

		double x0 = dptPos.x;
		double y0 = dptPos.y;

		x = (y0 - y1 - k2*x0 + k1*x1) / (k1 - k2);
		y = k1*x + y1 - k1*x1;
	}
	dptCalc = { x, y };

	double dist = pConfig->LengthToScreenX(Distance(dptPos, dptCalc));
	if (dist < pConfig->TOLERANCE)
	{
		dptIntersection = dptCalc;
		double length = Distance(dptBegin, dptEnd);
		double length1 = TDraw::Distance(dptCalc, dptBegin);
		double length2 = TDraw::Distance(dptCalc, dptEnd);
		if (length1 > length && length1 > length2)//��pt2һ��
			return 2;
		if (length2 > length && length2 > length1)//
			return 1;
		return 0;
	}
	else
	{
		return -1;
	}
	//}
	//return status;
}


bool TDraw::PointInRealLine(const POINT &ptPos, const DPOINT &dptBegin, const DPOINT &dptEnd, const TConfiguration *pConfig)
{
	POINT pt1 = pConfig->RealToScreen(dptBegin);
	POINT pt2 = pConfig->RealToScreen(dptEnd);

	return PointInRealLine(ptPos, pt1, pt2, pConfig);
}

//�жϵ��Ƿ���������
bool TDraw::PointInRealLine(const POINT &ptPos, const  POINT &ptBegin, const  POINT &ptEnd, const TConfiguration *pConfig)
{
	double length = TDraw::Distance(ptBegin, ptEnd);
	double length1 = TDraw::Distance(ptPos, ptBegin);
	double length2 = TDraw::Distance(ptPos, ptEnd);

	if (length1 + length2 - length <= 0.5)//�ݲ�
		return true;
	else
		return false;
}

//�жϵ��Ƿ���������
bool TDraw::PointInRealLine(POINT ptPos, TRealLine *pRealLine, const TConfiguration *pConfig)
{
	return PointInRealLine(ptPos, pConfig->RealToScreen(pRealLine->ptBegin), pConfig->RealToScreen(pRealLine->ptEnd), pConfig);
}

//������ʼ�����Ӵֺ��߶ε��ĸ��ǵ� angle��ʹ��GetAngleFromScreen����õ�
void TDraw::CalcBarRectCoor(POINT ptResult[4], const POINT &ptBegin, const POINT &ptEnd, double angle, int width)
{
	ptResult[0] = { (LONG)(ptBegin.x + width / 2.0*sin(angle)), (LONG)(ptBegin.y + width / 2.0*cos(angle)) };
	ptResult[1] = { (LONG)(ptEnd.x + width / 2.0*sin(angle)), (LONG)(ptEnd.y + width / 2.0*cos(angle)) };
	ptResult[3] = { (LONG)(ptBegin.x - width / 2.0*sin(angle)), (LONG)(ptBegin.y - width / 2.0*cos(angle)) };
	ptResult[2] = { (LONG)(ptEnd.x - width / 2.0*sin(angle)), (LONG)(ptEnd.y - width / 2.0*cos(angle)) };
}

//���㻬��������� ���룺���ĵ� �Ƕ� �����4������
void TDraw::CalcSliderRectCoor(POINT aptResult[4], const POINT &pt, double angle, const TConfiguration *pConfig)
{
	double c = sqrt(pConfig->SLIDER_H*pConfig->SLIDER_H + pConfig->SLIDER_B*pConfig->SLIDER_B) / 2;
	double theta = atan(double(pConfig->SLIDER_B) / pConfig->SLIDER_H);
	aptResult[0] = { LONG(pt.x + c*sin(angle - theta)), LONG(pt.y + c*cos(angle - theta)) };
	aptResult[1] = { LONG(pt.x + c*sin(angle + theta)), LONG(pt.y + c*cos(angle + theta)) };
	aptResult[2] = { LONG(pt.x - c*sin(angle - theta)), LONG(pt.y - c*cos(angle - theta)) };
	aptResult[3] = { LONG(pt.x - c*sin(angle + theta)), LONG(pt.y - c*cos(angle + theta)) };
}

void TDraw::GetAbsoluteReal(std::vector<DPOINT> &vecdptResult, const std::vector<DPOINT> &vecdpt, const DPOINT &Org, double angle)
{
	for (auto dpt : vecdpt)
		vecdptResult.push_back(GetAbsolute(dpt, Org, angle));
}

//�������ת�������� rp=r+A*s'p Result��Ԥ�����
void TDraw::GetAbsoluteScreen(std::vector<POINT> &vecptResult, const std::vector<DPOINT> &vecdpt, const DPOINT &Org, double angle, const TConfiguration *pConfig)
{
	for (auto dpt : vecdpt)
		vecptResult.push_back(pConfig->RealToScreen(GetAbsolute(dpt, Org, angle)));
}

//�������ת�������� rp=r+A*s'p
DPOINT TDraw::GetAbsolute(const DPOINT &dpt, const DPOINT &Org, double angle)
{
	return{ Org.x + dpt.x*cos(angle) - dpt.y*sin(angle),
		Org.y + dpt.x*sin(angle) + dpt.y*cos(angle) };
}

//��������ת������� s'p=AT*(rp-r)
DPOINT TDraw::GetRelative(const DPOINT &dpt, const DPOINT &Org, double angle)
{
	return{ cos(angle)*(dpt.x - Org.x) + sin(angle)*(dpt.y - Org.y),
		-sin(angle)*(dpt.x - Org.x) + cos(angle)*(dpt.y - Org.y) };
}

//�õ������ߵĽ���
bool TDraw::GetIntersection(const DPOINT &dptL1Begin, const DPOINT &dptL1End, const DPOINT &dptL2Begin, const  DPOINT &dptL2End, DPOINT &dptIntersection)
{
	//L1,L2�˵��غϵ����δ����
	const double &x1 = dptL1Begin.x;
	const double &y1 = dptL1Begin.y;
	const double &x2 = dptL1End.x;
	const double &y2 = dptL1End.y;
	const double &x3 = dptL2Begin.x;
	const double &y3 = dptL2Begin.y;
	const double &x4 = dptL2End.x;
	const double &y4 = dptL2End.y;

	double denominator = ((x1 - x2)*(y3 - y4) - (y1 - y2)*(x3 - x4));
	double t = ((x3 - x1)*(y4 - y3) - (y3 - y1)*(x4 - x3)) / denominator;
	double u = ((x1 - x3)*(y2 - y1) + (y3 - y1)*(x2 - x1)) / -denominator;

	dptIntersection.x = dptL1Begin.x + (dptL1End.x - dptL1Begin.x) * t;
	dptIntersection.y = dptL1Begin.y + (dptL1End.y - dptL1Begin.y) * t;

	if ((t >= 0 && t <= 1) && (u >= 0 && u <= 1))
		return true;
	else
		return false;
}

//������ʽ����
//pt:ԭ�� r:�뾶
//pt1,pt2:����ʼĩ����ĵ�
void TDraw::DrawArc(HDC hdc, const POINT &pt, int r, const POINT &pt1, const POINT &pt2, bool bAlwaysDrawAngleBetween)
{
	if (bAlwaysDrawAngleBetween && GetAngleBetweenPointScreen(pt1, pt, pt2) > M_PI)//>180
		Arc(hdc, pt.x - r, pt.y - r, pt.x + r, pt.y + r, pt2.x, pt2.y, pt1.x, pt1.y);
	else
		Arc(hdc, pt.x - r, pt.y - r, pt.x + r, pt.y + r, pt1.x, pt1.y, pt2.x, pt2.y);
}

//������ʽ����
//pt:ԭ�� r:�뾶
//pt1,pt2:����ʼĩ����ĵ�
//bAlwaysDrawAngleBetween:Ϊtrue��ʼ�ջ��н�
void TDraw::DrawPie(HDC hdc, const POINT &pt, int r, const POINT &pt1, const POINT &pt2, bool bAlwaysDrawAngleBetween)
{
	if (bAlwaysDrawAngleBetween && GetAngleBetweenPointScreen(pt1, pt, pt2) > M_PI)//>180
		Pie(hdc, pt.x - r, pt.y - r, pt.x + r, pt.y + r, pt2.x, pt2.y, pt1.x, pt1.y);
	else
		Pie(hdc, pt.x - r, pt.y - r, pt.x + r, pt.y + r, pt1.x, pt1.y, pt2.x, pt2.y);
}

//����ʽ����
//pt:ԭ�� r:�뾶
//pt1,pt2:����ʼĩ����ĵ�
void TDraw::DrawPie(HDC hdc, const POINT &pt, int r, const POINT &pt1, const POINT &pt2, const LOGPEN &logpen, const COLORREF &crColor)
{
	HPEN hPen;
	hPen = ::CreatePenIndirect(&logpen);
	::SelectObject(hdc, hPen);

	HBRUSH hBrush;
	hBrush = CreateSolidBrush(crColor);
	::SelectObject(hdc, hBrush);

	Pie(hdc, pt.x - r, pt.y - r, pt.x + r, pt.y + r, pt1.x, pt1.y, pt2.x, pt2.y);

	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

void TDraw::DrawSlider(HDC hdc, TSlider *pSlider, const TConfiguration *pConfig)
{
	//����
	if (pConfig->bDrawReal == false)
		for (auto iter = pSlider->vecLine.begin(); iter != pSlider->vecLine.end(); ++iter)
		{
			DPOINT &A1 = pSlider->vecDpt[iter->index1];//�������
			DPOINT &A2 = pSlider->vecDpt[iter->index2];//�������
			DPOINT dptBegin = GetAbsolute(A1, pSlider->dpt, pSlider->angle);
			DPOINT dptEnd = GetAbsolute(A2, pSlider->dpt, pSlider->angle);
			DrawRealLine(hdc, dptBegin, dptEnd, pSlider->logpenStyleShow, pConfig);

			//����õ��ཻ��
			double b = pConfig->ScreenToLengthX(pConfig->SLIDER_B);
			double h = pConfig->ScreenToLengthY(pConfig->SLIDER_H);
			DPOINT R1 = { b / 2, h / 2 };//�������
			DPOINT R2 = { -b / 2, h / 2 };//�������
			DPOINT R3 = { -b / 2, -h / 2 };//�������
			DPOINT R4 = { b / 2, -h / 2 };//�������
			DPOINT B1, B2;//�������
			DPOINT dptIntersection;//�������
			if (GetIntersection(A1, A2, R4, R1, dptIntersection))
			{
				B1 = R4; B2 = R1;
				goto Label;
			}
			if (GetIntersection(A1, A2, R3, R2, dptIntersection))
			{
				B1 = R2; B2 = R3;
				goto Label;
			}
			if (GetIntersection(A1, A2, R1, R2, dptIntersection))
			{
				B1 = R1; B2 = R2;
				goto Label;
			}
			if (GetIntersection(A1, A2, R4, R3, dptIntersection))
			{
				B1 = R3; B2 = R4;
				goto Label;
			}
			goto End;
		Label:
			{
				POINT ptA1;
				POINT ptA2;
				if (GetAngleBetweenPointReal(A1, dptIntersection, B1) > GetAngleBetweenPointReal(B1, dptIntersection, A2))
				{
					ptA1 = pConfig->RealToScreen(dptBegin);
					ptA2 = pConfig->RealToScreen(dptEnd);
				}
				else
				{
					ptA1 = pConfig->RealToScreen(dptEnd);
					ptA2 = pConfig->RealToScreen(dptBegin);
				}
				POINT ptIntersection = pConfig->RealToScreen(GetAbsolute(dptIntersection, pSlider->dpt, pSlider->angle));

				DrawPie(hdc, ptIntersection, pConfig->FRAMEPOINT_R, ptA1, ptA2, pSlider->logpenStyleShow, pSlider->logpenStyleShow.lopnColor);
			}
		End:
			;
		}

	POINT apt[4];
	CalcSliderRectCoor(apt, pConfig->RealToScreen(pSlider->dpt), pSlider->angle, pConfig);

	HRGN hRgn = CreatePolygonRgn(apt, 4, ALTERNATE);

	HBRUSH hBrushFill, hBrushFrame;
	if (pConfig->bDrawReal)
		hBrushFill = CreateSolidBrush(pSlider->logpenStyleShow.lopnColor);
	else
		hBrushFill = CreateSolidBrush(pConfig->crBackground);
	hBrushFrame = CreateSolidBrush(pSlider->logpenStyleShow.lopnColor);

	RECT rect;
	TDrawTranslucent Translucent;
	if (pConfig->bDrawReal)
	{
		//���Ӷ�θ�����
		HRGN hRgnPolylineBar;
		CalcPolylineBarRgn(hRgnPolylineBar, pSlider->vecDpt, pSlider->dpt, pSlider->angle, pConfig);
		CombineRgn(hRgn, hRgn, hRgnPolylineBar, RGN_OR);
		::DeleteObject(hRgnPolylineBar);

		FrameRgn(hdc, hRgn, hBrushFrame, 1, 1);//������

		GetRgnBox(hRgn, &rect);
		SetMarginRect(&rect, -1);

		Translucent.Start(hdc, pSlider->alpha, rect, pSlider->logpenStyleShow.lopnColor == 0);

		OffsetRgn(hRgn, -rect.left, -rect.top);

		FillRgn(hdc, hRgn, hBrushFill);
	}
	else
	{
		FillRgn(hdc, hRgn, hBrushFill);

		FrameRgn(hdc, hRgn, hBrushFrame, 1, 1);//������
	}

	::DeleteObject(hBrushFill);
	::DeleteObject(hBrushFrame);

	if (pConfig->bDrawReal)
	{
		Translucent.End();
	}

	::DeleteObject(hRgn);
}

bool TDraw::PointInSlider(POINT ptPos, TSlider *pSlider, const TConfiguration *pConfig)
{
	POINT apt[4];
	CalcSliderRectCoor(apt, pConfig->RealToScreen(pSlider->dpt), pSlider->angle, pConfig);

	if (PointInRgn(apt, 4, ptPos))
		return true;
	else
	{
		for (auto iter = pSlider->vecLine.begin(); iter != pSlider->vecLine.end(); ++iter)
		{
			if (PointInRealLine(ptPos, GetAbsolute(pSlider->vecDpt[iter->index1], pSlider->dpt, pSlider->angle),
				GetAbsolute(pSlider->vecDpt[iter->index2], pSlider->dpt, pSlider->angle), pConfig))
				return true;
		}
	}

	return false;
}

bool TDraw::PickConstraintColinear(POINT ptPos, TConstraintColinear *pColinear, const TConfiguration *pConfig)
{
	POINT ptCenter1, ptCenter2;
	if (ShowConstraintColinearDotLine(pColinear, ptCenter1, ptCenter2, pConfig))
		if (PointInRealLine(ptPos, ptCenter1, ptCenter2, pConfig))
			return true;
	return false;
}

bool TDraw::PickConstraintCoincide(POINT ptPos, TElement *element, const TConfiguration *pConfig)
{
	TConstraintCoincide *temp = (TConstraintCoincide *)element;

	//�غ�������ʾ�������߱�ʰȡ
	if (TDraw::ShowConstraintCoincideDotLine(temp, pConfig) && TDraw::PointInRealLine(ptPos, temp->GetLinkDpt(0), temp->GetLinkDpt(1), pConfig))
	{
		return true;
	}
	else
		return false;
}

//void TDraw::DrawTranslucentBar(HDC hdc,)