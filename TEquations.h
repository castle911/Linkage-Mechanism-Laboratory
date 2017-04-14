#pragma once
#include "TVariableTable.h"
#include "TExpressionTree.h"
#include "enumError.h"

class TEquations
{
private:
	const double epsilon = 1e-6;

	enumError eError;
	typedef std::vector<std::vector<double>> TMatrix;
	typedef std::vector<double> TVector;
	typedef std::vector<TExpressionTree *> TPEquations;
	typedef std::vector<TPEquations> TJacobian;
	TJacobian Jacobian;
	TJacobian JacobianV;
	std::vector<bool> EquationIsTemp;
	void TEquations::CalcPhiValue(String *pStr, TPEquations Equations, TVector &PhiValue);
	void TEquations::CalcJacobianValue(String *pStr, TMatrix &JacobianValue);
	int TEquations::GetMaxAbsRowIndex(const TMatrix &A, int RowStart, int RowEnd, int Col);
	void TEquations::SwapRow(TMatrix &A, TVector &b, int i, int j);
	bool TEquations::AllIs0(TVector &V);
	bool TEquations::VectorAdd(TVector &Va, const TVector &Vb);
	void TEquations::Output(String *pStr, TMatrix& m);
	void TEquations::Output(String *pStr, TVector& v);
	void TEquations::RemoveTPEquations(TPEquations &Equations);
	void TEquations::RemoveJacobian(TJacobian &Jacobian);
	void TEquations::SubsVar(String *pStr, TPEquations &Equations, TVariableTable &LinkVariableTable, TCHAR *VarStr, double Value);
public:
	TEquations();
	~TEquations();
	bool hasSolved;
	TPEquations Equations, EquationsV;
	TVariableTable VariableTable; //�ܱ�����
	TVariableTable VariableTableV; //�ܱ�����
	TVariableTable VariableTableSolved;//�ѽ��������
	TVariableTable VariableTableUnsolved;
	TExpressionTree * TEquations::GetLastExpressionTree()
	{
		return Equations.back();
	}
	void TEquations::BuildEquationsV(String *pStr);
	void TEquations::BuildJacobian(String *pStr);//����Jacobian
	void TEquations::BuildJacobianV(String *pStr);//����JacobianV
	void TEquations::BuildVariableTableV(String *pStr);
	size_t TEquations::GetEquationsCount();
	void TEquations::AddEquation(String *pStr, TCHAR *szInput, bool istemp);//��ӷ���
	void TEquations::RemoveTempEquations();//�Ƴ���ʱ����
	enumError TEquations::SolveLinear(TMatrix &A, TVector &x, TVector &b);//�����Է����� ϵ��A��δ֪��x
	void TEquations::SolveEquations(String *pStr);//��ⷽ����
	void TEquations::SolveEquationsV(String *pStr);//��ⷽ����V
	void TEquations::SimplifyEquations(String *pStr);//���������еļ򵥷��̽��
	void TEquations::DefineVariable(String *pStr, TCHAR *input_str, TCHAR *input_num);
	void TEquations::Subs(String *pStr, TCHAR *subsVar, TCHAR *subsValue);//����
	void TEquations::SubsV(String *pStr, TCHAR *VarStr, double Value);

};

