#pragma once
#include "DetectMemoryLeak.h"

#include "TEquations.h"
#include <Windows.h>

#include "TTransfer.h"

TEquations::TEquations()
{
	eError = ERROR_NO;
	VariableTableUnsolved.bShared = true;
	VariableTableSolved.bShared = true;
}


TEquations::~TEquations()
{
	//�ͷŷ�����
	for (int i = 0; i < Equations.size(); i++)
		delete Equations[i];

	//�ͷ��ſɱ�
	for (int i = 0; i < Jacobian.size(); i++)
		for (int j = 0; j < Jacobian[i].size(); j++)
			delete Jacobian[i][j];
}

void TEquations::RemoveTempEquations()
{
	int i;
	i = EquationIsTemp.size() - 1;
	for (; i > -1; i--)
	{
		if (EquationIsTemp[i] == true)
		{
			std::vector<bool>::iterator iter1 = EquationIsTemp.begin() + i;
			std::vector<TExpressionTree *>::iterator iter2 = Equations.begin() + i;
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

const TCHAR * TEquations::DefineVariable(bool bOutput, TCHAR *input_str, TCHAR *input_num)
{
	Str+=VariableTable.Define(bOutput, input_str, input_num);

	VariableTableUnsolved.VariableTable = VariableTable.VariableTable;
	VariableTableUnsolved.VariableValue = VariableTable.VariableValue;

	return Str.c_str();
}

const TCHAR * TEquations::AddEquation(bool output, TCHAR *szInput, bool istemp)
{
	TExpressionTree *temp = new TExpressionTree;
	temp->LinkVariableTable(&VariableTable);
	temp->Read(szInput, false);

	temp->Simplify(false);

	if ((eError = temp->GetError()) != ERROR_NO)//������
	{
		Str += temp->GetErrorInfo();
		delete temp;
		return Str.c_str();
	}

	//���뷽����
	Equations.push_back(temp);
	EquationIsTemp.push_back(istemp);

	hasSolved = false;

	if (output)
		return temp->OutputStr();
	else
		return NULL;
}

const TCHAR * TEquations::BuildJacobian(bool bOutput, TCHAR *subsVar, TCHAR *subsValue)
{
	if (eError != ERROR_NO)
		return Str.c_str();

	//�ͷžɵ��ſɱ�
	for (int i = 0; i < Jacobian.size(); i++)
		for (int j = 0; j < Jacobian[i].size(); j++)
			delete Jacobian[i][j];

	TExpressionTree *temp;
	TVariableTable exceptVars;
	//���������滻����������ж���
	if (subsVar != NULL)
		exceptVars.Define(bOutput, subsVar);

	//�����ſɱȾ���
	Jacobian.clear();
	Jacobian.resize(Equations.size());
	for (int i = 0; i < Equations.size(); i++)
	{
		//�滻�����ܵ�����
		if (subsVar != NULL && _tcslen(subsVar)>0)
			Equations[i]->Subs(subsVar, subsValue, false);

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
				Jacobian[i].push_back(temp);
			}
		}
	}

	//�޳������滻���ı���
	if (subsVar != NULL && _tcslen(subsVar)>0)
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

		Str += TEXT("\r\nJacobian=\r\n[");
		for (int ii = 0; ii < Jacobian.size(); ii++)
		{
			for (int jj = 0; jj < Jacobian[ii].size(); jj++)
			{
				Str += Jacobian[ii][jj]->OutputStr();
				Str += TEXT(" ");
			}
			if (ii != Jacobian.size() - 1)
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
		Str += temp;
		Str += TEXT(" ");
	}
	Str += TEXT("]");
	delete[] temp;
}

//���ñ������е�ֵ�����ſɱ�
void TEquations::CalcJacobianValue(Matrix &JacobianValue, const Vector &Q)
{
	JacobianValue.clear();
	JacobianValue.resize(Jacobian.size());
	TExpressionTree *temp;
	for (size_t i = 0; i < Jacobian.size(); i++)
	{
		for (auto exprJacobian : Jacobian[i])
		{
			temp = new TExpressionTree;
			*temp = *exprJacobian;
			try
			{
				temp->Vpa(false);
				JacobianValue[i].push_back(temp->Value(true));//�õ���ʱ���ʽֵ�����ſɱ�
			}
			catch (enumError& err)
			{
				Str += TEXT("ERROR:");
				Str += temp->OutputStr(true);
				Str += TEXT("\r\nJacobian�������:");
				Str += temp->GetErrorInfo();
				delete temp;
				throw err;
			}
			delete temp;
		}
	}
}

//���ñ������е�ֵ���㣬ÿ��ֵǰ�����˸���
void TEquations::CalcPhiValue(Vector &PhiValue, const Vector &Q)
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

int TEquations::GetMaxAbsRowIndex(const Matrix &A, int RowStart, int RowEnd, int Col)
{
	double max = 0.0;
	int index = RowStart;
	for (int i = RowStart; i <= RowEnd; i++)
	{
		if (abs(A[i][Col]) > max)
		{
			max = abs(A[i][Col]);
			index = i;
		}
	}
	return index;
}

void TEquations::SwapRow(Matrix &A, Vector &b, int i, int j)
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

enumError TEquations::SolveLinear(Matrix &A, Vector &x, Vector &b)
{
	auto m = A.size();//����
	auto n = m;//����=δ֪������

	auto RankA = m, RankAb = m;//��ʼֵ

	if (x.size() != m) x.resize(m);//���Է������

	if (m != b.size())//Jacobian����������Phi����
		return ERROR_JACOBI_ROW_NOT_EQUAL_PHI_ROW;

	if (m > 0)
		if ((n = A[0].size()) != m)//���Ƿ���
		{
			if (m > n)
				return ERROR_OVER_DETERMINED_EQUATIONS;
			x.resize(n);
			//return ERROR_INDETERMINATE_EQUATION;
		}

	std::vector<decltype(m)> TrueRowNumber(n);

	//����Ԫ��Ԫ��
	for (decltype(m) y = 0, x = 0; y < m; y++, x++)
	{
		//if (A[i].size() != m)

		//���������������
		SwapRow(A, b, y, GetMaxAbsRowIndex(A, y, m - 1, x));

		while (abs(A[y][x]) < epsilon)//xһֱ��������0
		{
			x++;
			if (x == n)
				break;

			//���������������
			SwapRow(A, b, y, GetMaxAbsRowIndex(A, y, m - 1, x));
		}

		if (x != n && x > y)
		{
			TrueRowNumber[y] = x;//���뷽��ʱ ��ǰ��Ӧ����x��
		}

		if (x == n)//����ȫΪ0
		{
			RankA = y;
			if (abs(b[y]) < epsilon)
				RankAb = y;

			if (RankA != RankAb)//���죬��ϵ��������������Ȳ����->�޽�
				return ERROR_SINGULAR_MATRIX;
			else
				break;//����for���õ��ؽ�
		}

		//���Խ��߻�Ϊ1
		double m_num = A[y][x];
		for (decltype(m) j = y; j < n; j++)//y�е�j��->��n��
			A[y][j] /= m_num;
		b[y] /= m_num;

		//ÿ�л�Ϊ0
		for (decltype(m) row = y + 1; row < m; row++)//��1��->���1��
		{
			if (abs(A[row][x]) < epsilon)
				;
			else
			{
				double mi = A[row][x];
				for (int col = x; col < n; col++)//row�е�x��->��n��
				{
					A[row][col] -= A[y][col] * mi;
				}
				b[row] -= b[y] * mi;
			}
		}
	}

	bool bIndeterminateEquation = false;//���ô˱�������Ϊ����m��=n��������ж��Ƿ�Ϊ����������

	//��Ϊ���������飬��ȱ��ȫ��0��������
	if (m != n)
	{
		A.resize(n);//A��Ϊn��
		for (auto i = m; i < n; i++)//A��m�п�ʼÿ��n����
			A[i].resize(n);
		b.resize(n);
		m = n;
		bIndeterminateEquation = true;

		//����˳��
		for (int i = m - 1; i >= 0; i--)
		{
			if (TrueRowNumber[i] != 0)
			{
				SwapRow(A, b, i, TrueRowNumber[i]);
			}
		}
	}

	//���û��õ�x
	for (int i = m - 1; i >= 0; i--)//���1��->��1��
	{
		double sum_others = 0.0;
		for (decltype(m) j = i + 1; j < m; j++)//���� ���Ԫ�س�����֪x ����
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

//ţ��-����ɭ�������
const TCHAR * TEquations::SolveEquations(bool bOutput)
{
	if (eError != ERROR_NO)
		return Str.c_str();

	if (hasSolved == false)
	{

		TCHAR *buffer = new TCHAR[20];
		Matrix JacobianValue;
		Vector PhiValue, DeltaQ, &Q = VariableTable.VariableValue;
		Vector VariableValueBackup = VariableTable.VariableValue;
		int n = 0;

		Str = TEXT("");


		while (1)
		{
			if (bOutput)
			{
				Str += TEXT("q(");
				Str += TTransfer::int2TCHAR(n, buffer);
				Str += TEXT(")=\r\n");
				Output(Q);
				Str += TEXT("\r\n");
			}

			try
			{
				CalcJacobianValue(JacobianValue, Q);
			}
			catch (enumError& err)
			{
				Str += TEXT("�޷����㡣\r\n");
				delete[] buffer;
				VariableTable.VariableValue = VariableValueBackup;
				return Str.c_str();
			}

			if (bOutput)
			{
				Str += TEXT("Jacobian(");
				Str += TTransfer::int2TCHAR(n, buffer);
				Str += TEXT(")=\r\n");
				Output(JacobianValue);
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
				VariableTable.VariableValue = VariableValueBackup;
				return Str.c_str();
			}
			if (bOutput)
			{
				Str += TEXT("Phi(");
				Str += TTransfer::int2TCHAR(n, buffer);
				Str += TEXT(")=\r\n");
				Output(PhiValue);
				Str += TEXT("\r\n");
			}

			switch (SolveLinear(JacobianValue, DeltaQ, PhiValue))
			{
			case ERROR_SINGULAR_MATRIX:
				//��������
				Str += TEXT("Jacobian�����������޽⣨����ì�ܷ��̣���\r\n");
				delete[] buffer;
				VariableTable.VariableValue = VariableValueBackup;
				return Str.c_str();
			case ERROR_INDETERMINATE_EQUATION:
				Str += TEXT("���������顣����һ���ؽ⡣\r\n");
				break;
			case ERROR_JACOBI_ROW_NOT_EQUAL_PHI_ROW:
				Str += TEXT("Jacobian������Phi�����������ȣ��������\r\n");
				delete[] buffer;
				VariableTable.VariableValue = VariableValueBackup;
				return Str.c_str();
			case ERROR_INFINITY_SOLUTIONS:
				Str += TEXT("Jacobian�������죬���������⣨���ڵȼ۷��̣�������һ���ؽ⡣\r\n");
				break;
			case ERROR_OVER_DETERMINED_EQUATIONS:
				Str += TEXT("ì�ܷ����飬�޷���⡣\r\n");
				delete[] buffer;
				VariableTable.VariableValue = VariableValueBackup;
				return Str.c_str();
			}

			if (bOutput)//���DeltaQ
			{
				Str += TEXT("��q(");
				Str += TTransfer::int2TCHAR(n, buffer);
				Str += TEXT(")=\r\n");
				Output(DeltaQ);
				Str += TEXT("\r\n\r\n");
			}

			VectorAdd(Q, DeltaQ);

			if (AllIs0(DeltaQ))
				break;

			if (n > 19)//
			{
				Str += TEXT("����20����δ������\r\n");
				delete[] buffer;
				VariableTable.VariableValue = VariableValueBackup;
				return Str.c_str();
			}
			n++;
		}

		hasSolved = true;
		delete[] buffer;
	}
	else
	{
		VariableTable.SetValueByVarTable(VariableTableSolved);
	}

	Str += TEXT("\r\n�õ������\r\n");
	VariableTable.OutputValue(Str);

	return Str.c_str();
}


const TCHAR * TEquations::SimplifyEquations(bool bOutput)//���������еļ򵥷��̽��
{
	std::vector<bool> vecHasSolved(Equations.size(), false);
	//for (auto pExpr : Equations)
	for (size_t i = 0; i < Equations.size(); ++i)
	{
		if (vecHasSolved[i] == false)
		{
			TExpressionTree *pExpr = Equations[i];

			//pExpr->OutputStr();
			//����
			pExpr->Subs(VariableTableSolved.VariableTable, VariableTableSolved.VariableValue, bOutput);
			//pExpr->OutputStr();

			if (pExpr->CheckOnlyOneVar())
			{
				TCHAR *var;
				double value;
				pExpr->Solve(var, value);
				VariableTableSolved.VariableTable.push_back(var);//Ϊ����λ����������������
				VariableTableSolved.VariableValue.push_back(value);

				VariableTableUnsolved.DeleteByAddress(var);

				vecHasSolved[i] = true;
				i = -1;//�ػ����
			}
		}
	}

	//������ѽ������
	for (int i = vecHasSolved.size()-1; i >=0 ; --i)
	{
		if (vecHasSolved[i] == true)
		{
			delete Equations[i];
			auto iter = Equations.begin() + i;
			Equations.erase(iter);

			auto iter2 = EquationIsTemp.begin() + i;
			EquationIsTemp.erase(iter2);
		}
	}

	//
	if (VariableTableUnsolved.VariableTable.size() == 0)
		hasSolved = true;//false��AddEquation����
	
	if (bOutput)
	{
		Str += TEXT("\r\n��ã�\r\n");
		VariableTableSolved.OutputValue(Str);
	}
	return Str.c_str();
}