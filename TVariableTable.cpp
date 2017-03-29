#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "DetectMemoryLeak.h"

#include <iostream>
#include <Windows.h>
#include "TVariableTable.h"
#include "TMyString.h"
#include "TTransfer.h"

TVariableTable::TVariableTable()
{
	eError = ERROR_NO;
}


TVariableTable::~TVariableTable()
{
	ReleaseVariableTable(VariableTable);
}

void TVariableTable::ReleaseVariableTable(std::vector<TCHAR *> &input)
{
	for (auto szStr: input)
	{
		delete[] szStr;
	}
	input.clear();
}

double TVariableTable::GetValueFromVarPoint(TCHAR *pVar)
{
	for (size_t i = 0; i < VariableTable.size(); i++)
		if (pVar == VariableTable[i])
			return VariableValue[i];

	eError = ERROR_UNDEFINED_VARIABLE;
	throw eError;
}

TCHAR * TVariableTable::FindVariableTable(TCHAR *varstr)
{
	for (auto szVar:VariableTable)
		if (_tcscmp(varstr, szVar) == 0)
			return szVar;
	return NULL;
}

//����valueһ��ɾ
void TVariableTable::DeleteByAddress(TCHAR *var)
{
	for (size_t i = 0; i < VariableTable.size(); i++)
	{
		if (VariableTable[i] == var)
		{
			delete[] VariableTable[i];
			std::vector<TCHAR *>::iterator iter = VariableTable.begin() + i;
			VariableTable.erase(iter);

			std::vector<double>::iterator iter2 = VariableValue.begin() + i;
			VariableValue.erase(iter2);
			break;
		}
	}
}

const TCHAR * TVariableTable::Remove(const TCHAR input_str[])
{
	TCHAR *input = new TCHAR[_tcslen(input_str) + 1];
	_tcscpy(input, input_str);

	std::vector<TCHAR *> temp;
	TMyString::Trim(input);
	TMyString::ReplaceLoop(input, TEXT("  "), TEXT(" "));
	TMyString::Split(input, temp, TEXT(" "));
	for (auto szInputVar: temp)
	{
		if (TMyString::isVariableName(szInputVar) == false)
		{
			//���ֲ��Ϸ��ı�����
			str = TEXT("���Ϸ��ı���");
			str += szInputVar;
			str += TEXT("��");
			eError = ERROR_INVALID_VARNAME;
			ReleaseVariableTable(temp);
			delete[] input;
			return str.c_str();
		}
	}

	//�ҳ����ڵı���
	TCHAR *var;
	for (size_t i = 0; i < temp.size(); i++)
		if (var=FindVariableTable(temp[i]))
		{
			DeleteByAddress(var);
		}

	ReleaseVariableTable(temp);
	delete[] input;
	return Output();
}

//�¶��������ɵ������ɹ��˵��ض���
const TCHAR * TVariableTable::Define(bool bOutput,TCHAR *input_str,TCHAR *input_num)
{
	//�������봮
	TCHAR *input = new TCHAR[_tcslen(input_str) + 1];
	_tcscpy(input, input_str);

	//�з�str��new��ÿ���±���
	std::vector<TCHAR *> temp;
	TMyString::Trim(input);
	TMyString::ReplaceLoop(input, TEXT("  "), TEXT(" "));
	TMyString::Split(input, temp, TEXT(" "));
	delete[] input;

	std::vector<double> vectorNums;
	if (input_num != NULL)
	{
		TCHAR *str_num = new TCHAR[_tcslen(input_num) + 1];
		_tcscpy(str_num, input_num);

		//�з�str��new��ÿ���±���
		std::vector<TCHAR *> vectorStrNums;
		TMyString::Trim(str_num);
		TMyString::ReplaceLoop(str_num, TEXT("  "), TEXT(" "));
		TMyString::Split(str_num, vectorStrNums, TEXT(" "));
		delete[] str_num;

		//���������ʼֵ�������Ե�
		if (temp.size() != vectorStrNums.size())
		{
			str = TEXT("���������ʼֵ�������Եȡ�");
			eError = ERROR_VAR_COUNT_NOT_EQUAL_NUM_COUNT;
			ReleaseVariableTable(vectorStrNums);
			ReleaseVariableTable(temp);
			return str.c_str();

		}

		for (size_t i = 0; i < vectorStrNums.size(); i++)
		{
			vectorNums.push_back(TTransfer::TCHAR2double(vectorStrNums[i]));//���Ϸ���ֵ��ת��Ϊ0
			delete[] vectorStrNums[i];
		}
	}

	//�˵����Ϸ�����
	for (size_t i = 0; i < temp.size(); i++)
	{
		if (TMyString::isVariableName(temp[i])==false)
		{
			//���ֲ��Ϸ��ı�����
			str = TEXT("���Ϸ��ı���");
			str += temp[i];
			str += TEXT("��");
			eError = ERROR_INVALID_VARNAME;
			ReleaseVariableTable(temp);
			return str.c_str();
		}
	}

	//
	for (size_t i = 0; i < temp.size(); i++)
		if (FindVariableTable(temp[i]))//���еĲ��ٶ���
		{
			delete[] temp[i];
		}
		else
		{//δ����Ľ��ж���
			VariableTable.push_back(temp[i]);
			if (input_num == NULL)
				VariableValue.push_back(0.0);
			else
			VariableValue.push_back(vectorNums[i]);
		}

	if (bOutput)
		return Output();//����������
	else
	{
		str = TEXT("");
		return str.c_str();
	}
}

const TCHAR * TVariableTable::Output()
{
	str = TEXT("�Ѷ������(");

	TCHAR tc[8];
	_itow(VariableTable.size(), tc, 10);
	str += tc;

	str += TEXT("��):");
	for (auto Var: VariableTable)
	{
		str += TEXT(" ");
		str+=Var;
	}

	str += TEXT("\r\n");

	return str.c_str();
}