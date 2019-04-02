#include "enumError.h"

String GetErrorInfo(enumError err)
{
	switch (err)
	{
	case ERROR_NO:
		return TEXT("�����ɹ���ɡ�");
		break;
	case ERROR_ILLEGALCHAR:
		return TEXT("���󣺳��ַǷ��ַ���");
		break;
	case ERROR_PARENTHESIS_NOT_MATCH:
		return TEXT("�������Ų�ƥ�䡣");
		break;
	case ERROR_INVALID_VARNAME:
		return TEXT("���󣺲���ȷ�ı��������������»���\"_\"��Ӣ����ĸ��ͷ����");
		break;
	case ERROR_WRONG_EXPRESSION:
		return TEXT("���󣺴���ı��ʽ��");
		break;
	case ERROR_EMPTY_INPUT:
		return TEXT("���ʽΪ�ա�");
		break;
	case ERROR_DIVIDE_ZERO:
		return TEXT("���󣺲��ܳ���0��");
		break;
	case ERROR_UNDEFINED_VARIABLE:
		return TEXT("����δ����ı�����");
		break;
	case ERROR_ZERO_POWEROF_ZERO:
		return TEXT("����0��0�η���");
		break;
	case ERROR_SUBS_NOT_EQUAL:
		return TEXT("�����滻�뱻�滻��Ŀ���ȡ�");
		break;
	case ERROR_NOT_LINK_VARIABLETABLE:
		return TEXT("�������δ���ӱ�����");
		break;
	case ERROR_OUTOF_DOMAIN:
		return TEXT("���󣺳���������");
		break;
	case ERROR_VAR_COUNT_NOT_EQUAL_NUM_COUNT:
		return TEXT("���󣺱��������ʼֵ�������Եȡ�");
		break;
	case ERROR_I:
		return TEXT("�ݲ�֧��������");
		break;
	default:
		return TEXT("undefined error");
		break;
	}
}