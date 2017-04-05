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
		::SendMessage(m_hParent, WM_KEYDOWN, wParam, lParam);
		return false;
	default:
		::PostMessage(m_hParent, WM_KEYDOWN, wParam, lParam);//���͸����ڣ�������ַ����ɸ����ڴ���
		return true;
	}
	return true;
}

bool TLineEdit::OnChar(WPARAM wParam, LPARAM lParam)
{
	return true;
}