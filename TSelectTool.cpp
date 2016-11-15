#pragma once
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
	iPickIndex = -1;
	iHoverIndex = -1;
}

//��TTool����������������
TSelectTool::~TSelectTool()
{
	if (iPickIndex != -1)
	{
		//�ָ�����
		pShape->Element[iPickIndex]->logpenStyleShow.lopnStyle = pShape->Element[iPickIndex]->logpenStyle.lopnStyle;

		CancelTreeViewAndListView();
	}
}


void TSelectTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_DELETE:
		if (iPickIndex != -1)
		{
			pShape->DeleteElement(iPickIndex);
			iPickIndex = -1;
			iHoverIndex = -1;

			CancelTreeViewAndListView();
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
	//��֮ǰ�Ѿ�ʰȡ�õ��ߣ���ָ����͡������ٴ�ʰȡ���潫������ͣ������������ͽ��ڴ˻ָ���
	if (iHoverIndex != -1)
	{
			pShape->Element[iHoverIndex]->logpenStyleShow.lopnColor = pShape->Element[iHoverIndex]->logpenStyle.lopnColor;
		iHoverIndex = -1;
	}

	//��������
	for (int i = 0; i < pShape->Element.size(); i++)
	{
		switch (pShape->Element[i]->eType)
		{
		case ELEMENT_REALLINE:
			if (PickRealLine(ptPos, pShape->Element[i]))//����ʰȡ
			{
				iHoverIndex = i;
			}
			break;
		case ELEMENT_FRAMEPOINT:
			if (TDraw::PointInFramePoint(pConfig->RealToScreen(((TFramePoint *)(pShape->Element[i]))->dpt), ptPos))
			{
				iHoverIndex = i;
			}
			break;
		}
		if (iHoverIndex != -1)
		{
			if (iPickIndex!=iHoverIndex)//�����������ѱ�ѡ���򲻱�ɫ
			//�ݴ浱ǰ���Ͳ�����
			pShape->Element[iHoverIndex]->logpenStyleShow.lopnColor= RGB(200,200,200);
			return;
		}
	}
}

//Ӧ�ڵ���ǰ���������жϣ����⽫��TRealLine *��Ԫ�ش���
bool TSelectTool::PickRealLine(POINT ptPos, TElement *Element)
{
	TRealLine *pRealLine = (TRealLine *)(Element);
		POINT pt1 = pConfig->RealToScreen(pRealLine->ptBegin);
		POINT pt2 = pConfig->RealToScreen(pRealLine->ptEnd);
		double length = TDraw::Distance(pt1, pt2);
		double length1 = TDraw::Distance(ptPos, pt1);
		double length2 = TDraw::Distance(ptPos, pt2);

		if (length1 + length2 - length <= 0.5)//�ݲ�
			return true;
		else
			return false;
}

void TSelectTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	//��֮ǰ�Ѿ�ʰȡ�õ��ߣ���ָ����͡������ٴ�ʰȡ���潫������ͣ������������ͽ��ڴ˻ָ���
	if (iPickIndex != -1)
	{
		pShape->Element[iPickIndex]->logpenStyleShow.lopnStyle = pShape->Element[iPickIndex]->logpenStyle.lopnStyle;
		iPickIndex = -1;
	}

	//��������
	for (int i = 0; i < pShape->Element.size(); i++)
	{
		switch (pShape->Element[i]->eType)
		{
		case ELEMENT_REALLINE:
			if (PickRealLine(ptPos, pShape->Element[i]))//����ʰȡ
			{
				iPickIndex = i;
			}
			break;
		case ELEMENT_FRAMEPOINT:
			if (TDraw::PointInFramePoint(pConfig->RealToScreen(((TFramePoint *)(pShape->Element[i]))->dpt), ptPos))
			{
				iPickIndex = i;
			}
			break;
		}
		if (iPickIndex != -1)
		{
			//�ݴ浱ǰ���Ͳ�����
			pShape->Element[iPickIndex]->logpenStyleShow.lopnStyle = PS_DOT;

			//֪ͨTreeViewѡ��

			//֪ͨListView����
			pShape->Element[iPickIndex]->NoticeListView(pListView);
			return;
		}
	}

	//û��ʰ��
	CancelTreeViewAndListView();
}

void TSelectTool::CancelTreeViewAndListView()
{
	//֪ͨTreeViewȡ��ѡ��

	//֪ͨListView���
	pListView->DeleteAllItems();
}

void TSelectTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{

}

//�����Ȳ���WM_PAINT�¼��н��л���
void TSelectTool::Draw(HDC hdc)
{
	if (iPickIndex != -1)
	{
		switch (pShape->Element[iPickIndex]->eType)
		{
		case ELEMENT_REALLINE:
			//��ʰȡ����
			TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TRealLine *)pShape->Element[iPickIndex])->ptBegin));
			TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TRealLine *)pShape->Element[iPickIndex])->ptEnd));
			//�������ͱ仯���һ���λ�ڹ��߻���֮ǰ��������Ҫˢ��һ��
			::InvalidateRect(pCanvas->m_hWnd, &(pCanvas->ClientRect), FALSE);
			break;

		case ELEMENT_FRAMEPOINT:
			//��ʰȡ����
			TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TFramePoint *)pShape->Element[iPickIndex])->dpt));
			//�������ͱ仯���һ���λ�ڹ��߻���֮ǰ��������Ҫˢ��һ��
			::InvalidateRect(pCanvas->m_hWnd, &(pCanvas->ClientRect), FALSE);
			break;
		}

	}
}