#pragma once
#include "tchar_head.h"
#include <vector>
#include <map>

#include "String.h"
#include "enumError.h"

class TVariableTable
{
private:
	void TVariableTable::ReleaseVariableTable(std::vector<TCHAR *> &input);
public:
	bool bShared;//����ǹ����������deleteԪ��
	enumError eError;
	String str;
	//std::map<TCHAR *, double> VariableTable;
	std::vector<TCHAR *> VariableTable;
	std::vector<double> VariableValue;
	TVariableTable();
	~TVariableTable();
	void TVariableTable::DeleteByAddress(TCHAR *var);//ͬʱ�������������
	TCHAR * TVariableTable::FindVariableTable(TCHAR *varstr);//���ұ����Ƿ��ڱ������У�û���򷵻�NULL
	const TCHAR * TVariableTable::Define(bool bOutput, TCHAR *input, TCHAR *input_num = NULL);
	const TCHAR * TVariableTable::Output();
	void TVariableTable::OutputValue(String &s);//��� x=0 ��ʽ
	const TCHAR * TVariableTable::Remove(const TCHAR input[]);
	double TVariableTable::GetValueFromVarPoint(TCHAR *pVar);
	void TVariableTable::SetValueFromVarStr(TCHAR *VarStr, double value);
	void TVariableTable::SetValueByVarTable(TVariableTable &VarTable);
};

