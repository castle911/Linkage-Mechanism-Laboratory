#pragma once
#include "DetectMemoryLeak.h"

#include "MyMath.h"
#include <time.h>

#include "TTransfer.h"

#include "TSolver.h"

#include "TMyString.h"

#include "TBar.h"
#include "DPOINT.h"
#include "TMainWindow.h"
#include "TShape.h"
#include "TExpressionTree.h"
#include "TVariableTable.h"
#include "TEquations.h"
#include "TConstraintCoincide.h"
#include "TConstraintColinear.h"

TSolver::TSolver()
{
	hwndOutput = NULL;
	Equations = NULL;
	EquationsV = NULL;
	pStr = NULL;
}


TSolver::~TSolver()
{
	ClearEuqations();

	if (pStr != NULL)
		delete pStr;
}

void TSolver::SetHwnd(HWND hwnd)
{
	hwndOutput = hwnd;
	if (hwnd != NULL)
		pStr = new String;
	else
	{
		if (pStr != NULL)
			delete pStr;
		pStr = NULL;
	}
}

void TSolver::Outputln(const TCHAR *szFormat, ...)
{
	if (pStr!=NULL)
	if (szFormat != NULL && _tcslen(szFormat) > 0)
	{
		TCHAR *szBuffer = new TCHAR[_tcslen(szFormat) + 1024];
		va_list pArgList;
		va_start(pArgList, szFormat);
		_vsntprintf(szBuffer, _tcslen(szFormat) + 1024, szFormat, pArgList);
		va_end(pArgList);

		//追加换行
		*pStr += szBuffer;
		*pStr+=TEXT("\r\n");

		delete[] szBuffer;
	}
}

void TSolver::Output(TCHAR *szFormat, ...)
{
	if (szFormat == NULL || pStr==NULL)
		return;
	TCHAR szBuffer[1024];
	va_list pArgList;
	va_start(pArgList, szFormat);
	_vsntprintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);
	va_end(pArgList);

	*pStr += szBuffer;
}

int TSolver::GetIdFromVariableStr(TCHAR varname[])
{
	int i = -1;
	TCHAR *temp = new TCHAR[10];
	int len = _tcslen(varname);
	TCHAR *end = varname + len;
	while (*varname != TEXT('\0'))
	{
		if (*varname >= TEXT('0') && *varname <= TEXT('9'))
		{
			break;
		}
		varname++;
	}
	_tcsncpy(temp, varname, end - varname + 1);
	i = TTransfer::TCHAR2int(temp);

	delete[] temp;
	return i;
}

void TSolver::ClearConstraint()
{
	Equations->RemoveTempEquations();
	EquationsV->RemoveTempEquations();
}

//添加鼠标约束
void TSolver::AddMouseConstraint(int index, DPOINT dpt)
{
	switch (pShape->Element[index]->eType)
	{
	case ELEMENT_BAR:
	case ELEMENT_REALLINE:
	case ELEMENT_SLIDER:
	{
		TCHAR temp[200];
		TElement *element = pShape->Element[index];
		double xm = dpt.x;
		double ym = dpt.y;
		int i = element->id;

		_stprintf(temp, TEXT("(x%d-%f)*sin(phi%d)-(y%d-%f)*cos(phi%d)"), i, xm, i, i, ym, i);

		TCHAR szVar[100], szValue[100];
		_stprintf(szVar, TEXT("x%d y%d phi%d"), i, i, i);
		_stprintf(szValue, TEXT("%f %f %f"), element->dpt.x, element->dpt.y, element->angle);

		Equations->DefineVariable(pStr, szVar, szValue);
		Equations->AddEquation(pStr, temp, true);

		return;
	}
	case ELEMENT_FRAMEPOINT:
	case ELEMENT_SLIDEWAY:
	case CONSTRAINT_COINCIDE:
		return;
	default:
		assert(0);
		return;
	}
}

void TSolver::ClearEuqations()
{
	if (Equations != NULL)
		delete Equations;
	Equations = new TEquations;

	if (EquationsV != NULL)
		delete EquationsV;
	EquationsV = new TEquations;
}

