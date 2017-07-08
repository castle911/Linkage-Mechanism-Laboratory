#pragma once
#include "tchar_head.h"
#include <vector>

#include "String.h"
#include "enumError.h"

class TVariableTable
{
private:
	void TVariableTable::ReleaseVariableTable(std::vector<TCHAR *> &input);
public:
	bool bShared;//����ǹ����������deleteԪ��
	enumError eError;
	std::vector<TCHAR *> VariableTable;
	std::vector<double> VariableValue;
	TVariableTable();
	~TVariableTable();
	void TVariableTable::DeleteByAddress(TCHAR *var);//ͬʱ�������������
	TCHAR * TVariableTable::FindVariableTable(const TCHAR *varstr);//���ұ����Ƿ��ڱ������У�û���򷵻�NULL
	void TVariableTable::Define(Ostream *pStr,const TCHAR *input_str,const TCHAR *input_num = NULL);
	void TVariableTable::Define(Ostream *pStr,const TCHAR *input_str, double value);
	void TVariableTable::Output(Ostream *pStr);
	void TVariableTable::OutputValue(Ostream *pStr);//��� x=0 ��ʽ
	void TVariableTable::Remove(Ostream *pStr, const TCHAR input_str[]);
	double TVariableTable::GetValueFromVarPoint(TCHAR *pVar);
	void TVariableTable::SetValueFromVarStr(TCHAR *VarStr, double value);
	void TVariableTable::SetValueByVarTable(TVariableTable &VarTable);
};

