#pragma once
#include "DetectMemoryLeak.h"

#include "resource.h"

#include "TSelectTool.h"

#include "TSlider.h"
#include "TCanvas.h"
#include "TShape.h"
#include "TListView.h"
#include "TTreeViewContent.h"

#include "TDraw.h"
#include "TConfiguration.h"

#include "TFramePoint.h"
#include "TRealLine.h"
#include "TConstraintCoincide.h"

#include "TSolver.h"

TSelectTool::TSelectTool()
{
	eMode = SELECT_MOVE;
	//bShowTips = false;
	bDrag = false;
	bMove = false;
	iPickIndex = -1;
	iPrevPickIndex = -1;
	iHoverIndex = -1;
	Cursor = IDC_ARROW;

	bShowTips = true;
}

//��TTool����������������
TSelectTool::~TSelectTool()
{
	SelectNull();
}


void TSelectTool::SelectNull()
{

	if (iPickIndex != -1)
	{
		//�ָ�����
		RestorePickedLineStyle();
		RestoreHoveredLineStyle();

		CancelTreeViewAndListView();
		iPickIndex = -1;
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

			//�õ���Ӱ���Ԫ��id����ɾ��
			int id = pShape->Element[iPickIndex]->id;
			std::vector<int> InfluenceId;// = pShape->GetInfluenceId(id);

			pTreeViewContent->DeleteById(pShape->Element[iPickIndex]->id);
			InfluenceId = pShape->DeleteElement(iPickIndex);

			for (auto i : InfluenceId)
				pTreeViewContent->DeleteById(i);

			iPickIndex = -1;
			iHoverIndex = -1;
			EndDrag();
			EndMove();

			CancelTreeViewAndListView();

			pSolver->RefreshEquations();

			pCanvas->Invalidate();
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
	ptMouse = ptPos;

	switch (eMode)
	{
	case SELECT_DRAG:
		if (bDrag)
		{
			pSolver->ClearOutput();
			pSolver->ClearConstraint();
			pSolver->AddMouseConstraint(iPickIndex, pConfig->ScreenToReal(ptPos));
			pSolver->Solve();

			pCanvas->Invalidate();

			sTips = TEXT("�ƶ�����Ԥ���϶����");

			return;
		}
		break;
	case SELECT_MOVE:
		if (bMove)
		{
			DPOINT dptDelta = pConfig->ScreenToReal(ptPos) - pConfig->ScreenToReal(ptMouseClick);

			pShape->ChangePos(iPickIndex, dptDelta);

			ptMouseClick = ptPos;

			sTips = TEXT("�����ȷ����λ��");
			return;
		}
		break;
	}

	RestoreHoveredLineStyle();

	//������������ʾ����Ч��
	iHoverIndex = pShape->GetPickedElementIndex(ptPos, pConfig);

	if (iHoverIndex != -1)//��ͣ�ж���
	{
		if (iPickIndex != iHoverIndex)//��������δ��ѡ��
		{
			//�ݴ浱ǰ���Ͳ�����
			pShape->Element[iHoverIndex]->logpenStyleShow.lopnColor = RGB(200, 200, 200);
			HoveredLineId.push(pShape->Element[iHoverIndex]->id);
			sTips = TEXT("�����ѡ��");
			return;
		}
		else
		{
			//�����������ѱ�ѡ���򲻱�ɫ
			switch (eMode)
			{
			case SELECT_MOVE:
				sTips = TEXT("�ٴε���ɽ����ƶ�");
				return;
			case SELECT_DRAG:
				sTips = TEXT("�ٴε���ɽ����϶�");
				return;
			}
		}
	}
	else
		sTips = TEXT("");

}



void TSelectTool::EndDrag()
{
	iPrevPickIndex = -1;
	bDrag = false;
	Cursor = IDC_ARROW;
}

void TSelectTool::EndMove()
{
	iPrevPickIndex = -1;
	bMove = false;
	Cursor = IDC_ARROW;
}

void TSelectTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	CancelTreeViewAndListView();
	RestorePickedLineStyle();

	if (bDrag)
	{
		EndDrag();
	}

	if (bMove)
	{
		EndMove();
	}

	//��������
	iPickIndex = pShape->GetPickedElementIndex(ptPos, pConfig);

	if (iPickIndex != -1)//ʰȡ����
	{
		switch (eMode)
		{
		case SELECT_MOVE:
			//��ʰȡ״̬�ٴε��ͬһ�������ʰȡ
			if (bMove == false)
			{
				if (iPrevPickIndex != -1 && iPrevPickIndex == iPickIndex)
				{
					bMove = true;
					Cursor = IDC_CROSS;
					ptMouseClick.x = ptPos.x;
					ptMouseClick.y = ptPos.y;
				}
				iPrevPickIndex = iPickIndex;
			}
			break;
		case SELECT_DRAG:
			if (bDrag == false)
			{
				if (iPrevPickIndex != -1 && iPrevPickIndex == iPickIndex)
				{
					//��ʰȡ״̬�ٴε��ͬһ�������ʰȡ
					bDrag = true;
					Cursor = IDC_HAND;
				}
				iPrevPickIndex = iPickIndex;
			}
			break;
		}

		//�ݴ浱ǰ���Ͳ�����
		pShape->Element[iPickIndex]->logpenStyleShow.lopnStyle = PS_DOT;
		PickedLineId.push(pShape->Element[iPickIndex]->id);

		//֪ͨTreeViewѡ��
		pTreeViewContent->SelectById(pShape->Element[iPickIndex]->id);

		//֪ͨListView����
		pShape->Element[iPickIndex]->NoticeListView(pListView);
		return;
	}

	//��������
	if (iPickIndex == -1)
		iPrevPickIndex = -1;

}

