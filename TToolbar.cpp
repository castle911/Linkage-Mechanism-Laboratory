#include "TToolbar.h"

TToolbar::TToolbar()
{
	m_hWnd = NULL;
	m_hInst = NULL;
	tbButtons = NULL;
	iButtonNum = 0; 

	bIsFlat = false;
	bTextOnRight = false;
	bAutoWrap = false;
}


TToolbar::~TToolbar()
{
	free(tbButtons);
}

void TToolbar::CreateToolbar(HWND hwndParent, HINSTANCE hInst)
{
	// Create the toolbar.
	m_hWnd = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD | WS_VISIBLE |TBSTYLE_ALTDRAG|CCS_ADJUSTABLE
		|(bIsFlat?TBSTYLE_FLAT:0)
		|(bTextOnRight?TBSTYLE_LIST:0)
		|(bAutoWrap?TBSTYLE_WRAPABLE:0),
		0, 0, 0, 0,
		hwndParent, NULL, hInst, NULL);
	m_hInst = hInst;
}

//�ڲ�����������ͼƬ�б�
void TToolbar::CreateImageList(UINT uMsg,int cx, int cy, UINT BitmapID,COLORREF crMask)
{
	// Create the image list.
	HIMAGELIST m_hImageList;
	m_hImageList = ImageList_Create(cx, cy, ILC_COLOR32 | ILC_MASK, 1, 1);//
	HANDLE hImage = NULL;
	hImage = LoadImage(m_hInst, (TCHAR *)BitmapID, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	ImageList_AddMasked(m_hImageList, (HBITMAP)hImage, crMask);

	DeleteObject(hImage);
	SendMessage(m_hWnd, uMsg, (WPARAM)0, (LPARAM)m_hImageList);
}

//���ù�����ͼƬ
void TToolbar::LoadImageList(int cx, int cy, UINT BitmapID,COLORREF crMask)
{
	CreateImageList(TB_SETIMAGELIST, cx, cy, BitmapID,crMask);
}

//���ù���������ͼƬ
void TToolbar::LoadHotImageList(int cx, int cy, UINT BitmapID, COLORREF crMask)
{
	CreateImageList(TB_SETHOTIMAGELIST, cx, cy, BitmapID, crMask);
}

//���ù���������ͼƬ
void TToolbar::LoadDisabledImageList(int cx, int cy, UINT BitmapID, COLORREF crMask)
{
	CreateImageList(TB_SETDISABLEDIMAGELIST, cx, cy, BitmapID,crMask);
}

//��Ӱ�ť
void TToolbar::AddElement(int IconIndex,int idCommand,BYTE fsState,BYTE fsStyle,BYTE bReverse[],DWORD_PTR dwData,INT_PTR iString)
{
	iButtonNum++;
	tbButtons = (TBBUTTON *)realloc(tbButtons, iButtonNum*sizeof(TBBUTTON));
	ZeroMemory(&(tbButtons[iButtonNum - 1]), sizeof(TBBUTTON));
	tbButtons[iButtonNum - 1].iBitmap = IconIndex;
	tbButtons[iButtonNum - 1].idCommand = idCommand;
	tbButtons[iButtonNum - 1].fsState = fsState;
	tbButtons[iButtonNum - 1].fsStyle = fsStyle;
	if (bReverse!=NULL)
		memcpy(tbButtons[iButtonNum - 1].bReserved, bReverse, sizeof(tbButtons->bReserved));
	tbButtons[iButtonNum - 1].dwData = dwData;
	tbButtons[iButtonNum - 1].iString = iString;
}

//�����ͨ��ť��IconIndexΪ��ť��ţ���Ӧ��ͼƬ�зֺ�ڼ��顣��0��ʼ��
void TToolbar::AddButton(int IconIndex, int idCommand,bool Enable, TCHAR iString[])
{
	AddElement(IconIndex, idCommand, Enable?TBSTATE_ENABLED:0, BTNS_BUTTON, NULL, NULL, (INT_PTR)iString);
}

//��ӷָ�����iWidthָ�ָ�����߽絽��һ����ť��߽�ľ���
void TToolbar::AddSeparator(int iWidth)
{
	AddElement(iWidth, 0, 0, BTNS_SEP, NULL, NULL, NULL);
}

void TToolbar::AddGroup(int IconIndex, int idCommand, bool Enable, TCHAR iString[])
{
	AddElement(IconIndex, idCommand, Enable ? TBSTATE_ENABLED : 0, BTNS_CHECKGROUP, NULL, NULL, (INT_PTR)iString);
}

//���һ��Check��ť
void TToolbar::AddCheck(int IconIndex, int idCommand, bool Enable, TCHAR iString[])
{
	AddElement(IconIndex, idCommand, Enable ? TBSTATE_ENABLED : 0, BTNS_CHECK, NULL, NULL, (INT_PTR)iString);
}

//��ʾ������
void TToolbar::ShowToolbar()
{
	// Add buttons.
	SendMessage(m_hWnd, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(m_hWnd, TB_ADDBUTTONS, (WPARAM)iButtonNum, (LPARAM)tbButtons);

	// Resize the toolbar, and then show it.
	SendMessage(m_hWnd, TB_AUTOSIZE, 0, 0);
}

//ˢ�¹�����
void TToolbar::FreshSize()
{
	// Resize the toolbar, and then show it.
	SendMessage(m_hWnd, TB_AUTOSIZE, 0, 0);
}

//��ù�������С
RECT TToolbar::GetClientRect()
{
	RECT rect;
	::GetClientRect(m_hWnd, &rect);
	return rect;
}