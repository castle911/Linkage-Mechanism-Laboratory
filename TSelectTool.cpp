#include "TSelectTool.h"

#include "TDraw.h"
#include "TMainWindow.h"
#include "TConfiguration.h"

extern TMainWindow win;
TSelectTool::TSelectTool()
{
	Shape = &(win.m_Shape);
	Config = &(win.m_Configuration);
	Canvas = &(win.Canvas);
	iPickRealLineIndex = -1;
}

//��TTool����������������
TSelectTool::~TSelectTool()
{
	if (iPickRealLineIndex != -1)
	{
		Shape->RealLine[iPickRealLineIndex].logpenStyle.lopnStyle = uiLastLineStyle;
	}
}


void TSelectTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_DELETE:
		if (iPickRealLineIndex != -1)
		{
			Shape->DeleteRealLine(iPickRealLineIndex);
			iPickRealLineIndex = -1;
		}
		return;
	}
}

void TSelectTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
}


void TSelectTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
}

bool TSelectTool::PickRealLine(POINT ptPos, TRealLine &RealLine)
{
	POINT pt1 = Config->RealToScreen(RealLine.ptBegin);
	POINT pt2 = Config->RealToScreen(RealLine.ptEnd);
	double length = TDraw::Distance(pt1, pt2);
	double length1 = TDraw::Distance(ptPos, pt1);
	double length2 = TDraw::Distance(ptPos, pt2);

	if (length1 + length2 - length <=0.1)//�ݲ�
		return true;
	else
		return false;
}

void TSelectTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	//��֮ǰ�Ѿ�ʰȡ�õ��ߣ���ָ����͡������ٴ�ʰȡ���潫������ͣ������������ͽ��ڴ˻ָ���
	if (iPickRealLineIndex != -1)
	{
		Shape->RealLine[iPickRealLineIndex].logpenStyle.lopnStyle=uiLastLineStyle;
	}

	//����������
	for (int i = 0; i < Shape->RealLine.size(); i++)
	{
		if (PickRealLine(ptPos,Shape->RealLine[i]))//����ʰȡ
		{
			iPickRealLineIndex = i;

			//�ݴ浱ǰ���Ͳ�����
			uiLastLineStyle = Shape->RealLine[iPickRealLineIndex].logpenStyle.lopnStyle;
			Shape->RealLine[iPickRealLineIndex].logpenStyle.lopnStyle = PS_DOT;

			//֪ͨTreeViewѡ��

			//֪ͨListView����

			return;
		}
	}

	//û��ʰ��
	iPickRealLineIndex = -1;
}

void TSelectTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{

}

//�����Ȳ���WM_PAINT�¼��н��л���
void TSelectTool::Draw(HDC hdc)
{
	if (iPickRealLineIndex != -1)
	{

		//��ʰȡ����
		TDraw::DrawPickSquare(hdc, Config->RealToScreen(Shape->RealLine[iPickRealLineIndex].ptBegin));
		TDraw::DrawPickSquare(hdc, Config->RealToScreen(Shape->RealLine[iPickRealLineIndex].ptEnd));

		//�������ͱ仯���һ���λ�ڹ��߻���֮ǰ��������Ҫˢ��һ��
		::InvalidateRect(Canvas->m_hWnd, &(Canvas->ClientRect), FALSE);
	}
}