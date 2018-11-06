#pragma once
#include <vector>
#include <Windows.h>

class TDrawTranslucent
{
private:
	HDC *phdc;
	HDC hdcOld;
	HDC hBitmapDC;
	HBITMAP hBitmap;
	VOID *pvBits;
	bool bNeedDrawBlack;
	BYTE alpha;
	long left, top, width, height;

	bool bExchangedX, bExchangedY;//���ڳ��ָ���ȼ����߶ȵ��µ�X,Y���꽻��

	std::vector<RECT *> vecpRect;
	struct TAPT
	{
		POINT *apt;
		int count;
	};
	std::vector<TAPT> vecapt;
	static void TDrawTranslucent::StartTranslucent(HDC &hBitmapDC, HBITMAP &hBitmap, VOID *&pvBits, long left, long top, long width, long height, bool bNeedDrawBlack);
	static void TDrawTranslucent::EndTranslucent(HDC &hdc, HDC &hBitmapDC, HBITMAP &hBitmap, VOID *&pvBits, long left, long top, long width, long height, BYTE alpha, bool bNeedDrawBlack);

	void TDrawTranslucent::Deal(RECT &rect);
	void TDrawTranslucent::Restore(RECT &rect);
	void TDrawTranslucent::Deal(POINT *apt,int count);
	void TDrawTranslucent::Restore(POINT *apt,int count);

public:
	TDrawTranslucent();
	~TDrawTranslucent();

	//ִ��Start��hdc����ΪhBitmap��֮��ֱ��ʹ��hdc��ͼ���ɡ���End�н��ָ�ԭhdc
	void TDrawTranslucent::Start(HDC &hdc, byte alpha, long left, long top, long width, long height, bool bNeedDrawBlack);
	void TDrawTranslucent::Start(HDC &hdc, byte alpha, const RECT &rect, bool bNeedDrawBlack);
	void TDrawTranslucent::End();

	TDrawTranslucent& TDrawTranslucent::Input(RECT *rect);
	TDrawTranslucent& TDrawTranslucent::Input(POINT *point);
	TDrawTranslucent& TDrawTranslucent::Input(POINT *point, int count);
};

