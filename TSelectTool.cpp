#pragma once
#include "TSelectTool.h"

#include "TCanvas.h"
#include "TShape.h"
#include "TListView.h"
#include "TTreeViewContent.h"
#include "TConstraintCoincide.h"

#include "TDraw.h"
#include "TConfiguration.h"

#include "TSolver.h"

TSelectTool::TSelectTool()
{
	bDrag = false;
	iPickIndex = -1;
	iPrevPickIndex = -1;
	iHoverIndex = -1;
	Cursor = IDC_ARROW;
}

//��TTool����������������
TSelectTool::~TSelectTool()
{
	if (iPickIndex != -1)
	{
		//�ָ�����
		RestorePickedLineStyle();
		RestoreHoveredLineStyle();

		CancelTreeViewAndListView();
	}
}

void TSelectTool::RestorePickedLineStyle()
{
	while (PickedLineId.size() > 0)
	{
		pShape->GetElementById(PickedLineId.top())->logpenStyleShow.lopnStyle = pShape->GetElementById(PickedLineId.top())->logpenStyle.lopnStyle;
		PickedLineId.pop();
	}
}

void TSelectTool::RestoreHoveredLineStyle()
{
	while (HoveredLineId.size() > 0)
	{
		pShape->GetElementById(HoveredLineId.top())->logpenStyleShow.lopnColor = pShape->GetElementById(HoveredLineId.top())->logpenStyle.lopnColor;
		HoveredLineId.pop();
	}
}

void TSelectTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_DELETE:
		if (iPickIndex != -1)
		{
			//�Ȼָ����ͣ�����ɾ��Ԫ�غ�index��ʧЧ
			RestorePickedLineStyle();
			RestoreHoveredLineStyle();

			int id = pShape->Element[iPickIndex]->id;
			std::vector<int> InfluenceId;// = pShape->GetInfluenceId(id);

			pTreeViewContent->DeleteById(pShape->Element[iPickIndex]->id);
			InfluenceId = pShape->DeleteElement(iPickIndex);

			for (int i = 0; i < InfluenceId.size(); i++)
				pTreeViewContent->DeleteById(InfluenceId[i]);

			iPickIndex = -1;
			iPrevPickIndex = -1;
			iHoverIndex = -1;

			CancelTreeViewAndListView();

			pSolver->RefreshEquations(true);
		}
		return;
	}
}

void TSelectTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, Cursor));
}


void TSelectTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	//::SetCursor(::LoadCursor(NULL, IDC_WAIT));
	if (bDrag)
	{
		pSolver->ClearOutput();
		pSolver->ClearConstraint();
		pSolver->AddMouseConstraint(false, iPickIndex, pConfig->ScreenToReal(ptPos));
		pSolver->Solve(true);
		return;
	}

	RestoreHoveredLineStyle();
	iHoverIndex = -1;

	//��������
	for (int i = 0; i < pShape->Element.size(); i++)
	{
		switch (pShape->Element[i]->eType)
		{
		case ELEMENT_BAR:
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
			if (iPickIndex != iHoverIndex)//�����������ѱ�ѡ���򲻱�ɫ
			{
				//�ݴ浱ǰ���Ͳ�����
				pShape->Element[iHoverIndex]->logpenStyleShow.lopnColor = RGB(200, 200, 200);
				HoveredLineId.push(pShape->Element[iHoverIndex]->id);
			}
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
	CancelTreeViewAndListView();
	RestorePickedLineStyle();
	iPickIndex = -1;

	//��������
	for (size_t i = 0; i < pShape->Element.size(); i++)
	{
		switch (pShape->Element[i]->eType)
		{
		case ELEMENT_BAR:
			if (PickRealLine(ptPos, pShape->Element[i]))//����ʰȡ
			{
				iPickIndex = i;
				if (bDrag)
				{
					iPrevPickIndex = -1;
					bDrag = false;
					Cursor = IDC_ARROW;
				}
				else
				{
					if (iPrevPickIndex != -1 && iPrevPickIndex == iPickIndex)
					{
						bDrag = true;
						Cursor = IDC_HAND;
					}
					iPrevPickIndex = i;
				}
			}
			break;
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
			PickedLineId.push(pShape->Element[iPickIndex]->id);

			//֪ͨTreeViewѡ��
			pTreeViewContent->SelectById(pShape->Element[iPickIndex]->id);

			//֪ͨListView����
			pShape->Element[iPickIndex]->NoticeListView(pListView);
			return;
		}
	}

}

void TSelectTool::SelectById(int id)
{
	RestorePickedLineStyle();
	iPickIndex = -1;

	for (size_t i = 0; i < pShape->Element.size(); i++)
	{
		if (pShape->Element[i]->id == id)
		{
			iPickIndex = i;
			//�ݴ浱ǰ���Ͳ�����
			pShape->Element[iPickIndex]->logpenStyleShow.lopnStyle = PS_DOT;
			PickedLineId.push(pShape->Element[iPickIndex]->id);

			//֪ͨListView����
			//pShape->Element[iPickIndex]->NoticeListView(pListView);

			::InvalidateRect(pCanvas->m_hWnd, &(pCanvas->ClientRect), FALSE);
			return;
		}
	}
}

void TSelectTool::CancelTreeViewAndListView()
{
	//֪ͨTreeViewȡ��ѡ��
	pTreeViewContent->SelectNull();

	//֪ͨListView���
	pListView->DeleteAllItems();
}

void TSelectTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	if (bDrag)
	{
		iPrevPickIndex = -1;
		bDrag = false;
		Cursor = IDC_ARROW;
	}

	for (int i = 0; i < pShape->Element.size(); i++)
	{
		switch (pShape->Element[i]->eType)
		{
		case ELEMENT_FRAMEPOINT:
			break;
		}
	}

}

//�����Ȳ���WM_PAINT�¼��н��л���
void TSelectTool::Draw(HDC hdc)
{
	if (iPickIndex != -1)
	{
		switch (pShape->Element[iPickIndex]->eType)
		{
		case ELEMENT_BAR:
		case ELEMENT_REALLINE:
			//��ʰȡ����
			TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TRealLine *)pShape->Element[iPickIndex])->ptBegin));
			TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TRealLine *)pShape->Element[iPickIndex])->ptEnd));
			break;

		case ELEMENT_FRAMEPOINT:
			//��ʰȡ����
			TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TFramePoint *)pShape->Element[iPickIndex])->dpt));
			break;
		case CONSTRAINT_COINCIDE:
			if (pShape->Element[iPickIndex]->available == true)
			{
				//���غϵ�
				int id = ((TConstraintCoincide *)pShape->Element[iPickIndex])->ElementId1;
				switch (((TConstraintCoincide *)pShape->Element[iPickIndex])->eElementType1)
				{
				case ELEMENT_BAR:
				case ELEMENT_REALLINE:
					if (((TConstraintCoincide *)pShape->Element[iPickIndex])->Element1PointIndex == 1)
						TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TRealLine *)pShape->GetElementById(id))->ptBegin));
					else
						TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TRealLine *)pShape->GetElementById(id))->ptEnd));
					break;
				case ELEMENT_FRAMEPOINT:
					TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TFramePoint *)pShape->GetElementById(id))->dpt));
					break;
				}
			}
			break;
		}
		//�������ͱ仯���һ���λ�ڹ��߻���֮ǰ��������Ҫˢ��һ��
		::InvalidateRect(pCanvas->m_hWnd, &(pCanvas->ClientRect), FALSE);

	}
}