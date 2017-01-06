#pragma once
#include "DetectMemoryLeak.h"

#include "TEquations.h"
#include <Windows.h>

TEquations::TEquations()
{
	eError = ERROR_NO;
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

const TCHAR * TEquations::AddEquation(bool output,TCHAR *szInput, bool istemp)
{
	TExpressionTree *temp;
	temp = new TExpressionTree;
	temp->LinkVariableTable(&VariableTable);
	temp->Read(szInput, false);
	temp->Simplify(false);

	if ((eError=temp->GetError()) != ERROR_NO)
	{
		Str += temp->GetErrorInfo();
		delete temp;
		return Str.c_str();
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
	if (eError != ERROR_NO)
		return Str.c_str();

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
		Str = TEXT("\r\nPhi=\r\n[");
		for (int i = 0; i < Equations.size(); i++)
		{
			Str += Equations[i]->OutputStr();
			if (i != Equations.size() - 1)
				Str += TEXT(";\r\n");
		}
		Str += TEXT("]\r\n");

		Str += TEXT("\r\nJacobi=\r\n[");
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
	int index = RowStart;
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
	if (i == j)
		return;
	Vector temp(A[i].size());
	temp = A[i];
	A[i] = A[j];
	A[j] = temp;

	double n;
	n = b[i];
	b[i] = b[j];
	b[j] = n;
}

enumError TEquations::SolveLinear(Matrix &A,Vector &x, Vector &b)
{
	auto m = A.size();//����
	auto n = m;//����=δ֪������
	
	auto RankA = m, RankAb = m;//��ʼֵ

	if (x.size() != m) x.resize(m);//���Է������
	
	if (m != b.size())//Jacobi����������Phi����
		return ERROR_JACOBI_ROW_NOT_EQUAL_PHI_ROW;

	if (m>0)
		if (A[0].size()!=m)//���Ƿ���
		{
			n = A[0].size();
			x.resize(n);
			//return ERROR_INDETERMINATE_EQUATION;
		}

	//����Ԫ��Ԫ��
	for (decltype(m) i = 0; i < m; i++)
	{
		//if (A[i].size() != m)

		//���������������
		SwapRow(A,b, i, GetMaxAbsRowIndex(A,i,m-1, i)); 

		if (abs(A[i][i]) < epsilon)//���ֵΪ0����������
		{
			RankA = i;
			if (abs(b[i]) < epsilon)
				RankAb = i;

			if (RankA != RankAb)//���죬��ϵ��������������Ȳ����->�޽�
				return ERROR_SINGULAR_MATRIX;
			else
				break;//����for���õ��ؽ�
		}

		//���Խ��߻�Ϊ1
		double m_num = A[i][i];
		for (decltype(m) j = i; j < m; j++)
			A[i][j] /= m_num;
		b[i] /= m_num;

		//ÿ�л�Ϊ0
		for (decltype(m) row = i+1; row < m; row++)
		{
			if (abs(A[row][i]) < epsilon)
				;
			else
			{
				double mi = A[row][i];
				for (int col = i; col < m; col++)
				{
					A[row][col] -= A[i][col] * mi;
				}
				b[row] -= b[i] * mi;
			}
		}
	}

	bool bIndeterminateEquation = false;

	//��Ϊ���������飬��ȱ��ȫ��0��������
	if (m != n)
	{
		A.resize(n);
		for (auto i = m ; i < n; i++)
			A[i].resize(n);
		b.resize(n);
		m = n;
		bIndeterminateEquation = true;
	}

	//���û��õ�x
	for (int i = m - 1; i >= 0; i--)//���1��->��1��
	{
		double sum_others = 0.0;
		for (decltype(m) j = i +1; j < m; j++)//���� ���Ԫ�س�����֪x ����
		{
			sum_others += A[i][j] * x[j];
		}
		x[i] = b[i] - sum_others;
	}

	if (RankA < n && RankA == RankAb)
	{
		if (bIndeterminateEquation)
			return ERROR_INDETERMINATE_EQUATION;
		else
			return ERROR_INFINITY_SOLUTIONS;
	}

	return ERROR_NO;
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
	if (eError != ERROR_NO)
		return Str.c_str();
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

		switch (SolveLinear(JacobiValue, DeltaQ, PhiValue))
		{
		case ERROR_SINGULAR_MATRIX:
			//��������
			Str += TEXT("Jacobi�����������޽⣨����ì�ܷ��̣���\r\n");
			delete[] buffer;
			return Str.c_str();
		case ERROR_INDETERMINATE_EQUATION:
			Str += TEXT("���������顣����һ���ؽ⡣\r\n");
			break;
		case ERROR_JACOBI_ROW_NOT_EQUAL_PHI_ROW:
			Str += TEXT("Jacobi������Phi�����������ȣ��������\r\n");
			delete[] buffer;
			return Str.c_str(); 
		case ERROR_INFINITY_SOLUTIONS:
			Str += TEXT("Jacobi�������죬���������⣨���ڵȼ۷��̣�������һ���ؽ⡣\r\n");
			break;
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