#pragma once
#include "DetectMemoryLeak.h"

#include "TEquations.h"
#include <Windows.h>

TEquations::TEquations()
{
	szStr = NULL;
}


TEquations::~TEquations()
{
	//�ͷŷ�����
	for (int i = 0; i < Equations.size(); i++)
		delete Equations[i];

	//�ͷ��ſɱ�
	for (int i = 0; i < Jacobi.size(); i++)
		for (int j = 0; j < Jacobi[i].size(); j++)
			delete Jacobi[i][j];

	if (szStr != NULL)
		delete[] szStr;
}

void TEquations::RemoveTempEquations()
{
	int i;
	i = EquationIsTemp.size() - 1;
	for (; i >-1; i--)
	{
		if (EquationIsTemp[i] == true)
		{
			std::vector<bool>::iterator iter1=EquationIsTemp.begin()+i;
			std::vector<TExpressionTree *>::iterator iter2=Equations.begin()+i;
			EquationIsTemp.erase(iter1);

			delete Equations[i];
			Equations.erase(iter2);
		}
	}
}

size_t TEquations::GetEquationsCount()
{
	return Equations.size();
}

TCHAR * TEquations::AddEquation(bool output,TCHAR *szInput, bool istemp)
{
	TExpressionTree *temp;
	temp = new TExpressionTree;
	temp->LinkVariableTable(&VariableTable);
	temp->Read(szInput, false);
	temp->Simplify(false);

	if (temp->GetError() != ERROR_NO)
	{
		delete temp;
		return temp->GetErrorInfo();
	}

	Equations.push_back(temp);
	EquationIsTemp.push_back(istemp);

	if (output)
		return temp->OutputStr();
	else
		return NULL;
}

const TCHAR * TEquations::BuildJacobi(bool bOutput,TCHAR *subsVar,TCHAR *subsValue)
{
	//�ͷžɵ��ſɱ�
	for (int i = 0; i < Jacobi.size(); i++)
		for (int j = 0; j < Jacobi[i].size(); j++)
			delete Jacobi[i][j];

	TExpressionTree *temp;
	TVariableTable exceptVars;
	//���������滻����������ж���
	if (subsVar!=NULL)
		exceptVars.Define(bOutput,subsVar);

	//�����ſɱȾ���
	Jacobi.clear();
	Jacobi.resize(Equations.size());
	for (int i = 0; i < Equations.size(); i++)
	{
		//�滻�����ܵ�����
		if (subsVar!=NULL && _tcslen(subsVar)>0)
			Equations[i]->Subs(subsVar, subsValue,false);

		Equations[i]->Simplify(false);
		for (int j = 0; j < VariableTable.VariableTable.size(); j++)
		{
			//���ų������������ſɱȾ�����
			if (exceptVars.FindVariableTable(VariableTable.VariableTable[j]) == NULL)//���ų�����
			{
				temp = new TExpressionTree;
				*temp = *Equations[i];
				temp->Diff(VariableTable.VariableTable[j], 1, false);
				temp->Simplify(false);
				Jacobi[i].push_back(temp);
			}
		}
	}

	//�޳������滻���ı���
	if (subsVar!=NULL && _tcslen(subsVar)>0)
		VariableTable.Remove(subsVar);

	//
	if (bOutput)
	{
		Str = TEXT("Phi=\r\n[");
		for (int i = 0; i < Equations.size(); i++)
		{
			Str += Equations[i]->OutputStr();
			if (i != Equations.size() - 1)
				Str += TEXT(";\r\n");
		}
		Str += TEXT("]\r\n");

		Str += TEXT("Jacobi=\r\n[");
		for (int ii = 0; ii < Jacobi.size(); ii++)
		{
			for (int jj = 0; jj < Jacobi[ii].size(); jj++)
			{
				Str += Jacobi[ii][jj]->OutputStr();
				Str += TEXT(" ");
			}
			if (ii != Jacobi.size() - 1)
				Str += TEXT(";\r\n");
		}
		Str += TEXT("]\r\n");
	}
	else
		Str = TEXT("");

	return Str.c_str();
}

