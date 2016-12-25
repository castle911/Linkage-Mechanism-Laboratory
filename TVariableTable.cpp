#pragma once
#include <Windows.h>
#include "TVariableTable.h"
#include "TMyString.h"
#include "TTransfer.h"

TVariableTable::TVariableTable()
{
	info = NULL;
	input_len = 0;
}


TVariableTable::~TVariableTable()
{
	ReleaseVariableTable(VariableTable);

	if (info != NULL)
		delete[] info;
}

void TVariableTable::ReleaseVariableTable(std::vector<TCHAR *> &input)
{
	for (unsigned int i = 0; i < input.size(); i++)
	{
		delete[] input[i];
	}

}

TCHAR * TVariableTable::FindVariableTable(TCHAR *varstr)
{
	for (int i = 0; i < VariableTable.size(); i++)
		if (_tcscmp(varstr, VariableTable[i]) == 0)
			return VariableTable[i];
	return NULL;
}

//����valueһ��ɾ
void TVariableTable::DeleteByAddress(TCHAR *var)
{
	for (int i = 0; i < VariableTable.size(); i++)
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

TCHAR * TVariableTable::Remove(TCHAR *input)
{
	TCHAR *str = new TCHAR[_tcslen(input) + 1];
	_tcscpy(str, input);

	std::vector<TCHAR *> temp;
	input_len += _tcslen(str);
	TMyString::Trim(str);
	TMyString::ReplaceLoop(str, TEXT("  "), TEXT(" "));
	TMyString::Split(str, temp, TEXT(" "));
	for (int i = 0; i < temp.size(); i++)
	{
		if (TMyString::isVariableName(temp[i]) == false)
		{
			//���ֲ��Ϸ��ı�����
			info = new TCHAR[20 + _tcslen(temp[i])];
			_stprintf(info, TEXT("���Ϸ��ı���%s��"), temp[i]);
			ReleaseVariableTable(temp);
			delete[] str;
			return info;
		}
	}

	//�ҳ����ڵı���
	TCHAR *var;
	for (int i = 0; i < temp.size(); i++)
		if (var=FindVariableTable(temp[i]))
		{
			input_len -= _tcslen(temp[i]);
			DeleteByAddress(var);
		}

	ReleaseVariableTable(temp);
	delete[] str;
	return Output();
}

//�¶��������ɵ������ɹ��˵��ض���
TCHAR * TVariableTable::Define(TCHAR *input,TCHAR *input_num)
{
	//�������봮
	TCHAR *str = new TCHAR[_tcslen(input) + 1];
	_tcscpy(str, input);


	//�з�str��new��ÿ���±���
	std::vector<TCHAR *> temp;
	input_len += _tcslen(str);
	TMyString::Trim(str);
	TMyString::ReplaceLoop(str, TEXT("  "), TEXT(" "));
	TMyString::Split(str, temp, TEXT(" "));
	delete[] str;

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

		for (int i = 0; i < vectorStrNums.size(); i++)
		{
			vectorNums.push_back(TTransfer::TCHAR2double(vectorStrNums[i]));
			delete[] vectorStrNums[i];
		}
	}

	//�˵����Ϸ�����
	for (int i = 0; i < temp.size(); i++)
	{
		if (TMyString::isVariableName(temp[i])==false)
		{
			//���ֲ��Ϸ��ı�����
			info = new TCHAR[20 + _tcslen(temp[i])];
			_stprintf(info, TEXT("���Ϸ��ı���%s��"), temp[i]);
			ReleaseVariableTable(temp);
			return info;
		}
	}

	//
	for (int i = 0; i < temp.size(); i++)
		if (FindVariableTable(temp[i]))
		{
			input_len -= _tcslen(temp[i]);
			delete[] temp[i];
		}
		else
		{
			VariableTable.push_back(temp[i]);
			if (i < vectorNums.size())
				VariableValue.push_back(vectorNums[i]);
			else
				VariableValue.push_back(0.0);
		}

	return Output();
}

TCHAR * TVariableTable::Output()
{
	if (info != NULL)
		delete[] info;

	info = new TCHAR[32 + input_len];
	_stprintf(info, TEXT("�Ѷ������(%d��):"), VariableTable.size());
	for (unsigned int i = 0; i < VariableTable.size(); i++)
	{
		_tcscat(info, TEXT(" "));
		_tcscat(info, VariableTable[i]);
	}

	return info;
}