#include "TDrawTranslucent.h"
#include "DetectMemoryLeak.h"
#include "TDraw.h"

TDrawTranslucent::TDrawTranslucent()
{
}


TDrawTranslucent::~TDrawTranslucent()
{
}

//���л���x�����-left��y����-top
//ֻҪ���Ĳ��Ǻ�ɫ �������Ǻ�ɫ -> bNeedDrawBlack=false
//��׼��ͷ��ע����EndTranslucent����ɣ������ֶ�����
//HDC hBitmapDC;
//HBITMAP hBitmap;
//VOID *pvBits;
void TDrawTranslucent::StartTranslucent(HDC &hBitmapDC, HBITMAP &hBitmap, VOID *&pvBits, long left, long top, long width, long height, bool bNeedDrawBlack)
{
	//���п�ʼ�����˴�
	hBitmapDC = CreateCompatibleDC(NULL);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = width;
	bmpInfo.bmiHeader.biHeight = height;//������˵�����ݴ��µ��ϣ���δ����������ϵ���  
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	hBitmap = CreateDIBSection(hBitmapDC, &bmpInfo, DIB_RGB_COLORS, &pvBits, NULL, 0x0);
	SelectObject(hBitmapDC, hBitmap);

	//��ʼ��
	if (bNeedDrawBlack)//�������Ǻ�ɫ����Ϳ��
	{
		HPEN hPen = (HPEN)::GetStockObject(NULL_PEN);
		::SelectObject(hBitmapDC, hPen);

		HBRUSH hBrush = CreateSolidBrush(0xffffff);//white

		RECT rcTrans = { 0, 0, width, height };
		::FillRect(hBitmapDC, &rcTrans, hBrush);

		DeleteObject(hPen);
		DeleteObject(hBrush);
	}
}

void TDrawTranslucent::EndTranslucent(HDC &hdc, HDC &hBitmapDC, HBITMAP &hBitmap, VOID *&pvBits, long left, long top, long width, long height, BYTE alpha, bool bNeedDrawBlack)
{
	if (pvBits)
	{
		//��������������Ϊ��͸��
		UINT32 *data;
		if (bNeedDrawBlack)
			for (int i = 0; i < width*height; ++i)
			{
				data = ((UINT32 *)pvBits) + i;
				if (*data != 0x00ffffff)//not white
					*data |= 0xff000000;
			}
		else
			for (int i = 0; i < width*height; ++i)//��ɫ����
			{
				data = ((UINT32 *)pvBits) + i;
				if (*data)//Ϊ��ɫ
					*data |= 0xff000000;
			}
	}
#ifdef _DEBUG
	else
		OutputDebugPrintf(TEXT("pvBits == false\n"));
#endif
	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.AlphaFormat = AC_SRC_ALPHA;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = alpha;

	AlphaBlend(hdc, left, top, width, height, hBitmapDC, 0, 0, width, height, bf);

	DeleteObject(hBitmap);
	DeleteObject(hBitmapDC);
}

//���л���x�����-left��y����-top
//ֻҪ���Ĳ��Ǻ�ɫ �������Ǻ�ɫ -> bNeedDrawBlack=false
void TDrawTranslucent::Start(HDC &hdc, byte alpha, long input_left, long input_top, long input_width, long input_height, bool bNeedDrawBlack)
{
	//OutputDebugPrintf(TEXT("il%d it%d iw%d ih%d\n"), input_left, input_top, input_width, input_height);
	//���п�ʼ�����˴�
	if (input_width < 0)
	{
		this->left = input_left + input_width;
		this->width = -input_width;
		bExchangedX = true;
	}
	else
	{
		this->left = input_left;
		this->width = input_width;
		bExchangedX = false;
	}

	if (input_height < 0)
	{
		this->top = input_top + input_height;
		this->height = -input_height;
		bExchangedY = true;
	}
	else
	{
		this->top = input_top;
		this->height = input_height;
		bExchangedY = false;
	}
	//OutputDebugPrintf(TEXT("x%d y%d w%d h%d\n"), this->left, this->top, this->width, this->height);

	this->phdc = &hdc;
	this->hdcOld = hdc;
	this->bNeedDrawBlack = bNeedDrawBlack;
	this->alpha = alpha;
	this->StartTranslucent(hBitmapDC, hBitmap, pvBits, left, top, width, height , bNeedDrawBlack);
	hdc = hBitmapDC;
}

//���л���x�����Ӧ-left��y����Ӧ-top
//ֻҪ���Ĳ��Ǻ�ɫ �������Ǻ�ɫ -> bNeedDrawBlack=false
void TDrawTranslucent::Start(HDC &hdc, byte alpha, const RECT &rect, bool bNeedDrawBlack)
{
	Start(hdc, alpha, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, bNeedDrawBlack);
}

void TDrawTranslucent::End()
{
	this->EndTranslucent(hdcOld, hBitmapDC, hBitmap, pvBits, left,top,width,height, alpha, bNeedDrawBlack);
	
	//�ָ�����
	*phdc = hdcOld;
	for (auto &pRect:vecpRect)
		Restore(*pRect);

	for (auto &tapt : vecapt)
		Restore(tapt.apt, tapt.count);
}

void TDrawTranslucent::Deal(RECT &rect)
{
	TDraw::MoveRect(rect, left, top);
	if (bExchangedX)
		std::swap(rect.left, rect.right);
	if (bExchangedY)
		std::swap(rect.top, rect.bottom);
}
void TDrawTranslucent::Restore(RECT &rect)
{
	TDraw::MoveRect(rect, -left, -top);
	if (bExchangedX)
		std::swap(rect.left, rect.right);
	if (bExchangedY)
		std::swap(rect.top, rect.bottom);
}

void TDrawTranslucent::Deal(POINT *apt, int count)
{
	TDraw::MoveByDelta(apt, count, -left, -top);
}

void TDrawTranslucent::Restore(POINT *apt, int count)
{
	TDraw::MoveByDelta(apt, count, left, top);
}

TDrawTranslucent& TDrawTranslucent::Input(RECT *rect)
{
	Deal(*rect);
	vecpRect.push_back(rect);
	return *this;
}

TDrawTranslucent& TDrawTranslucent::Input(POINT *point)
{
	return Input(point, 1);;
}
TDrawTranslucent& TDrawTranslucent::Input(POINT *point, int count)
{
	Deal(point, count);
	vecapt.push_back({ point, count });
	return *this;
}