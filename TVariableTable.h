#pragma once
#include <tchar.h>
#include <vector>

class TVariableTable
{
private:
	int input_len;
	TCHAR *info;
	void TVariableTable::ReleaseVariableTable(std::vector<TCHAR *> &input);
	void TVariableTable::DeleteByAddress(TCHAR *var);
public:
	std::vector<TCHAR *> VariableTable;
	std::vector<double> VariableValue;
	TVariableTable();
	~TVariableTable();
	TCHAR * TVariableTable::FindVariableTable(TCHAR *varstr);//���ұ����Ƿ��ڱ������У�û���򷵻�NULL
	TCHAR * TVariableTable::Define(TCHAR *input,TCHAR *input_num=NULL);
	TCHAR * TVariableTable::Output();
	TCHAR * TVariableTable::Remove(TCHAR *input);
};

