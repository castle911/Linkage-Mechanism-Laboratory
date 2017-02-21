#pragma once
#include "DetectMemoryLeak.h"
#include "TListView.h"

#include "TEdit.h"

#include "TMainWindow.h"

extern TMainWindow win;
TListView::TListView()
{
	iRowCount = 0;
	iColumnCount = 0;
	pShape = &(win.m_Shape);
}


TListView::~TListView()
{
}

WNDPROC TListView::oldListViewProc;
LRESULT CALLBACK TListView::subListViewProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TListView * pListView;
	pListView = (TListView *)GetWindowLong(hWnd, GWL_USERDATA);
	if (pListView)
		return pListView->WndProc(oldListViewProc, hWnd, uMsg, wParam, lParam);
	else
		return CallWindowProc(oldListViewProc, hWnd, uMsg, wParam, lParam);
}

LRESULT TListView::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_USER:
	{
			if (tempEdit.Text != NULL)
			{
				//ˢ��ListView
				pShape->GetElementById(id)->NoticeListView(this);

				//�ָ�λ��
				ListView_Scroll(hWnd,origin.x,origin.y);
				win.Canvas.Invalidate();
			}
		break;
	}
	case WM_VSCROLL:
		//assert(0);
		break;
	case WM_MOUSEWHEEL:
		if (tempEdit.GetVisible())
		{
			RECT rect = GetGridRectInMargin(tempEdit.ListItemIndex, 1);//�����������������

			tempEdit.SetPos(rect);
			PostMessage(hWnd, WM_PAINT, 0, 0);
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	{
		int xPos = LOWORD(lParam);
		int yPos = HIWORD(lParam);

		int index = ListView_GetHotItem(hWnd);//���������
		RECT rect = GetGridRectInMargin(index, 1);//�����������������
		bool bClickCol1 =xPos > ListView_GetColumnWidth(hWnd, 0);//�������������������Ŀ��


		tempEdit.SetVisible(false);

		if (index != -1)//ȷʵѡ��
		{
			if (bClickCol1)//��ĵ�2��
			{
				//��ʾ�ؼ�

				//�õ���ͼԭ��
				ListView_GetOrigin(hWnd, &origin);

				switch (vecCtrlType[index])
				{
				case CTRLTYPE_EDIT:
				case CTRLTYPE_COOR_EDIT:
				case CTRLTYPE_COOR_P1_EDIT:
				case CTRLTYPE_COOR_P2_EDIT:
				case CTRLTYPE_LEN_EDIT:
				case CTRLTYPE_ANGLE_EDIT:
				{

					//����Edit����ʾ
					tempEdit.eCtrlType = vecCtrlType[index];
					tempEdit.ListItemIndex = index;
					tempEdit.SetPos(rect);

					TCHAR buf[100];
					ListView_GetItemText(hWnd, index, 1, buf, sizeof(buf)*sizeof(TCHAR));
					tempEdit.SetText(buf);

					//�����޸�Ȩ
					tempEdit.pElement = pShape->GetElementById(id);

					tempEdit.SetVisible(true);
					SetFocus(tempEdit.m_hWnd);

					POINT ptClickPos;
					ptClickPos.x = xPos - rect.left;
					ptClickPos.y = yPos - rect.top;

					PostMessage(tempEdit.m_hWnd, WM_LBUTTONDOWN, 0, (LPARAM)MAKELONG(ptClickPos.x,ptClickPos.y));

					return NULL;

					break;
				}
				case CTRLTYPE_NULL:
					break;
				default:
					assert(0);
					break;
				}
			}

		}
		break;
	}
	}
	return CallWindowProc(wndproc, hWnd, uMsg, wParam, lParam);
}

void TListView::DeleteAllItems()
{
	ListView_DeleteAllItems(m_hWnd);
	iRowCount = 0;
	iColumnCount = 0;

	vecCtrlType.clear();
	vecpContent.clear();
}

int TListView::GetItemCount()
{
	return ListView_GetItemCount(m_hWnd);
}

void CDECL TListView::AddAttributeItem(const TCHAR szName[], enumCtrlType eCtrlType, void *pContent, const TCHAR szEditFormat[], ...)
{
	//�����ǩ
	InsertItem(iRowCount, 0, szName);

	TCHAR szBuffer[1024];
	va_list pArgList;
	va_start(pArgList, szEditFormat);
	_vsntprintf_s(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szEditFormat, pArgList);
	va_end(pArgList);

	//��������
	InsertItem(iRowCount - 1, 1, szBuffer);

	vecCtrlType.push_back(eCtrlType);
	vecpContent.push_back(pContent);
}

