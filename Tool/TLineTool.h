#pragma once
#include <Windows.h>
#include <vector>

#include "..\Common\String.h"

#include "TTool.h"
#include "..\Common\DPOINT.h"
#include "..\Element\TElement.h"

class TAttach;
class TLine;
class TRealLine;
class TLineEdit;
class TConstraintCoincide;
class TLineTool :public TTool
{
private:
	POINT ptPrevPos;
	//int iPrevLineId;
	TElement *pPrevLine;
	TAttach *Attach;
	std::vector<DPOINT> dptHit;//ÿ�εĵ��λ��
	bool bShowDimLine;
	POINT pt1, pt2;
	TLine *Line1, *Line2, *LineDim;
	TConstraintCoincide *CoincideBegin;

	//bool bShowDptText;
	//DPOINT dptText;

	TLineEdit *LineEdit;
	void OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnMouseWheel(HWND hWnd, UINT nFlags, POINT ptPos);
	void TLineTool::InitialLine(DPOINT dptPos);
	void Draw(HDC hdc);
protected:
	bool bCanBuildCoincide;
	String sType;

	TRealLine *MoveLine;//��ʾ���λ��
	EnumElementType myElementType;
	virtual TElement * TLineTool::AddIntoShape(TRealLine &RealLine);
	virtual void TLineTool::AddIntoTreeViewContent(TElement *Element, int id);
	virtual void TLineTool::AddCoincide(TConstraintCoincide *pCoincide, int id, TConfiguration *pConfig);//���м���Լ�������˴�
public:
	TLineTool();
	~TLineTool();
};