void TEquations::Output(Matrix& m)
{
	TCHAR *temp = new TCHAR[20];
	Str += TEXT("[");
	for (int i = 0; i < m.size(); i++)
	{
		for (int j = 0; j < m[i].size(); j++)
		{
			_stprintf(temp, TEXT("%f"), m[i][j]);
			Str += temp;
			Str += TEXT(" ");
		}
		if (i != m.size() - 1)
			Str += TEXT(";\r\n");
	}
	Str += TEXT("]");

	delete[] temp;
}

void TEquations::Output(Vector& v)
{
	TCHAR *temp = new TCHAR[20];
	Str += TEXT("[");
	for (int i = 0; i < v.size(); i++)
	{
		_stprintf(temp, TEXT("%f"), v[i]);
			Str +=temp;
			Str += TEXT(" ");
	}
	Str += TEXT("]");
	delete[] temp;
}

//���ñ������е�ֵ�����ſɱ�
void TEquations::CalcJacobiValue(Matrix &JacobiValue,const Vector &Q)
{
	JacobiValue.clear();
	JacobiValue.resize(Jacobi.size());
	TExpressionTree *temp;
	for (size_t i = 0; i < Jacobi.size();i++)
	{
		for (auto exprJacobi:Jacobi[i])
		{
			temp = new TExpressionTree;
			*temp = *exprJacobi;
			try
			{
				temp->Vpa(false);
				JacobiValue[i].push_back(temp->Value(true));//�õ���ʱ���ʽֵ�����ſɱ�
			}
			catch (enumError& err)
			{
				Str += TEXT("ERROR:");
				Str += temp->OutputStr(true);
				Str += TEXT("\r\nJacobi�������:");
				Str += temp->GetErrorInfo();
				delete temp;
				throw err;
			}
			delete temp;
		}
	}
}

//���ñ������е�ֵ���㣬ÿ��ֵǰ�����˸���
void TEquations::CalcPhiValue(Vector &PhiValue,const Vector &Q)
{
	PhiValue.clear();
	TExpressionTree *temp;
	for (auto PhiExpr : Equations)
	{
			temp = new TExpressionTree;
			*temp = *PhiExpr;
			try
			{
				temp->Vpa(false);
				PhiValue.push_back(-temp->Value(true));//�õ���ʱ���ʽֵ����
			}
			catch (enumError& err)
			{
				Str += TEXT("ERROR:");
				Str += temp->OutputStr(true);
				Str += TEXT("\r\nPhi�������:");
				Str += temp->GetErrorInfo();
				delete temp;
				throw err;
			}
			delete temp;
	}
}

int TEquations::GetMaxAbsRowIndex(const Matrix &A,int RowStart,int RowEnd, int Col)
{
	double max = 0.0;
	int index = 0;
	for (int i = RowStart; i <=RowEnd; i++)
	{
		if (abs(A[i][Col])>max)
		{
			max = abs(A[i][Col]);
			index = i;
		}
	}
	return index;
}

void TEquations::SwapRow(Matrix &A,Vector &b, int i, int j)
{
	Vector temp(A[i].size());
	temp = A[i];
	A[i] = A[j];
	A[j] = temp;

	double n;
	n = b[i];
	b[i] = b[j];
	b[j] = n;
}