void TSolver::RefreshEquations()
{
	//全部清理
	_tcscpy(subsVar, TEXT(""));
	_tcscpy(subsValue, TEXT(""));
	ClearOutput();
	ClearEuqations();

	//开始
	Outputln(TEXT("自由度: DOF = nc - nh = nb*3 - nh = %d*3 - %d = %d"), pShape->nb,pShape->nh(), pShape->DOF());
	int i, j;
	DPOINT SiP, SjP,SiQ,SjQ;
	TCHAR buffer1[1024], buffer2[1024];

	Outputln(TEXT("\r\n约束方程:"));

	for (auto element : pShape->Element)
	{
		switch (element->eType)
		{
		case CONSTRAINT_COINCIDE:
		{
			pShape->GetSijP((TConstraintCoincide *)element, SiP, SjP, i, j);

			//得到2个重合构件的广义坐标
			double xi, yi, phii, xj, yj, phij;
			pShape->GetCoordinateByElement(((TConstraintCoincide *)element)->pElement[0], &xi, &yi, &phii);
			pShape->GetCoordinateByElement(((TConstraintCoincide *)element)->pElement[1], &xj, &yj, &phij);

			//定义变量及其初始值
			_stprintf(buffer1, TEXT("x%d y%d phi%d x%d y%d phi%d"), i, i, i, j, j, j);
			_stprintf(buffer2, TEXT("%f %f %f %f %f %f"), xi, yi, phii, xj, yj, phij);
			Equations->DefineVariable(pStr, buffer1, buffer2);
			EquationsV->DefineVariable(pStr, buffer1, buffer2);

			//读入方程
			/* 推导结果：
			xi + xiP*cos(phii) - yiP*sin(phii) - xj  - xjP*cos(phij) + yjP*sin(phij)
			yi + xiP*sin(phii) + yiP*cos(phii) - yj  - xjP*sin(phij) - yjP*cos(phij)  */
			_stprintf(buffer1, TEXT("x%d+%f*cos(phi%d)-%f*sin(phi%d)-x%d-%f*cos(phi%d)+%f*sin(phi%d)"), i, SiP.x, i, SiP.y, i, j, SjP.x, j, SjP.y, j);
			_stprintf(buffer2, TEXT("y%d+%f*sin(phi%d)+%f*cos(phi%d)-y%d-%f*sin(phi%d)-%f*cos(phi%d)"), i, SiP.x, i, SiP.y, i, j, SjP.x, j, SjP.y, j);
			Equations->AddEquation(pStr, buffer1, false);
			Equations->AddEquation(pStr, buffer2, false);
			EquationsV->AddEquation(pStr, buffer1, false);
			EquationsV->AddEquation(pStr, buffer2, false);
			break;
		}
		case CONSTRAINT_COLINEAR:
		{
			TConstraintColinear *pColinear = (TConstraintColinear *)element;
			pShape->GetSP(pColinear->pElement[0], 0, SiP, i);
			pShape->GetSQ(pColinear->pElement[0], 0, SiQ, i);

			pShape->GetSP(pColinear->pElement[1], 0, SjP, j);
			pShape->GetSQ(pColinear->pElement[1], 0, SjQ, j);

			//得到2个重合构件的广义坐标
			double xi, yi, phii, xj, yj, phij;
			pShape->GetCoordinateByElement(pColinear->pElement[0], &xi, &yi, &phii);
			pShape->GetCoordinateByElement(pColinear->pElement[1], &xj, &yj, &phij);

			//定义变量及其初始值
			_stprintf(buffer1, TEXT("x%d y%d phi%d x%d y%d phi%d"), i, i, i, j, j, j);
			_stprintf(buffer2, TEXT("%f %f %f %f %f %f"), xi, yi, phii, xj, yj, phij);
			Equations->DefineVariable(pStr, buffer1, buffer2);
			EquationsV->DefineVariable(pStr, buffer1, buffer2);

			//读入方程
			//推导结果：
			/*
			(cos(phii)*(yiP - yiQ) + sin(phii)*(xiP - xiQ))*(xi - xj + xiP*cos(phii) - xjP*cos(phij) - yiP*sin(phii) + yjP*sin(phij)) 
			- (cos(phii)*(xiP - xiQ) - sin(phii)*(yiP - yiQ))*(yi - yj + yiP*cos(phii) - yjP*cos(phij) + xiP*sin(phii) - xjP*sin(phij))
			*/
			_stprintf(buffer1, TEXT("(cos(phi%d)*(%f-%f)+sin(phi%d)*(%f-%f))*(x%d-x%d+%f*cos(phi%d)-%f*cos(phi%d)-%f*sin(phi%d)+%f*sin(phi%d))\
																	-(cos(phi%d)*(%f-%f)-sin(phi%d)*(%f-%f))*(y%d-y%d+%f*cos(phi%d)-%f*cos(phi%d)+%f*sin(phi%d)-%f*sin(phi%d))"),
																	i, SiP.y, SiQ.y, i, SiP.x, SiQ.x, i, j, SiP.x, i, SjP.x, j, SiP.y, i, SjP.y, j,
																	i, SiP.x, SiQ.x, i, SiP.y, SiQ.y, i, j, SiP.y, i, SjP.y, j, SiP.x, i, SjP.x, j);

			_stprintf(buffer2, TEXT("(cos(phi%d-phi%d)*(%f-%f)+sin(phi%d-phi%d)*(%f-%f))*(%f-%f)-(cos(phi%d-phi%d)*(%f-%f)-sin(phi%d-phi%d)*(%f-%f))*(%f-%f)"),
										i,j,SiP.y,SiQ.y,i,j,SiP.x,SiQ.x,SjP.x,SjQ.x,i,j,SiP.x,SiQ.x,i,j,SiP.y,SiQ.y,SjP.y,SjQ.y);

			Equations->AddEquation(pStr, buffer1, false);
			Equations->AddEquation(pStr, buffer2, false);
			EquationsV->AddEquation(pStr, buffer1, false);
			EquationsV->AddEquation(pStr, buffer2, false);
			break;
		}
		case ELEMENT_SLIDEWAY:
		case ELEMENT_FRAMEPOINT:
		{
			int id = element->id;

			_stprintf(subsVar, TEXT("%s x%d y%d phi%d"), subsVar, id, id, id);
			_stprintf(subsValue, TEXT("%s %f %f %f"), subsValue, element->dpt.x,element->dpt.y, element->angle);
			//Solve时，建立Jacobian会替换掉
			break;
		}
		}
	}

	if (Equations->GetEquationsCount() == 0)
		Outputln(TEXT("\r\n无约束。\r\n"));

	if (pShape->DOF() == 1)
		Outputln(TEXT("\r\n可以拖动。"));
	else
	if (pShape->DOF() > 1)
		Outputln(TEXT("\r\n欠约束。"));

	if (pStr!=NULL)
		RefreshWindowText();
}

