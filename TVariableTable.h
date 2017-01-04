#pragma once
#include <tchar.h>
#include <vector>

#include "String.h"
#include "enumError.h"

class TVariableTable
{
private:
	void TVariableTable::ReleaseVariableTable(std::vector<TCHAR *> &input);
	void TVariableTable::DeleteByAddress(TCHAR *var);
public:
	enumError eError;
	String str;
	std::vector<TCHAR *> VariableTable;
	std::vector<double> VariableValue;
	TVariableTable();
	~TVariableTable();
	TCHAR * TVariableTable::FindVariableTable(TCHAR *varstr);//���ұ����Ƿ��ڱ������У�û���򷵻�NULL
	const TCHAR * TVariableTable::Define(bool bOutput,TCHAR *input,TCHAR *input_num=NULL);
	const TCHAR * TVariableTable::Output();
	const TCHAR * TVariableTable::Remove(const TCHAR input[]);
	double TVariableTable::GetValueFromVarPoint(TCHAR *pVar);
};

