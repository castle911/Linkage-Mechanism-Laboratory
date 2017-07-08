#pragma once

#include "TTransfer.h"
#include <Windows.h>
#include <stdio.h>//stprintf

double TTransfer::TCHAR2double(TCHAR s[])
{
	return _tcstod(s, NULL);//(TCHAR **)(s + _tcslen(s))
}

double TTransfer::TCHAR2double(TCHAR *start, TCHAR *end)//��ʼ�ַ��������ַ�
{
	int len = end - start;
	TCHAR *temp = new TCHAR[len + 1];
	_tcsncpy(temp, start, len);
	temp[len] = TEXT('\0');
	double value = _tcstod(temp, NULL);
	delete[] temp;
	return value;
}

int TTransfer::TCHAR2int(TCHAR s[])
{
#ifdef _UNICODE
	return _wtoi(s);
#else
	return atoi(s);
#endif
}

TCHAR * TTransfer::int2TCHAR(int i, TCHAR s[], int Radix)
{
#ifdef _UNICODE
	return _itow(i, s, Radix);
#else
	return _itoa(i, s, Radix);
#endif
}

TCHAR * TTransfer::double2TCHAR(double d, TCHAR s[])
{
	_stprintf(s, TEXT("%f"), d);
	return s;
}

TCHAR * TTransfer::double2TCHAR(double d, TCHAR s[], int iDigit)
{
	if (iDigit < 0) iDigit = 0;
	TCHAR szFormat[8] = TEXT("%");
	_stprintf(s, TEXT(".%df"), iDigit);
	_tcscat(szFormat, s);
	_stprintf(s, szFormat, d);

	return s;
}

TCHAR * TTransfer::double2TCHAR_AutoTrim0(double d, TCHAR s[])
{
	int n = 6;//һ��ʼ�ͼٶ�С��λ��Ϊ6λ
	long temp = (long)(d * 1e6);//��6λС��ȫ��ȡ������������������Ŀ���������С������
	for (n = 6; n > 0; n--)
	{
		if (0 != temp % 10) break;
		temp = temp / 10;
	}
	_stprintf(s, TEXT("%.*f"), n, d);
	return s;
}