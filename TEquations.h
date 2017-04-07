#pragma once
#include "TVariableTable.h"
#include "TExpressionTree.h"
#include "enumError.h"

class TEquations
{
private:
	const double epsilon = 1e-6;

	enumError eError;
	typedef std::vector<std::vector<double>> Matrix;
	typedef std::vector<double> Vector;
	std::vector<std::vector<TExpressionTree *>> Jacobian;
	std::vector<TExpressionTree *> Equations,EquationsSolved;
	std::vector<bool> EquationIsTemp;
	void TEquations::CalcPhiValue(String *pStr, Vector &PhiValue, const Vector &Q);
	void TEquations::CalcJacobianValue(String *pStr, Matrix &JacobianValue, const Vector &Q);
	int TEquations::GetMaxAbsRowIndex(const Matrix &A, int RowStart, int RowEnd, int Col);
	void TEquations::SwapRow(Matrix &A, Vector &b, int i, int j);
	bool TEquations::AllIs0(Vector &V);
	bool TEquations::VectorAdd(Vector &Va, const Vector &Vb);
	void TEquations::Output(String *pStr, Matrix& m);
		void TEquations::Output(String *pStr, Vector& v);
	void TEquations::BuildJacobian(String *pStr, TCHAR *subsVar=NULL, TCHAR *subsValue=NULL);//����Jacobian
public:
	TEquations();
	~TEquations();
	bool hasSolved;
	TVariableTable VariableTable; //�ܱ�����
	TVariableTable VariableTableSolved;//�ѽ��������
	TVariableTable VariableTableUnsolved;
	TExpressionTree * TEquations::GetLastExpressionTree()
	{
		return Equations.back();
	}
	size_t TEquations::GetEquationsCount();
	void TEquations::AddEquation(String *pStr, TCHAR *szInput, bool istemp);//��ӷ���
	void TEquations::RemoveTempEquations();//�Ƴ���ʱ����
	enumError TEquations::SolveLinear(Matrix &A, Vector &x, Vector &b);//�����Է����� ϵ��A��δ֪��x
	void TEquations::SolveEquations(String *pStr);//��ⷽ����
	void TEquations::SimplifyEquations(String *pStr);//���������еļ򵥷��̽��
	void TEquations::DefineVariable(String *pStr, TCHAR *input_str, TCHAR *input_num);
	void TEquations::Subs(String *pStr, TCHAR *subsVar, TCHAR *subsValue);//����

};