void TSolver::ClearOutput()
{
	if (pStr!=NULL)
		*pStr = TEXT("");
}

//必须使用了本函数才刷新Edit内容
void TSolver::RefreshWindowText()
{
	if (hwndOutput != NULL)
	{
		SetWindowText(hwndOutput, pStr->c_str());

		//SetFocus(Edit.m_hWnd);

		int len = ::GetWindowTextLength(hwndOutput);
		::PostMessage(hwndOutput, EM_SETSEL, len, len);
		PostMessage(hwndOutput, EM_SCROLLCARET, 0, 0);
	}
}

void TSolver::SetElementPosition(TVariableTable &VariableTable)
{
	for (size_t i = 0; i < VariableTable.VariableTable.size(); i++)
	{
		enum enumQType{ x, y, phi ,other} eQType=other;
		switch (VariableTable.VariableTable[i][0])
		{
		case TEXT('x'):
			eQType = x;
			break;
		case TEXT('y'):
			eQType = y;
			break;
		case TEXT('p'):
			eQType = phi;
			break;
		default:
			eQType = other;
			break;
		}
		int id = GetIdFromVariableStr(VariableTable.VariableTable[i]);
		TElement *element = pShape->GetElementById(id);
		double &data = VariableTable.VariableValue[i];

		switch (element->eType)
		{
		case ELEMENT_BAR:
		case ELEMENT_REALLINE:
		case ELEMENT_SLIDEWAY:
		{
			TBar *bar = (TBar *)element;
			switch (eQType)
			{
			case x:
				((TBar *)bar)->dpt.x = data;
				break;
			case y:
				((TBar *)bar)->dpt.y = data;
				break;
			case phi:
				data = MakeIn2Pi(data);
				bar->SetPoint(bar->ptBegin, bar->dLength, data);
				break;
			}
			break;
		}
		case ELEMENT_FRAMEPOINT:
		case ELEMENT_SLIDER:
		{
			switch (eQType)
			{
			case x:
				element->dpt.x = data;
				break;
			case y:
				element->dpt.y = data;
				break;
			case phi:
				data = MakeIn2Pi(data);
				element->angle = data;
				break;
			}
			break;
		}
		default:
			assert(0);
			break;
		}
	}
}

