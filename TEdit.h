#pragma once
#include <Windows.h>
#include "tchar_head.h"
#include <stdio.h>

#include "TControl.h"
class TEdit :public TControl
{
private:
protected:
	//������Ϣ��������������
	virtual LRESULT TEdit::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	//����VK_RETURN,VK_ESCAPE�ȣ������������Ĭ����Ϣ����
	virtual bool TEdit::OnKeyDown(WPARAM wParam, LPARAM lParam){ return true; }

	//�����ַ������������������Ĭ����Ϣ����
	virtual bool TEdit::OnChar(WPARAM wParam, LPARAM lParam){ return true; }

	//����ʧȥ������Ϣ�������������Ĭ����Ϣ����
	virtual bool TEdit::OnKillFocus(WPARAM wParam, LPARAM lParam){ return true; }
public:
	//bool bSendParentUserMsg=false;//�����true,����յ�WM_KEYDOWNʱ�򸸴��ڷ���WM_USER��Ϣ
	bool bMultiLine,bAutoHScrol,bAutoVScrol,bNoHideSel;
	TEdit();
	~TEdit();
	void TEdit::CreateEditEx(HWND hParent, UINT id, HINSTANCE hInst, DWORD dwStyle = WS_EX_CLIENTEDGE);//����Edit
	void TEdit::SetSelect(int iStart, int iEnd);//ѡ��
	void TEdit::SetSelectAll();//ȫѡ
};