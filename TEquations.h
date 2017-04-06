#pragma once
#include <string>
#include "TVariableTable.h"
#include "TExpressionTree.h"
#include "enumError.h"

class TEquations
{
private:
	const double epsilon = 1e-6;

	enumError eError;
	std::String Str;
	typedef std::vector<std::vector<double>> Matrix;
	typedef std::vector<double> Vector;
	std::vector<std::vector<TExpressionTree *>> Jacobian;
	std::vector<TExpressionTree *> Equations,EquationsSolved;
	std::vector<bool> EquationIsTemp;
	void TEquations::CalcPhiValue(Vector &PhiValue, const Vector &Q);
	void TEquations::CalcJacobianValue(Matrix &JacobianValue,const Vector &Q);
	int TEquations::GetMaxAbsRowIndex(const Matrix &A, int RowStart, int RowEnd, int Col);
	void TEquations::SwapRow(Matrix &A, Vector &b, int i, int j);
	bool TEquations::AllIs0(Vector &V);
	bool TEquations::VectorAdd(Vector &Va, const Vector &Vb);
	void TEquations::Output(Matrix& m);
	void TEquations::Output(Vector& v);
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
	const TCHAR * TEquations::AddEquation(bool output, TCHAR *input, bool istemp);//��ӷ���
	void TEquations::RemoveTempEquations();//�Ƴ���ʱ����
	const TCHAR * TEquations::BuildJacobian(bool bOutput, TCHAR *subsVar=NULL, TCHAR *subsValue=NULL);//����Jacobian
	enumError TEquations::SolveLinear(Matrix &A, Vector &x, Vector &b);//�����Է����� ϵ��A��δ֪��x
	const TCHAR * TEquations::SolveEquations(bool bOutput);//��ⷽ����
	const TCHAR * TEquations::SimplifyEquations(bool bOutput);//���������еļ򵥷��̽��
	const TCHAR * TEquations::DefineVariable(bool bOutput, TCHAR *input_str, TCHAR *input_num);
	const TCHAR * TEquations::Subs(bool bOutput, TCHAR *subsVar, TCHAR *subsValue);//����

};

