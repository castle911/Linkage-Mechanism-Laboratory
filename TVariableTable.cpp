#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "DetectMemoryLeak.h"
#include "String.h"
#include <iostream>
#include <Windows.h>
#include "TVariableTable.h"
#include "TMyString.h"
#include "TTransfer.h"

TVariableTable::TVariableTable()
{
	bShared = false;
	eError = ERROR_NO;
}


TVariableTable::~TVariableTable()
{
	if (bShared!=true)
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

void TVariableTable::SetValueFromVarStr(TCHAR *VarStr,double value)
{
	for (size_t i = 0; i < VariableTable.size(); i++)
		if (_tcscmp(VarStr, VariableTable[i]) == 0)
		{
			VariableValue[i] = value;
			return;
		}

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
			if (bShared==false) delete[] VariableTable[i];
			std::vector<TCHAR *>::iterator iter = VariableTable.begin() + i;
			VariableTable.erase(iter);

			std::vector<double>::iterator iter2 = VariableValue.begin() + i;
			VariableValue.erase(iter2);
			break;
		}
	}
}

void TVariableTable::Remove(String *pStr,const TCHAR input_str[])
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
			*pStr+= TEXT("���Ϸ��ı���");
			*pStr += szInputVar;
			*pStr += TEXT("��");
			eError = ERROR_INVALID_VARNAME;
			ReleaseVariableTable(temp);
			delete[] input;
			return;
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
	// return Output();
}

void TVariableTable::Define(String *pStr, TCHAR *input_str, double value)
{
	//�������봮
	TCHAR *ptVar = new TCHAR[_tcslen(input_str) + 1];
	_tcscpy(ptVar, input_str);

	//�˵����Ϸ�����
		if (TMyString::isVariableName(ptVar) == false)
		{
			//���ֲ��Ϸ��ı�����
			if (pStr != NULL)
			{
				*pStr += TEXT("���Ϸ��ı���");
				*pStr += ptVar;
				*pStr += TEXT("��");
			}
			eError = ERROR_INVALID_VARNAME;
			delete[] ptVar;
			return;
		}

	if (pStr != NULL)
		*pStr += TEXT(">>Define: ");
	//
		if (FindVariableTable(ptVar))//���еĲ��ٶ���
		{
			delete[] ptVar;
		}
		else
		{//δ����Ľ��ж���
			VariableTable.push_back(ptVar);
			VariableValue.push_back(value);

			if (pStr != NULL)
			{
				*pStr += VariableTable.back();
				*pStr += TEXT("(");
				*pStr << VariableValue.back();
				*pStr += TEXT(") ");
			}
		}

	if (pStr != NULL)
		*pStr += TEXT("\r\n\r\n");
}

//�¶��������ɵ������ɹ��˵��ض���
void TVariableTable::Define(String *pStr,TCHAR *input_str,TCHAR *input_num)
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
			if (pStr!=NULL) *pStr += TEXT("���������ʼֵ�������Եȡ�");
			eError = ERROR_VAR_COUNT_NOT_EQUAL_NUM_COUNT;
			ReleaseVariableTable(vectorStrNums);
			ReleaseVariableTable(temp);
			return;

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
			if (pStr != NULL)
			{
				*pStr += TEXT("���Ϸ��ı���");
				*pStr += temp[i];
				*pStr += TEXT("��");
			}
			eError = ERROR_INVALID_VARNAME;
			ReleaseVariableTable(temp);
			return;
		}
	}

	if (pStr != NULL)
		*pStr += TEXT(">>Define: ");
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

			if (pStr != NULL)
			{
				*pStr += VariableTable.back();
				*pStr += TEXT("(");
				*pStr << VariableValue.back();
				*pStr += TEXT(") ");
			}
		}

	if (pStr != NULL)
		*pStr += TEXT("\r\n\r\n");
	//if (bOutput)
	//	return Output();//����������
	//else
	//{
	//	str = TEXT("");
	//	return str.c_str();
	//}
}

void TVariableTable::Output(String *pStr)
{
	if (pStr != NULL)
	{
		*pStr += TEXT("�Ѷ������(");

		TCHAR tc[8];
		TTransfer::int2TCHAR(VariableTable.size(), tc);
		*pStr += tc;

		*pStr += TEXT("��):");
		for (auto Var : VariableTable)
		{
			*pStr += TEXT(" ");
			*pStr += Var;
		}

		*pStr += TEXT("\r\n");
	}
}

void TVariableTable::SetValueByVarTable(TVariableTable &VarTable)
{
	for (size_t i = 0; i < VarTable.VariableTable.size(); i++)
	{
			SetValueFromVarStr(VarTable.VariableTable[i], VarTable.VariableValue[i]);
	}
}

void TVariableTable::OutputValue(String *pStr)//��� x=0 ��ʽ
{
	if (pStr != NULL)
	{
		for (size_t i = 0; i < VariableTable.size(); i++)
		{
			*pStr+= VariableTable[i];
			*pStr += TEXT(" = ");
			*pStr << VariableValue[i];
			*pStr += TEXT("\r\n");
		}
		*pStr += TEXT("\r\n");
	}
}