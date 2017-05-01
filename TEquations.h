#pragma once
#include "TVariableTable.h"
#include "TExpressionTree.h"
#include "enumError.h"

class TEquations
{
private:
	typedef std::vector<std::vector<double>> TMatrix;
	typedef std::vector<double> TVector;
	typedef std::vector<TExpressionTree *> TPEquations;
	typedef std::vector<TPEquations> TJacobian;

	const double epsilon = 1e-6;
	enumError eError;
	std::vector<bool> EquationIsTemp;

	TJacobian Jacobian;
	TPEquations Equations, EquationsV, EquationsA;
	TVariableTable VariableTableV; //�ٶ��ܱ�����
	TVariableTable VariableTableA; //�ٶ��ܱ�����
	TVariableTable VariableTableSolved;//�ѽ��������
	TVariableTable VariableTableUnsolved;

	void TEquations::CalcPhiValue(String *pStr,const TPEquations &Equations, TVector &PhiValue);
	void TEquations::MatrixMultiplyVector(TVector &Result, const TMatrix &Matrix, const TVector &Vector);
	void TEquations::CalcJacobianValue(String *pStr, TMatrix &JacobianValueResult, const TJacobian &Jacobian);
	int TEquations::GetMaxAbsRowIndex(const TMatrix &A, int RowStart, int RowEnd, int Col);
	void TEquations::SwapRow(TMatrix &A, TVector &b, int i, int j);
	bool TEquations::AllIs0(TVector &V);
	bool TEquations::VectorAdd(TVector &Va, const TVector &Vb);
	void TEquations::Output(String *pStr, TMatrix& m);
	void TEquations::Output(String *pStr, TVector& v);
	void TEquations::ReleaseTPEquations(TPEquations &Equations);
	void TEquations::ReleaseJacobian(TJacobian &Jacobian);
	void TEquations::SubsVar(String *pStr, TPEquations &Equations, TVariableTable &LinkVariableTable, TCHAR *VarStr, double Value);
	void TEquations::BuildJacobian_inner(TJacobian &JacobianResult,const TPEquations &Equations,TVariableTable &VariableTable);
public:
	TEquations();
	~TEquations();

	bool hasSolved;
	TVariableTable VariableTable; //�ܱ�����

	TExpressionTree * TEquations::GetLastExpressionTree()
	{
		return Equations.back();
	}
	void TEquations::BuildEquationsV(String *pStr);
	void TEquations::BuildEquationsA_Phitt(String *pStr);
	void TEquations::CalcEquationsARight(String *pStr, TVector &Right);
	void TEquations::OutputPhi(String *pStr, TPEquations &Equations);
	void TEquations::BuildJacobian(String *pStr);//����Jacobian
	void TEquations::CopyJacobian(TJacobian &Result, const TJacobian &Origin);
	//void TEquations::BuildJacobianV(String *pStr);//����JacobianV
	//void TEquations::BuildJacobianA(String *pStr);//����JacobianA
	void TEquations::OutputJacobian(String *pStr, const TJacobian &Jacobian);
	void TEquations::BuildVariableTableV(String *pStr);
	void TEquations::BuildVariableTableA(String *pStr);
	size_t TEquations::GetEquationsCount();
	void TEquations::AddEquation(String *pStr, TCHAR *szInput, bool istemp);//��ӷ���
	void TEquations::RemoveTempEquations();//�Ƴ���ʱ����
	enumError TEquations::SolveLinear(TMatrix &A, TVector &x, TVector &b);//�����Է����� ϵ��A��δ֪��x
	void TEquations::SolveEquations(String *pStr);//��ⷽ����
	void TEquations::SolveEquationsV(String *pStr);//��ⷽ����V
	void TEquations::SolveEquationsA(String *pStr);//��ⷽ����A
	void TEquations::SimplifyEquations(String *pStr);//���������еļ򵥷��̽��
	void TEquations::DefineVariable(String *pStr, TCHAR *input_str, TCHAR *input_num);
	void TEquations::Subs(String *pStr, TCHAR *subsVar, TCHAR *subsValue);//����
	void TEquations::SubsV(String *pStr, TCHAR *VarStr, double Value);
	void TEquations::SubsA(String *pStr, TCHAR *VarStr, double Value);
	void TEquations::CalcJacobianMultiplyVector(TPEquations &EquationsResult, const TJacobian &Jacobian, const TVector &Vector);

};

