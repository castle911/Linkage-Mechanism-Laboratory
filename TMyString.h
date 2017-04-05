#pragma once
#include "tchar_head.h"
#include <vector>

class TMyString
{
public:
	TMyString();
	~TMyString();

	//�滻ȫ�� srcԴ�ַ�����sub���滻���ݣ�dest�滻������
	//�����滻��������
	//src��С�����б�֤�������ڲ���ָ֤��Խ��
	static int TMyString::Replace(TCHAR *src, TCHAR *sub, TCHAR *dest);
	static void TMyString::ReplaceLoop(TCHAR *src, TCHAR *sub, TCHAR *dest);

	//����ָ���ָ����з��ַ�������result��result�е�Ԫ����new������
	static void TMyString::Split(TCHAR *src, std::vector<TCHAR *> &result, TCHAR *sub);

	static bool TMyString::isAlphaCharOrUnderline(TCHAR c);
	static bool TMyString::isNumberChar(TCHAR c);
	static bool TMyString::isVariableName(TCHAR *varname);
	static void TMyString::Trim(TCHAR *str);

	static void TMyString::ReleaseVectorTCHAR(std::vector<TCHAR *> &szVector);
};