bool TEquations::SolveLinear(Matrix &A,Vector &x, Vector &b)
{
	int n = A.size();
	if (x.size() != n) x.resize(n);
	if (n != b.size())
		return false;

	//����Ԫ��Ԫ��
	for (int i = 0; i < n; i++)
	{
		if (A[i].size() != n)//���Ƿ���
			return false;

		SwapRow(A,b, i, GetMaxAbsRowIndex(A,i,n-1, i)); 

		if (abs(A[i][i]) < epsilon)//���ֵΪ0����������
			return false;

		double m = A[i][i];
		for (int j = i; j < n; j++)
			A[i][j] /= m;
		b[i] /= m;

		//ÿ�л�Ϊ0
		for (int row = i+1; row < n; row++)
		{
			if (abs(A[row][i]) < epsilon)
				;
			else
			{
				double mi = A[row][i];
				for (int col = i; col < n; col++)
				{
					A[row][col] -= A[i][col] * mi;
				}
				b[row] -= b[i] * mi;
			}
		}
	}

	//���û��õ�x
	for (int i = n - 1; i >= 0; i--)
	{
		double sum_others = 0.0;
		for (int j = i + 1; j < n; j++)
		{
			sum_others += A[i][j] * x[j];
		}
		x[i] = b[i] - sum_others;
	}
	return true;
}

bool TEquations::AllIs0(Vector &V)
{
	for (int i = 0; i < V.size(); i++)
	{
		if (abs(V[i]) >= epsilon)
			return false;
	}
	return true;
}

bool TEquations::VectorAdd(Vector &Va, const Vector &Vb)
{
	if (Va.size() != Vb.size())
		return false;
	for (int i = 0; i < Va.size(); i++)
	{
		Va[i] += Vb[i];
	}
	return true;
}

const TCHAR * TEquations::SolveEquations(bool bOutput)
{
	hasSolved = false;

	Matrix JacobiValue;
	Vector PhiValue, DeltaQ, &Q = VariableTable.VariableValue;
	TCHAR *buffer = new TCHAR[20];
	int n = 0;

	Str = TEXT("");


	while (1)
	{
		if (bOutput)
		{
			Str += TEXT("q(");
			Str += _itow(n, buffer, 10);
			Str += TEXT(")=\r\n");
			Output(Q);
			Str += TEXT("\r\n");
		}

		try
		{
			CalcJacobiValue(JacobiValue, Q);
		}
		catch (enumError& err)
		{
			Str += TEXT("�޷����㡣\r\n");
			delete[] buffer;
			return Str.c_str();
		}
		if (bOutput)
		{
			Str += TEXT("Jacobi(");
			Str+=_itow(n, buffer,10);
			Str+=TEXT(")=\r\n");
			Output(JacobiValue);
			Str += TEXT("\r\n");
		}

		try
		{
			CalcPhiValue(PhiValue, Q);
		}
		catch (enumError& err)
		{
			Str += TEXT("�޷����㡣\r\n");
			delete[] buffer;
			return Str.c_str();
		}
		if (bOutput)
		{
			Str += TEXT("Phi(");
			Str += _itow(n, buffer, 10);
			Str += TEXT(")=\r\n");
			Output(PhiValue);
			Str += TEXT("\r\n");
		}

		if (SolveLinear(JacobiValue, DeltaQ, PhiValue) == false)//��������
		{
			Str += TEXT("Jacobi�������졣\r\n");
			delete[] buffer;
			return Str.c_str();
		}

		if (bOutput)//���DeltaQ
		{
			Str += TEXT("��q(");
			Str += _itow(n, buffer, 10);
			Str += TEXT(")=\r\n");
			Output(DeltaQ);
			Str += TEXT("\r\n\r\n");
		}

		VectorAdd(Q, DeltaQ);

		if (AllIs0(DeltaQ))
			break;

		if (n > 100)//
		{
			Str += TEXT("����100����δ������\r\n");
			delete[] buffer;
			return Str.c_str();
		}
		n++;
	}

	hasSolved = true;
	VariableTable.VariableValue = Q;

	Str += TEXT("\r\n�õ������\r\n");
	for (int i = 0;i< VariableTable.VariableTable.size(); i++)
	{
		Str += VariableTable.VariableTable[i];
		Str += TEXT(" = ");
		_stprintf(buffer, TEXT("%f"), VariableTable.VariableValue[i]);
		Str += buffer;
		Str += TEXT("\r\n");
	}

	delete[] buffer;
	//szStr = new TCHAR[_tcslen(Str.c_str()) + 1];
	//_tcscpy(szStr, Str.c_str());
	return Str.c_str();
}