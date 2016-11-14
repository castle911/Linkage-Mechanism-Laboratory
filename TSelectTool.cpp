#include "TSelectTool.h"

#include "TDraw.h"
#include "TMainWindow.h"
#include "TConfiguration.h"

extern TMainWindow win;
TSelectTool::TSelectTool()
{
	pShape = &(win.m_Shape);
	pConfig = &(win.m_Configuration);
	pCanvas = &(win.Canvas);
	pListView = &(win.RightWindow.ListView);
	iPickRealLineIndex = -1;
}

//��TTool����������������
TSelectTool::~TSelectTool()
{
	if (iPickRealLineIndex != -1)
	{
		//�ָ�����
		pShape->RealLine[iPickRealLineIndex].logpenStyleShow.lopnStyle = pShape->RealLine[iPickRealLineIndex].logpenStyle.lopnStyle;

		//֪ͨTreeViewȡ��ѡ��

		//֪ͨListView����
		pListView->DeleteAllItems();
	}
}


void TSelectTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_DELETE:
		if (iPickRealLineIndex != -1)
		{
			pShape->DeleteRealLine(iPickRealLineIndex);
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
	POINT pt1 = pConfig->RealToScreen(RealLine.ptBegin);
	POINT pt2 = pConfig->RealToScreen(RealLine.ptEnd);
	double length = TDraw::Distance(pt1, pt2);
	double length1 = TDraw::Distance(ptPos, pt1);
	double length2 = TDraw::Distance(ptPos, pt2);

	if (length1 + length2 - length <=0.5)//�ݲ�
		return true;
	else
		return false;
}

void TSelectTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	//��֮ǰ�Ѿ�ʰȡ�õ��ߣ���ָ����͡������ٴ�ʰȡ���潫������ͣ������������ͽ��ڴ˻ָ���
	if (iPickRealLineIndex != -1)
	{
		pShape->RealLine[iPickRealLineIndex].logpenStyleShow.lopnStyle = pShape->RealLine[iPickRealLineIndex].logpenStyle.lopnStyle;
	}

	//����������
	for (int i = 0; i < pShape->RealLine.size(); i++)
	{
		if (PickRealLine(ptPos,pShape->RealLine[i]))//����ʰȡ
		{
			iPickRealLineIndex = i;

			//�ݴ浱ǰ���Ͳ�����
			pShape->RealLine[iPickRealLineIndex].logpenStyleShow.lopnStyle = PS_DOT;

			//֪ͨTreeViewѡ��

			//֪ͨListView����
			pShape->RealLine[iPickRealLineIndex].NoticeListView(pListView);
			return;
		}
	}

	for (int i = 0; i < pShape->FramePoint.size(); i++)
	{
		if (TDraw::PointInFramePoint(pConfig->RealToScreen(pShape->FramePoint[i].dpt), ptPos))
		{
		}
	}

	//û��ʰ��
	iPickRealLineIndex = -1;

	//֪ͨTreeViewȡ��ѡ��

	//֪ͨListView����
	pListView->DeleteAllItems();
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
		TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(pShape->RealLine[iPickRealLineIndex].ptBegin));
		TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(pShape->RealLine[iPickRealLineIndex].ptEnd));

		//�������ͱ仯���һ���λ�ڹ��߻���֮ǰ��������Ҫˢ��һ��
		::InvalidateRect(pCanvas->m_hWnd, &(pCanvas->ClientRect), FALSE);
	}
}