//求解
void TSolver::Solve()
{

	clock_t start, stop;//clock_t是clock()函数返回类型
	double duration;

	start = clock();

	Equations->Subs(pStr, subsVar, subsValue);
	Equations->SimplifyEquations(pStr);
	Equations->BuildJacobian(pStr);
	Equations->SolveEquations(pStr);//解方程

	if (Equations->hasSolved)//解出
	{
		SetElementPosition(Equations->VariableTable);
	}

	stop = clock();
	duration = ((double)(stop - start)) / CLK_TCK;

	Outputln(TEXT("\r\n耗时 %f s"), duration);

	if (pStr!=NULL) 
		RefreshWindowText();
}

void TSolver::Demo()
{
	//Outputln(Equations->VariableTable.Define(true, TEXT("x1 y1 phi1 x2 y2 phi2 l t"),TEXT("0 0 0 2.3 0 0 1.3 0")));
	//Outputln(Equations->AddEquation(true, TEXT("x1"), false));
	//Outputln(Equations->AddEquation(true, TEXT("y1"), false));
	//Outputln(Equations->AddEquation(true, TEXT("y2"), false));
	//Outputln(Equations->AddEquation(true, TEXT("x1-x2+cos(phi1)+l*sin(phi2)"), false));
	//Outputln(Equations->AddEquation(true, TEXT("y1-y2+sin(phi1)-l*cos(phi2)"), false));
	//Outputln(Equations->AddEquation(true, TEXT("phi1-t"), false));
	//Outputln(Equations->BuildJacobian(true, TEXT("l t"), TEXT("1.3 0")));

	/*
	Outputln(Equations->DefineVariable(true, TEXT("x2 phi2 z t l"), TEXT("0 0 0 0 0")));
	Outputln(Equations->AddEquation(true, TEXT("-x2+1+1.3*sin(phi2)*l"), false));
	Outputln(Equations->AddEquation(true, TEXT("-1.3*(-phi2/(5+4))-20"), false));
	Outputln(Equations->AddEquation(true, TEXT("2*sin(z)+0.5*cos(z)+t"), false));
	Outputln(Equations->AddEquation(true, TEXT("t-0.3"), false));
	Outputln(Equations->Subs(true, TEXT("l"), TEXT("1.3")));
	Outputln(Equations->SimplifyEquations(true));
	Outputln(Equations->BuildJacobian(true));
	Outputln(Equations->SolveEquations(true));*/

	Equations->RemoveTempEquations();

	Equations->DefineVariable(pStr, TEXT("t"), TEXT("0"));
	Equations->AddEquation(pStr, TEXT("phi2-sin(t)"), true);
	Equations->Subs(pStr, subsVar, subsValue);

	Equations->BuildEquationsV(pStr);//

	Equations->Subs(pStr, TEXT("t"),TEXT("0.1"));
	//Equations->SimplifyEquations(pStr);

	Equations->BuildVariableTableV(pStr);//

	Equations->BuildJacobian(pStr);
	Equations->BuildJacobianV(pStr);//

	Equations->SolveEquations(pStr);

	Equations->SubsV(pStr, TEXT("t"), 0.1);//
	Equations->SolveEquationsV(pStr);//

	SetElementPosition(Equations->VariableTable);
	
	Equations->hasSolved = false;


	//EquationsV->RemoveTempEquations();
	//EquationsV->DefineVariable(pStr, TEXT("t"), TEXT("0"));
	//EquationsV->AddEquation(pStr, TEXT("phi2-sin(t)"), true);
	//EquationsV->Subs(pStr, subsVar, subsValue);
	//EquationsV->BuildJacobian(pStr);
	//for (auto pEqua : Equations->Equations)
	//{
	//	Outputln(pEqua->Diff(TEXT("t"), 1, true));
	//	Outputln(pEqua->Simplify(true));
	//}
	//Equations->SolveEquations(pStr);

	RefreshWindowText();

	//建立ExpressionTree
	//TExpressionTree ex;
	//TVariableTable VarTable;
	//Outputln(VarTable.Define(true,TEXT("  x   y z   ")));
	//Outputln(ex.LinkVariableTable(&VarTable));
	//Outputln(ex.Read(TEXT("sqrt(x/y)"), true));
	//Outputln(ex.Simplify(true));
	//Outputln(ex.Diff(TEXT("x"), 1, true));
	//Outputln(ex.Simplify(true));
	//Outputln(ex.Subs(TEXT("x y"), TEXT(" y 0.1 "), true));
	//Outputln(ex.Simplify(true));
	//Outputln(TEXT("%f"), ex.Value(true));
}