void TSelectTool::SelectByIndex(size_t index)
{
	RestorePickedLineStyle();

			iPickIndex = index;
			//�ݴ浱ǰ���Ͳ�����
			pShape->Element[iPickIndex]->logpenStyleShow.lopnStyle = PS_DOT;
			PickedLineId.push(pShape->Element[iPickIndex]->id);

			//֪ͨListView����
			pShape->Element[iPickIndex]->NoticeListView(pListView);

			pCanvas->Invalidate();
}

void TSelectTool::SelectById(int id)
{

	for (size_t i = 0; i < pShape->Element.size(); i++)
	{
		if (pShape->Element[i]->id == id)
		{
			SelectByIndex(i);
			return;
		}
	}
	iPickIndex = -1;
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
	iHoverIndex = pShape->GetPickedElementIndex(ptPos, pConfig);
	if (iHoverIndex != -1)// && eMode == SELECT_MOVE
	{
		SelectByIndex(iHoverIndex);

		//�����Ҽ��˵�
		HMENU hMenu = LoadMenu(hInst, (TCHAR *)(IDR_MENU_RIGHT));
		hMenu = GetSubMenu(hMenu, 0);
		ClientToScreen(hWnd, &ptPos);

		TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, ptPos.x, ptPos.y, 0, hWnd, NULL);
	}
	else
	{
		EndDrag();
		EndMove();
		SelectNull();
		pCanvas->Invalidate();
	}

}

//�����Ȳ���WM_PAINT�¼��н��л���
void TSelectTool::Draw(HDC hdc)
{
	if (iPickIndex != -1)
	{
		TElement *pElement = pShape->Element[iPickIndex];
		switch (pElement->eType)
		{
		case ELEMENT_BAR:
		case ELEMENT_REALLINE:
		case ELEMENT_SLIDEWAY:
			//��ʰȡ����
			TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TRealLine *)pElement)->ptBegin));
			TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TRealLine *)pElement)->ptEnd));
			break;

		case ELEMENT_FRAMEPOINT:
			//��ʰȡ����
			TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TFramePoint *)pElement)->dpt));
			break;
		case CONSTRAINT_COINCIDE:
		{
			TConstraintCoincide *temp = ((TConstraintCoincide *)pElement);
			if (TDraw::ShowConstraintCoincideDotLine(temp, pConfig))
			{
				//��ʰȡ����
				TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(temp->GetLinkDpt(0)));
				TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(temp->GetLinkDpt(1)));
			}
			else
			{
				//���غϵ�
				TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(temp->GetLinkDpt(0)));
			}
		}
		break;
		case ELEMENT_POLYLINEBAR:
		case ELEMENT_SLIDER:
		{
			for (auto iter = pElement->vecDpt.begin(); iter != pElement->vecDpt.end(); ++iter)
				//��ʰȡ����
				TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(TDraw::GetAbsolute(*iter, pElement->dpt, pElement->angle)));
		}
		break;
		case CONSTRAINT_COLINEAR:
			//

			break;
		default:
			assert(0);
			break;
		}

	}

	if (bShowTips)
		TDraw::DrawTips(hdc, ptMouse, sTips.c_str(), pConfig);
}

//���������ԭ����ʱ���ж�
bool TSelectTool::CanBeDriver()
{
	if (iPickIndex != -1)
	{
		switch (pShape->Element[iPickIndex]->eType)
		{
		case ELEMENT_BAR:
		case ELEMENT_SLIDER:
			return true;
		}
	}
	return false;
}