#pragma once
#include <Windows.h>
#include "tchar_head.h"
#include <stdio.h>

#include "TControl.h"
class TEdit :public TControl
{
private:
protected:
	TCHAR *Text;
	bool bVisible;

	//������Ϣ��������������
	virtual LRESULT TEdit::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	//����VK_RETURN,VK_ESCAPE������ֵ�����Ƿ����Ĭ����Ϣ����
	virtual bool TEdit::OnKeyDown(WPARAM wParam, LPARAM lParam);

	//�����ַ�����������ֵ�����Ƿ����Ĭ����Ϣ����
	virtual bool TEdit::OnChar(WPARAM wParam, LPARAM lParam){ return true; };

	//�����ַ�����������ֵ�����Ƿ����Ĭ����Ϣ����
	virtual bool TEdit::OnKillFocus(WPARAM wParam, LPARAM lParam){ return true; };
public:
	//bool bSendParentUserMsg=false;//�����true,����յ�WM_KEYDOWNʱ�򸸴��ڷ���WM_USER��Ϣ
	bool bMultiLine,bAutoHScrol,bAutoVScrol,bNoHideSel;
	HFONT m_hFont;
	TEdit();
	~TEdit();
	void TEdit::CreateEditEx(HWND hParent, UINT id, HINSTANCE hInst, DWORD dwStyle = WS_EX_CLIENTEDGE);//����Edit
	void TEdit::SetPos(int x, int y, int width, int height);//���ô�С��λ��
	void TEdit::SetPos(RECT rect);//���ô�С��λ��
	void TEdit::SetVisible(bool bShow);//���ÿɼ���
	void TEdit::SetDefaultGuiFont();
	void CDECL TEdit::SetText(TCHAR szFormat[], ...);//��������
	void TEdit::GetText(TCHAR text[]);
	TCHAR * TEdit::GetText();
	int TEdit::GetLength();//��ȡ�ַ�������
	void TEdit::SetSelect(int iStart, int iEnd);//ѡ��
	void TEdit::SetFont(HFONT hFont);
	void TEdit::SetSelectAll();//ȫѡ
	bool TEdit::GetVisible();
};