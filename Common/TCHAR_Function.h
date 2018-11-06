#pragma once
#include "..\Common\tchar_head.h"
#include <vector>

class TCHAR_Function
{
public:
	TCHAR_Function();
	~TCHAR_Function();

	//�滻ȫ�� srcԴ�ַ�����sub���滻���ݣ�dest�滻������
	//�����滻��������
	//src��С�����б�֤�������ڲ���ָ֤��Խ��
	static int TCHAR_Function::Replace(TCHAR *src, TCHAR *sub, TCHAR *dest);
	static void TCHAR_Function::ReplaceLoop(TCHAR *src, TCHAR *sub, TCHAR *dest);

	//����ָ���ָ����з��ַ�������result��result�е�Ԫ����new������
	static void TCHAR_Function::Split(TCHAR *src, std::vector<TCHAR *> &result, TCHAR *sub);

	static bool TCHAR_Function::isAlphaCharOrUnderline(TCHAR c);
	static bool TCHAR_Function::isNumberChar(TCHAR c);
	static bool TCHAR_Function::isVariableName(TCHAR *varname);
	static void TCHAR_Function::Trim(TCHAR *str);

	static void TCHAR_Function::ReleaseVectorTCHAR(std::vector<TCHAR *> &szVector);
};

