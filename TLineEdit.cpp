#pragma once
#include "TLineEdit.h"


TLineEdit::TLineEdit()
{
}


TLineEdit::~TLineEdit()
{
}

bool TLineEdit::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_ESCAPE:
	case VK_SPACE:
	case VK_RETURN:
		::SendMessage(hParent, WM_KEYDOWN, wParam, lParam);
		return false;
	default:
		::PostMessage(hParent, WM_KEYDOWN, wParam, lParam);//���͸����ڣ�������ַ����ɸ����ڴ���
		return true;
	}
	return true;
}

bool TLineEdit::OnChar(WPARAM wParam, LPARAM lParam)
{
	return true;
}