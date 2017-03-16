#pragma once
#include "DetectMemoryLeak.h"

#include "TSelectTool.h"

#include "TSlider.h"
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

			pSolver->RefreshEquations(true);

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
			pSolver->AddMouseConstraint(false, iPickIndex, pConfig->ScreenToReal(ptPos));
			pSolver->Solve(true);

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
	iHoverIndex = -1;

	//������������ʾ����Ч��
	for (size_t i = 0; i < pShape->Element.size(); i++)
	{
		switch (pShape->Element[i]->eType)
		{
		case ELEMENT_BAR:
		case ELEMENT_REALLINE:
		case ELEMENT_SLIDEWAY:
			if (TDraw::PointInRealLine(ptPos, (TRealLine *)pShape->Element[i], pConfig))//����ʰȡ
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
		case ELEMENT_SLIDER:
			if (TDraw::PointInSlider(ptPos, (TSlider *)pShape->Element[i], pConfig))
			{
				iHoverIndex = i;
			}
			break;
		case CONSTRAINT_COINCIDE:
			if (PickConstraintCoincide(ptPos, pShape->Element[i]))
			{
				iHoverIndex = i;
			}
			break;
		case CONSTRAINT_COLINEAR:
			if (PickConstraintColinear(ptPos, pShape->Element[i]))
			{
				iHoverIndex = i;
			}
			break;
		default:
			assert(0);
			break;
		}
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
}


bool TSelectTool::PickConstraintColinear(POINT ptPos, TElement *element)
{
	return false;
}

bool TSelectTool::PickConstraintCoincide(POINT ptPos, TElement *element)
{
	TConstraintCoincide *temp = (TConstraintCoincide *)element;

	//�غ�������ʾ�������߱�ʰȡ
	if (TDraw::ShowConstraintCoincideDotLine(temp, pConfig) && TDraw::PointInRealLine(ptPos, *(temp->pDpt[0]), *(temp->pDpt[1]), pConfig))
	{
		return true;
	}
	else
		return false;
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
	iPickIndex = -1;

	if (bDrag)
	{
		EndDrag();
	}

	if (bMove)
	{
		EndMove();
	}

	//��������
	for (size_t i = 0; i < pShape->Element.size(); i++)
	{
		switch (pShape->Element[i]->eType)
		{
		case ELEMENT_BAR:
		case ELEMENT_REALLINE:
		case ELEMENT_SLIDEWAY:
			if (TDraw::PointInRealLine(ptPos, (TRealLine *)pShape->Element[i], pConfig))//����ʰȡ
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
		case ELEMENT_SLIDER:
			if (TDraw::PointInSlider(ptPos, (TSlider *)pShape->Element[i], pConfig))
			{
				iPickIndex = i;
			}
			break;
		case CONSTRAINT_COINCIDE:
			if (PickConstraintCoincide(ptPos, pShape->Element[i]))
			{
				iPickIndex = i;
			}
			break;
		case CONSTRAINT_COLINEAR:
			if (PickConstraintColinear(ptPos, pShape->Element[i]))
			{
				iPickIndex = i;
			}
			break;
		default:
			assert(0);
			break;
		}

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
					iPrevPickIndex = i;
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
					iPrevPickIndex = i;
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
	}
	//��������
	if (iPickIndex == -1)
		iPrevPickIndex = -1;

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
			pShape->Element[iPickIndex]->NoticeListView(pListView);

			pCanvas->Invalidate();
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
	EndDrag();
	EndMove();
	SelectNull();

	pCanvas->Invalidate();

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
		case ELEMENT_SLIDEWAY:
			//��ʰȡ����
			TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TRealLine *)pShape->Element[iPickIndex])->ptBegin));
			TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TRealLine *)pShape->Element[iPickIndex])->ptEnd));
			break;

		case ELEMENT_FRAMEPOINT:
			//��ʰȡ����
			TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TFramePoint *)pShape->Element[iPickIndex])->dpt));
			break;
		case CONSTRAINT_COINCIDE:
		{
			TConstraintCoincide *temp = ((TConstraintCoincide *)pShape->Element[iPickIndex]);
			if (TDraw::ShowConstraintCoincideDotLine(temp, pConfig))
			{
				//��ʰȡ����
				TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(*(temp->pDpt[0])));
				TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(*(temp->pDpt[1])));
			}
			else
			{
				//���غϵ�
				TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(*(temp->pDpt[0])));
			}
		}
		break;
		case ELEMENT_SLIDER:
		{
			TSlider *pSlider = (TSlider *)pShape->Element[iPickIndex];
			for (auto iter = pSlider->vecDpt.begin(); iter != pSlider->vecDpt.end(); ++iter)

				//��ʰȡ����
				TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(TDraw::GetAbsolute(*iter, pSlider->dpt, pSlider->angle)));
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