void CDECL TListView::SetAttributeItemValue(int index, TCHAR szEditFormat[], ...)
{
	TCHAR szBuffer[1024];
	va_list pArgList;
	va_start(pArgList, szEditFormat);
	_vsntprintf_s(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szEditFormat, pArgList);
	va_end(pArgList);

	ListView_SetItemText(m_hWnd, index, 1, szBuffer);
}

void TListView::CreateListViewEx(HWND hParent, UINT id, HINSTANCE hInst)
{
	m_hInst = hInst;
	m_hWnd = ::CreateWindowEx(0,
		WC_LISTVIEW,
		0,
		WS_CHILD | WS_VISIBLE | LVS_ICON | LVS_REPORT | LVS_SINGLESEL,//|LVS_EDITLABELS|WS_BORDER
		0, 0, 0, 0, hParent, (HMENU)id, hInst, 0);

	//������չ�����Ӹ������Լ�����ѡ��
	DWORD styles = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyleEx(m_hWnd, styles, styles);

	//����Edit
	tempEdit.CreateEditEx(m_hWnd, 0, m_hInst);
	tempEdit.SetDefaultGuiFont();
	tempEdit.SetPos(10, 10, 100, 20);
	tempEdit.SetVisible(false);

	//
	SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);
	oldListViewProc = (WNDPROC)::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)subListViewProc);

}


void TListView::InsertColumn(int index, TCHAR text[], int width, int styleLVCFMT)
{
	LVCOLUMN lvc = { 0 };
	lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_FMT | LVCF_SUBITEM | LVCF_ORDER;
	lvc.cx = width;
	lvc.fmt = styleLVCFMT;
	lvc.pszText = text;
	lvc.iSubItem = index;
	lvc.iOrder = index;
	SendMessage(m_hWnd, LVM_INSERTCOLUMN, index, (LPARAM)&lvc);
	iColumnCount++;
}

void TListView::AddColumn(TCHAR text[], int width, int styleLVCFMT)
{
	InsertColumn(iColumnCount, text, width, styleLVCFMT);
}

void CDECL TListView::InsertItem(int index, int subitem, const TCHAR szFormat[], ...)
{
	TCHAR szBuffer[1024];
	va_list pArgList;
	va_start(pArgList, szFormat);
	_vsntprintf_s(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);
	va_end(pArgList);

	LVITEM lvi = { 0 };
	lvi.mask = LVIF_TEXT;
	lvi.iItem = index;
	lvi.pszText = szBuffer;

	if (subitem == 0)
	{
		lvi.iSubItem = 0;
		::SendMessage(m_hWnd, LVM_INSERTITEM, 0, (LPARAM)&lvi);
		iRowCount++;
	}
	else
	{
		lvi.iSubItem = subitem;
		::SendMessage(m_hWnd, LVM_SETITEM, 0, (LPARAM)&lvi);
	}

}

RECT TListView::GetGridRectInMargin(int index, int subitem)
{
	RECT rect;
	rect = GetGridRect(index, subitem);
	rect.left += 1;
	rect.top += 1;
	rect.right -= 1;
	rect.bottom -= 1;
	return rect;
}

RECT TListView::GetGridRect(int index, int subitem)
{
	RECT rect;
	ListView_GetSubItemRect(m_hWnd, index, subitem, LVIR_BOUNDS, &rect);
	//���ڵ�һ�У���Ҫ���⴦��
	if (subitem == 0)
	{
		int nWidth0 = ListView_GetColumnWidth(m_hWnd, 0);
		rect.right = rect.left + nWidth0;
	}
	return rect;
}

void TListView::LoadObject(int index)
{
	this->DeleteAllItems();

	TCHAR buffer[16];

	switch (pShape->Element[index]->eType)
	{
	case ELEMENT_FRAMEPOINT:
	{
		TFramePoint *pElement = (TFramePoint *)pShape->Element[index];
		AddAttributeItem(TEXT("ID"), CTRLTYPE_NULL, NULL, TEXT("%d"), pElement->id);
		AddAttributeItem(TEXT("����"), CTRLTYPE_EDIT, &(pElement->Name), pElement->Name);
		AddAttributeItem(TEXT("����"), CTRLTYPE_NULL, NULL, TEXT("����"));
		AddAttributeItem(TEXT("����"), CTRLTYPE_NULL, NULL, pElement->GetLineStyleName(pElement->logpenStyle.lopnStyle, buffer));
		AddAttributeItem(TEXT("�߿�"), CTRLTYPE_NULL, NULL, TEXT("%d"), pElement->logpenStyle.lopnWidth);
		AddAttributeItem(TEXT("��ɫ"), CTRLTYPE_NULL, NULL, TEXT("0x%X"), pElement->logpenStyle.lopnColor);
		AddAttributeItem(TEXT("P0"), CTRLTYPE_COOR_EDIT, &(pElement->dpt), TEXT("%.3f,%.3f"), pElement->dpt.x, pElement->dpt.y);
		break;
	}
	}
}