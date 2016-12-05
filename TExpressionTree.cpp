#pragma once
#include "TExpressionTree.h"
#include <Windows.h>
#include "TTransfer.h"

TExpressionTree::TExpressionTree()
{
	head = NULL;
	ErrorInfo = NULL;
	szPostOrder = NULL;
	szInOrder = NULL;
}


TExpressionTree::~TExpressionTree()
{
	Release();
}

void TExpressionTree::Release()
{
	if (ErrorInfo != NULL)
		delete[] ErrorInfo;
	ErrorInfo = NULL;

	if (szPostOrder != NULL)
		delete[] szPostOrder;
	szPostOrder = NULL;

	if (szInOrder != NULL)
		delete[] szInOrder;
	szInOrder = NULL;

	for (UINT i = 0; i < VariableTable.size(); i++)
	{
		delete[] VariableTable[i];
	}
	VariableTable.clear();

	DeleteNode(head);
	head = NULL;
}

void TExpressionTree::DeleteNode(TNode *node)//ɾ��nodeָ����󵫲�����nodeָ��
{
	if (node != NULL)
	{
		std::vector<TNode *> DeleteList;
		DeleteNode(node, DeleteList);

		if (node->parent != NULL)
		{
			if (node->parent->left == node)
				node->parent->left = NULL;
			if (node->parent->right == node)
				node->parent->right = NULL;
		}
		for (int i = 0; i < DeleteList.size(); i++)
		{
			delete DeleteList[i];
		}
	}
}

void TExpressionTree::DeleteNode(TNode *node, std::vector<TNode *> DeleteList)
{
	if (node->left != NULL)
		DeleteNode(node->left);
	if (node->right != NULL)
		DeleteNode(node->right);
	DeleteList.push_back(node);
}

TExpressionTree::enumError TExpressionTree::BuildExpressionTree(std::vector<TNode *> &PostOrder)
{
	std::stack<TNode *> tempStack;
	//���ʶ��PostOrder���У��������ʽ��
	for (int i = 0; i < PostOrder.size(); i++)
	{
		switch (PostOrder[i]->eType)
		{
		case NODE_NUMBER:
		case NODE_VARIABLE:
			tempStack.push(PostOrder[i]);
			break;
		case NODE_FUNCTION:
		case NODE_OPERATOR:
			if (GetOperateNum(PostOrder[i]->eOperator) == 2)
			{
				PostOrder[i]->right = tempStack.top();
				tempStack.top()->parent = PostOrder[i];
				tempStack.pop();
				PostOrder[i]->left = tempStack.top();
				tempStack.top()->parent = PostOrder[i];
				tempStack.pop();

				tempStack.push(PostOrder[i]);
			}
			else
			{
				PostOrder[i]->left = tempStack.top();
				tempStack.top()->parent = PostOrder[i];
				tempStack.pop();

				tempStack.push(PostOrder[i]);
			}
			break;
		}
	}

	//�ҳ�root
	head = PostOrder[0];
	while (head->parent != NULL)
	{
		head = head->parent;
	}
	return ERROR_NO;
}

/* �������������� */
bool TExpressionTree::isLeft2Right(enumMathOperator eOperator)
{
	switch (eOperator)
	{
	case MATH_MOD://%
	case MATH_AND://&
	case MATH_OR://|
	case MATH_MULTIPLY:
	case MATH_DIVIDE:
	case MATH_ADD:
	case MATH_SUBSTRACT:
		return true;

	case MATH_POSITIVE://������Ϊ�ҽ��
	case MATH_NEGATIVE:
	case MATH_POWER://^
		return false;
		//���������Ų��ƽ����
	default:
		return true;
	}
}

/* ��������������ȼ� */
int TExpressionTree::Rank(enumMathOperator eOperator)
{
	switch (eOperator)
	{
	case MATH_SQRT:
	case MATH_SIN:
	case MATH_COS:
	case MATH_TAN:
	case MATH_ARCSIN:
	case MATH_ARCCOS:
	case MATH_ARCTAN:
	case MATH_LN:
	case MATH_LOG10:
	case MATH_EXP:
		return 15;

	case MATH_POSITIVE://���˺�����������������ɽ������ż���
	case MATH_NEGATIVE:
		return 14;

	case MATH_MOD://%
		return 13;

	case MATH_AND://&
	case MATH_OR://|
		return 12;

	case MATH_POWER://^
		return 11;

	case MATH_MULTIPLY:
	case MATH_DIVIDE:
		return 10;

	case MATH_ADD:
	case MATH_SUBSTRACT:
		return 5;

	case MATH_LEFT_PARENTHESIS://�����������ȼ�С��Ϊ�˲��������κ����������
	case MATH_RIGHT_PARENTHESIS:
		return 0;
	}
}

int TExpressionTree::GetOperateNum(TExpressionTree::enumMathOperator eOperator)
{
	switch (eOperator)
	{
	case MATH_SQRT:
	case MATH_SIN:
	case MATH_COS:
	case MATH_TAN:
	case MATH_ARCSIN:
	case MATH_ARCCOS:
	case MATH_ARCTAN:
	case MATH_LN:
	case MATH_LOG10:
	case MATH_EXP:

	case MATH_POSITIVE://������
	case MATH_NEGATIVE:
		return 1;

	case MATH_MOD://%
	case MATH_AND://&
	case MATH_OR://|
	case MATH_POWER://^
	case MATH_MULTIPLY:
	case MATH_DIVIDE:
	case MATH_ADD:
	case MATH_SUBSTRACT:
		return 2;

	case MATH_LEFT_PARENTHESIS:
	case MATH_RIGHT_PARENTHESIS:
		return 0;
	}
}

/* �ǻ�������� */
bool TExpressionTree::isBaseOperator(TCHAR c)
{
	switch (c)
	{
	case TEXT('('):
	case TEXT(')'):
	case TEXT('+'):
	case TEXT('-'):
	case TEXT('*'):
	case TEXT('/'):
	case TEXT('^'):
	case TEXT('&'):
	case TEXT('|'):
	case TEXT('%'):return true;
	}
	return false;
}

TExpressionTree::enumMathOperator TExpressionTree::Str2Function(TCHAR *start, TCHAR *end)
{
	int len = end - start;
	TCHAR *temp = new TCHAR[len + 1];
	_tcsncpy(temp, start, len);
	temp[len] = TEXT('\0');
	if (_tcscmp(temp, TEXT("sin")) == 0)
	{
		delete[] temp;
		return MATH_SIN;
	}
	if (_tcscmp(temp, TEXT("cos")) == 0)
	{
		delete[] temp;
		return MATH_COS;
	}
	if (_tcscmp(temp, TEXT("tan")) == 0)
	{
		delete[] temp;
		return MATH_TAN;
	}
	if (_tcscmp(temp, TEXT("arcsin")) == 0)
	{
		delete[] temp;
		return MATH_ARCSIN;
	}
	if (_tcscmp(temp, TEXT("arccos")) == 0)
	{
		delete[] temp;
		return MATH_ARCCOS;
	}
	if (_tcscmp(temp, TEXT("arctan")) == 0)
	{
		delete[] temp;
		return MATH_ARCTAN;
	}
	if (_tcscmp(temp, TEXT("sqrt")) == 0)
	{
		delete[] temp;
		return MATH_SQRT;
	}
	if (_tcscmp(temp, TEXT("ln")) == 0)
	{
		delete[] temp;
		return MATH_LN;
	}
	if (_tcscmp(temp, TEXT("log10")) == 0)
	{
		delete[] temp;
		return MATH_LOG10;
	}
	if (_tcscmp(temp, TEXT("exp")) == 0)
	{
		delete[] temp;
		return MATH_EXP;
	}
	delete[] temp;
	return MATH_NOT_AVAILIALBE;
}

void TExpressionTree::Function2Str(TExpressionTree::enumMathOperator eOperator, TCHAR *buffer)
{
	switch (eOperator)
	{
	case MATH_SIN:
		_tcscpy(buffer, TEXT("sin"));
		break;
	case MATH_COS:
		_tcscpy(buffer, TEXT("cos"));
		break;
	case MATH_TAN:
		_tcscpy(buffer, TEXT("tan"));
		break;
	case MATH_ARCSIN:
		_tcscpy(buffer, TEXT("arcsin"));
		break;
	case MATH_ARCCOS:
		_tcscpy(buffer, TEXT("arccos"));
		break;
	case MATH_ARCTAN:
		_tcscpy(buffer, TEXT("arctan"));
		break;
	case MATH_SQRT:
		_tcscpy(buffer, TEXT("sqrt"));
		break;
	case MATH_LN:
		_tcscpy(buffer, TEXT("ln"));
		break;
	case MATH_LOG10:
		_tcscpy(buffer, TEXT("log10"));
		break;
	case MATH_EXP:
		_tcscpy(buffer, TEXT("exp"));
		break;

	}
}


/*  */
TExpressionTree::enumMathOperator TExpressionTree::BaseOperatorCharToEnum(TCHAR c)
{
	switch (c)
	{
	case TEXT('('):
		return MATH_LEFT_PARENTHESIS;
	case TEXT(')'):
		return MATH_RIGHT_PARENTHESIS;
	case TEXT('+'):
		return MATH_ADD;
	case TEXT('-'):
		return MATH_SUBSTRACT;
	case TEXT('*'):
		return MATH_MULTIPLY;
	case TEXT('/'):
		return MATH_DIVIDE;
	case TEXT('^'):
		return MATH_POWER;
	case TEXT('&'):
		return MATH_AND;
	case TEXT('|'):
		return MATH_OR;
	case TEXT('%'):
		return MATH_MOD;
	default:
		return MATH_NOT_AVAILIALBE;
	}
}
/*  */
TCHAR TExpressionTree::EnumOperatorToTChar(TExpressionTree::enumMathOperator eOperator)
{
	switch (eOperator)
	{
	case MATH_POSITIVE:
		return TEXT('+');
	case MATH_NEGATIVE:
		return TEXT('-');
	case MATH_LEFT_PARENTHESIS:
		return TEXT('(');
	case MATH_RIGHT_PARENTHESIS:
		return TEXT(')');
	case MATH_ADD:
		return TEXT('+');
	case MATH_SUBSTRACT:
		return TEXT('-');
	case MATH_MULTIPLY:
		return TEXT('*');
	case MATH_DIVIDE:
		return TEXT('/');
	case MATH_POWER:
		return TEXT('^');
	case MATH_AND:
		return TEXT('&');
	case MATH_OR:
		return TEXT('|');
	case MATH_MOD:
		return TEXT('%');
	}
}

/* ��Ч�Լ�飨����0������쳣�ַ��� */
bool TExpressionTree::isLegal(TCHAR c)
{
	if (isDoubleChar(c)) return true;
	if (isBaseOperator(c)) return true;
	if (isAlphaCharOrUnderline(c)) return true;
	return false;
}

/* �ַ���a-zA-z��_ */
bool TExpressionTree::isAlphaCharOrUnderline(TCHAR c)
{
	if ((c >= TEXT('a') && c <= TEXT('z')) || (c >= TEXT('A') && c <= TEXT('Z'))
		|| c == TEXT('_'))
		return true;
	else
		return false;
}

/* �ַ���0-9��. */
bool TExpressionTree::isDoubleChar(TCHAR c)
{
	if ((c >= TEXT('0') && c <= TEXT('9')) || c == TEXT('.'))
		return true;
	else
		return false;
}


/*��in order���еõ�post order����*/
TExpressionTree::enumError TExpressionTree::InQueue2PostQueue(std::queue<TNode *> &InOrder, std::vector<TNode *> &PostOrder)
{
	int parenthesis_num = 0;
	std::stack<TNode *> temp;
	while (InOrder.size() > 0)
	{
		if (InOrder.front()->eType == NODE_NUMBER || InOrder.front()->eType == NODE_VARIABLE)
		{
			PostOrder.push_back(InOrder.front());//����ֱ����ջ
			InOrder.pop();
		}
		else
		{
			if (InOrder.front()->eOperator == MATH_LEFT_PARENTHESIS) //(������ֱ����ջ
			{
				temp.push(InOrder.front());
				InOrder.pop();
				parenthesis_num++;
			}
			else
			{
				if (InOrder.front()->eOperator == MATH_RIGHT_PARENTHESIS)//)����������
				{
					//pop��������
					while (temp.size() > 0)
					{
						if (temp.top()->eOperator == MATH_LEFT_PARENTHESIS)//(
						{
							temp.pop();//�ӵ�������
							parenthesis_num--;
							break;
						}
						else
						{
							PostOrder.push_back(temp.top());//���
							temp.pop();
						}
					}

					//ȡ������
					if (temp.size() > 0 && temp.top()->eType == NODE_FUNCTION)
					{
						PostOrder.push_back(temp.top());
						temp.pop();
					}

					//pop����ȡ��ȡ��
					while (temp.size() > 0)
					{
						if (temp.top()->eOperator == MATH_POSITIVE || temp.top()->eOperator == MATH_NEGATIVE)
						{
							PostOrder.push_back(temp.top());
							temp.pop();
						}
						else
							break;
					}
					InOrder.pop();//�ӵ�������
				}
				else//InOrder.front()��������
				{
					if (InOrder.front()->eOperator == MATH_POSITIVE || InOrder.front()->eOperator == MATH_NEGATIVE)
					{
						temp.push(InOrder.front());
						InOrder.pop();
					}
					else//��������Ҳ����������
					{
						if (temp.size() > 0 && isLeft2Right(temp.top()->eOperator) == true)//����
							while (temp.size() > 0 && Rank(InOrder.front()->eOperator) <= Rank(temp.top()->eOperator))//��ʱջ�����ݣ����½��������ȼ��ͣ��򼷳������ȼ���ͬ���ȼ�����
							{
								PostOrder.push_back(temp.top());//���Ž���post����
								temp.pop();
							}
						else//�ҽ��
							while (temp.size() > 0 && Rank(InOrder.front()->eOperator) < Rank(temp.top()->eOperator))//��ʱջ�����ݣ����½��������ȼ��ͣ��򼷳������ȼ�����������ͬ���ȼ����ţ���Ϊ�ҽ�ϣ�
							{
								PostOrder.push_back(temp.top());//���Ž���post����
								temp.pop();
							};
						temp.push(InOrder.front());//�����ȼ���ȫ����������ǰ������ջ
						InOrder.pop();
					}
				}

			}
		}
	}
	while (temp.size() > 0)
	{
		PostOrder.push_back(temp.top());
		temp.pop();
	}
	if (parenthesis_num > 0)
		return ERROR_PARENTHESIS_NOT_MATCH;
	else
		return ERROR_NO;
}

void TExpressionTree::Node2Str(TNode &node, TCHAR *result)
{
	switch (node.eType)
	{
	case NODE_NUMBER:
		if (abs(node.ValueOrName.value - (int)node.ValueOrName.value) < MIN_DOUBLE)
			_stprintf(result, TEXT("%d"), (int)node.ValueOrName.value);
		else
			_stprintf(result, TEXT("%f"), node.ValueOrName.value);
		break;
	case NODE_VARIABLE:
		_tcscpy(result, node.ValueOrName.varname);
		break;
	case NODE_FUNCTION:
		Function2Str(node.eOperator, result);
		break;
	case NODE_OPERATOR:
		result[0] = EnumOperatorToTChar(node.eOperator);
		result[1] = TEXT('\0');
		break;
	}
}

void TExpressionTree::TraverseInOrder(TNode *now, TCHAR *output, TCHAR *buffer)
{
	int has_parenthesis = 0;
	if (GetOperateNum(now->eOperator) == 1)//һԪ�������������ȡ��
	{
		if (now->eType == NODE_FUNCTION)
		{
			Node2Str(*now, buffer);
			_tcscat(output, buffer);
			_tcscat(output, TEXT("("));
			has_parenthesis = 1;
		}
		else
		{
			_tcscat(output, TEXT("("));
			Node2Str(*now, buffer);
			_tcscat(output, buffer);
			has_parenthesis = 1;
		}
	}

	if (GetOperateNum(now->eOperator) != 1)//��һԪ��������������һԪ����������˳���Ѹı�
	{
		if (now->eType == NODE_OPERATOR)//����Ϊ�����
			//����������ڣ�Ϊ��Ԫ�������ȼ����ڱ��������������
			if ((now->parent != NULL && GetOperateNum(now->parent->eOperator) == 2 && 
				(
				Rank(now->parent->eOperator) > Rank(now->eOperator) 
				||//���������ҽ�������ȼ����
				(Rank(now->parent->eOperator)==Rank(now->eOperator) && (isLeft2Right(now->parent->eOperator)==false && isLeft2Right(now->eOperator)==false))
				)
				)
				||
				//����������ڣ�Ϊ���ţ��ұ���Ϊ��ĸ�����������
				(now->parent != NULL && now->parent->eOperator == MATH_DIVIDE && now == now->parent->right))
			{
				_tcscat(output, TEXT("("));
				has_parenthesis = 1;
			}
	}

	if (now->left != NULL)//�����
	{
		TraverseInOrder(now->left, output, buffer);
	}

	if (GetOperateNum(now->eOperator) != 1)//��һԪ��������������һԪ����������˳���Ѹı�
	{
		Node2Str(*now, buffer);
		_tcscat(output, buffer);
	}


	if (now->right != NULL)//�ұ���
	{
		TraverseInOrder(now->right, output, buffer);
	}

	//�ص�����ʱ���������ţ���סǰ��Ķ���
	if (has_parenthesis)
	{
		_tcscat(output, TEXT(")"));
	}
}

void TExpressionTree::GetNodeNum(TNode *now, int &n)
{
	if (now->left != NULL)
		GetNodeNum(now->left, n);
	if (now->right != NULL)
		GetNodeNum(now->right, n);
	n++;
}

int TExpressionTree::GetNodeNum(TNode *head)
{
	int num = 0;
	if (head != 0)
	{
		GetNodeNum(head, num);
		return num;
	}
	else
		return 0;
}

TCHAR * TExpressionTree::OutputStr()
{
	if (eError == ERROR_NO)
	{
		TCHAR buffer[MAX_VAR_NAME];
		if (szInOrder != NULL)
			delete[] szInOrder;
		szInOrder = new TCHAR[MAX_VAR_NAME * GetNodeNum(head)];
		szInOrder[0] = TEXT('\0');
		if (head != NULL)
			TraverseInOrder(head, szInOrder, buffer);
		return szInOrder;
	}
	else
		return GetErrorInfo(eError);
}

TCHAR * TExpressionTree::OutputPostOrderStr()
{
	if (eError == ERROR_NO)
	{
		TCHAR buffer[MAX_VAR_NAME];
		if (szPostOrder != NULL)
			delete[] szPostOrder;
		szPostOrder = new TCHAR[MAX_VAR_NAME * GetNodeNum(head)];
		szPostOrder[0] = TEXT('\0');
		//for (int i = 0; i < PostOrder.size(); i++)
		//{
		//	Node2Str(*PostOrder[i], buffer);
		//	_tcscat(szPostOrder, buffer);
		//}
		return szPostOrder;
	}
	else
		return GetErrorInfo(eError);
}

TExpressionTree::enumError TExpressionTree::ReadToInOrder(TCHAR *expression, std::queue<TNode *> &InOrder)
{
	if (_tcslen(expression) == 0)
		return ERROR_EMPTY_INPUT;
	Replace(expression, TEXT(" "), TEXT(""));
	Replace(expression, TEXT("\t"), TEXT(""));
	Replace(expression, TEXT("\r"), TEXT(""));
	Replace(expression, TEXT("\n"), TEXT(""));

	//���˵����ж���ļӼ�
	while (1)
	{
		if (Replace(expression, TEXT("--"), TEXT("+")) == 0 &&
			Replace(expression, TEXT("+-"), TEXT("-")) == 0 &&
			Replace(expression, TEXT("-+"), TEXT("-")) == 0)
			break;
	}

	TCHAR *start = expression;
	TCHAR *end = expression + _tcslen(expression);
	TCHAR *now = start;
	TCHAR *nowstart = start;
	TCHAR *nowend = start;

	//�ַ��Ϸ��Լ��
	while (now < end)
	{
		if (isLegal(*now))
			now++;
		else
			return ERROR_ILLEGALCHAR;
	}
	now = start;

	//���з֣�����operator�з�
	struct TStrPiece
	{
		bool bBaseOperator;
		TCHAR *start, *end;
	} tempPiece;
	std::vector<TStrPiece> Data;
	bool PrevIsBaseOperator = isBaseOperator(*now);
	nowstart = now;
	now++;
	while (now < end)
	{
		if (isBaseOperator(*now) != PrevIsBaseOperator)//��������
		{
			tempPiece.bBaseOperator = PrevIsBaseOperator;
			tempPiece.start = nowstart;
			tempPiece.end = now;//��βָ����һλ�����һ���βָ��\0
			Data.push_back(tempPiece);

			PrevIsBaseOperator = isBaseOperator(*now);
			nowstart = now;
		}
		now++;
	}
	tempPiece.bBaseOperator = PrevIsBaseOperator;
	tempPiece.start = nowstart;
	tempPiece.end = end;
	Data.push_back(tempPiece);

	//�����з֣��зֳ�4��Ԫ��
	//������Pre In order
	std::vector<TNode *> PreInOrder;
	TNode *tempNode;
	TCHAR *tempTChar = NULL;
	enumMathOperator tempeOperator;
	for (UINT i = 0; i < Data.size(); i++)
	{
		if (Data[i].bBaseOperator)//ʶ������������������Ҳ�����У�
		{
			now = Data[i].start;
			while (now < Data[i].end)
			{
				tempNode = new TNode;
				ZeroMemory(tempNode, sizeof(TNode));
				tempNode->eType = NODE_OPERATOR;
				tempNode->eOperator = BaseOperatorCharToEnum(*now);
				PreInOrder.push_back(tempNode);
				now++;
			}
		}
		else//
		{
			//��λ�����Ƿ�Ϊ����
			now = Data[i].start;
			bool isDouble = true;
			while (now < Data[i].end)
			{
				if (isDoubleChar(*now) == false)
				{
					isDouble = false;
					break;
				}
				now++;
			}

			if (isDouble)//����
			{
				tempNode = new TNode;
				ZeroMemory(tempNode, sizeof(TNode));
				tempNode->eType = NODE_NUMBER;
				tempNode->ValueOrName.value = TTransfer::TCHAR2double(Data[i].start, Data[i].end);
				PreInOrder.push_back(tempNode);
			}
			else
			{
				if ((tempeOperator = Str2Function(Data[i].start, Data[i].end)) != MATH_NOT_AVAILIALBE)//ʶ�������
				{
					tempNode = new TNode;
					ZeroMemory(tempNode, sizeof(TNode));
					tempNode->eType = NODE_FUNCTION;
					tempNode->eOperator = tempeOperator;
					PreInOrder.push_back(tempNode);
				}
				else//����
				{
					if (isAlphaCharOrUnderline(Data[i].start[0]) == false)//���������ַ���Ϊ�»��߻���ĸ
						return ERROR_INVALID_VARNAME;

					//����������tempTChar
					int len = Data[i].end - Data[i].start;
					tempTChar = new TCHAR[len + 1];
					_tcsncpy(tempTChar, Data[i].start, len);
					tempTChar[len] = TEXT('\0');

					//
					TCHAR *foundvar = NULL;
					if (foundvar = FindVariableTable(tempTChar))
					{
						delete[] tempTChar;
						tempTChar = foundvar;
					}
					else
						VariableTable.push_back(tempTChar);

					tempNode = new TNode;
					ZeroMemory(tempNode, sizeof(TNode));
					tempNode->eType = NODE_VARIABLE;
					tempNode->ValueOrName.varname = tempTChar;
					PreInOrder.push_back(tempNode);

					tempTChar = NULL;
				}
			}
		}
	}

	//ʶ��ȡ���������ȡ�������
	bool bFirstOrParenFirst = false;
	bool bAferOneOperator = false;
	int i = 0;
	if (PreInOrder[0]->eOperator == MATH_ADD)
	{
		PreInOrder[0]->eOperator = MATH_POSITIVE;
		i++;
	}
	if (PreInOrder[0]->eOperator == MATH_SUBSTRACT)
	{
		PreInOrder[0]->eOperator = MATH_NEGATIVE;
		i++;
	}
	for (; i < PreInOrder.size();)
	{
		if (PreInOrder[i]->eType == NODE_OPERATOR && PreInOrder[i]->eOperator != MATH_RIGHT_PARENTHESIS)
		{
			i++;
			if (PreInOrder[i]->eOperator == MATH_ADD)
			{
				PreInOrder[i]->eOperator = MATH_POSITIVE;
				i++;
				continue;
			}
			if (PreInOrder[i]->eOperator == MATH_SUBSTRACT)
			{
				PreInOrder[i]->eOperator = MATH_NEGATIVE;
				i++;
				continue;
			}
		}
		else
			i++;
	}

	//��Ԫ����������������֣���Ԫ��������ɳ������ַ�����β�������ź��һλ��������ǰһλ������������һ������

	for (i = 0; i < PreInOrder.size(); i++)
	{
		InOrder.push(PreInOrder[i]);
	}

	return ERROR_NO;
}

TCHAR * TExpressionTree::GetErrorInfo(TExpressionTree::enumError eError)
{
	if (ErrorInfo != NULL)
		delete[] ErrorInfo;
	ErrorInfo = new TCHAR[64];
	switch (eError)
	{
	case ERROR_NO:
		_tcscpy(ErrorInfo, TEXT("û�д���"));
		break;
	case ERROR_ILLEGALCHAR:
		_tcscpy(ErrorInfo, TEXT("���󣺳��ַǷ��ַ���"));
		break;
	case ERROR_PARENTHESIS_NOT_MATCH:
		_tcscpy(ErrorInfo, TEXT("�������Ų�ƥ�䡣"));
		break;
	case ERROR_INVALID_VARNAME:
		_tcscpy(ErrorInfo, TEXT("���󣺲���ȷ�ı��������������»���\"_\"��Ӣ����ĸ��ͷ����"));
		break;
	case ERROR_EMPTY_INPUT:
		_tcscpy(ErrorInfo, TEXT("���ʽΪ�ա�"));
		break;
	case ERROR_DIVIDE_ZERO:
		_tcscpy(ErrorInfo, TEXT("���󣺲��ó���0��"));
		break;
	case ERROR_UNKNOWN_VARIABLE:
		_tcscpy(ErrorInfo, TEXT("���󣺳���δ֪������"));//���������б�����������
		break;
	case ERROR_ZERO_POWEROF_ZERO:
		_tcscpy(ErrorInfo, TEXT("����0��0�η���"));
		break;
	case ERROR_SUBS_NOT_EQUAL:
		_tcscpy(ErrorInfo, TEXT("�����滻�뱻�滻��Ŀ���ȡ�"));
		break; 
	}
	return ErrorInfo;
}

void TExpressionTree::CalcNode(TNode *Operator, TNode *Node1, TNode *Node2 = NULL)
{
	Operator->eType = NODE_NUMBER;
	switch (Operator->eOperator)
	{
	case MATH_SQRT:
		Operator->ValueOrName.value = sqrt(Node1->ValueOrName.value);
		break;
	case MATH_SIN:
		Operator->ValueOrName.value = sin(Node1->ValueOrName.value);
		break;
	case MATH_COS:
		Operator->ValueOrName.value = cos(Node1->ValueOrName.value);
		break;
	case MATH_TAN:
		Operator->ValueOrName.value = tan(Node1->ValueOrName.value);
		break;
	case MATH_ARCSIN:
		Operator->ValueOrName.value = asin(Node1->ValueOrName.value);
		break;
	case MATH_ARCCOS:
		Operator->ValueOrName.value = acos(Node1->ValueOrName.value);
		break;
	case MATH_ARCTAN:
		Operator->ValueOrName.value = atan(Node1->ValueOrName.value);
		break;
	case MATH_LN:
		Operator->ValueOrName.value = log(Node1->ValueOrName.value);
		break;
	case MATH_LOG10:
		Operator->ValueOrName.value = log10(Node1->ValueOrName.value);
		break;
	case MATH_EXP:
		Operator->ValueOrName.value = exp(Node1->ValueOrName.value);
		break;
	case MATH_POSITIVE:
		break;
	case MATH_NEGATIVE:
		Operator->ValueOrName.value = -Node1->ValueOrName.value;
		break;

	case MATH_MOD://%
		Operator->ValueOrName.value = (int)Node1->ValueOrName.value % (int)Node2->ValueOrName.value;
		break;
	case MATH_AND://&
		Operator->ValueOrName.value = (int)Node1->ValueOrName.value & (int)Node2->ValueOrName.value;
		break;
	case MATH_OR://|
		Operator->ValueOrName.value = (int)Node1->ValueOrName.value | (int)Node2->ValueOrName.value;
		break;

	case MATH_POWER://^
		Operator->ValueOrName.value = pow(Node1->ValueOrName.value, Node2->ValueOrName.value);
		break;

	case MATH_MULTIPLY:
		Operator->ValueOrName.value = Node1->ValueOrName.value * Node2->ValueOrName.value;
		break;
	case MATH_DIVIDE:
		Operator->ValueOrName.value = Node1->ValueOrName.value / Node2->ValueOrName.value;
		break;

	case MATH_ADD:
		Operator->ValueOrName.value = Node1->ValueOrName.value + Node2->ValueOrName.value;
		break;
	case MATH_SUBSTRACT:
		Operator->ValueOrName.value = Node1->ValueOrName.value - Node2->ValueOrName.value;
		break;
	}
}


TExpressionTree::TNode * TExpressionTree::CopyNodeTree(TNode *oldNode)
{
	TNode *newNode;
	newNode = new TNode;
	ZeroMemory(newNode, sizeof(TNode));
	newNode->eType = oldNode->eType;
	newNode->eOperator = oldNode->eOperator;
	newNode->ValueOrName = oldNode->ValueOrName;

	if (oldNode->left != NULL)
	{
		newNode->left = CopyNodeTree(oldNode->left);
		newNode->left->parent = newNode;
	}
	if (oldNode->right != NULL)
	{
		newNode->right = CopyNodeTree(oldNode->right);
		newNode->right->parent = newNode;
	}

	return newNode;
}

void TExpressionTree::Diff(TNode *now, TCHAR *var)//varΪָ�������ָ�룬һ��λ�ڱ�������
{
	switch (now->eType)
	{
	case NODE_VARIABLE:
		now->eType = NODE_NUMBER;
		if (now->ValueOrName.varname == var)
			now->ValueOrName.value = 1;
		else
			now->ValueOrName.value = 0;
		return;
	case NODE_NUMBER:
		now->ValueOrName.value = 0;
		return;
	case NODE_OPERATOR://��ǰΪ������ڵ�
		switch (now->eOperator)
		{
		case MATH_POSITIVE:
		case MATH_NEGATIVE:
			if (now->left != NULL)
				Diff(now->left, var);
			return;
		case MATH_ADD:
		case MATH_SUBSTRACT:
			if (now->left != NULL)
				Diff(now->left, var);
			if (now->right != NULL)
				Diff(now->right, var);
			return;
		case MATH_MULTIPLY:
			if (now->left->eType == NODE_NUMBER || now->right->eType == NODE_NUMBER)//��������������һ��������
			{
				if (now->left->eType == NODE_NUMBER)
					Diff(now->right, var);
				else
					Diff(now->left, var);
				return;
			}
			else
			{
				TNode *plus;
				plus = new TNode;
				ZeroMemory(plus, sizeof(TNode));
				plus->eType = NODE_OPERATOR;
				plus->eOperator = MATH_ADD;
				if (now != head)
				{
					//plus����������
					if (now->parent->left == now)
						now->parent->left = plus;
					if (now->parent->right == now)
						now->parent->right = plus;
					plus->parent = now->parent;
				}
				else
				{
					head = plus;
				}
				now->parent = plus;
				plus->left = now;

				//�����ҽڵ�
				TNode *newRight;
				newRight = CopyNodeTree(now);

				plus->right = newRight;
				newRight->parent = plus;

				Diff(plus->left->left, var);
				Diff(plus->right->right, var);
				return;
			}
		case MATH_DIVIDE:
			if (now->right->eType == NODE_NUMBER)// f(x)/number = f'(x)/number
			{
				Diff(now->left, var);
				return;
			}
			else
			{
				TNode *divide = now;
				TNode *u1 = now->left;
				TNode *v1 = now->right;

				//��������
				TNode *substract;
				substract = new TNode;
				ZeroMemory(substract, sizeof(TNode));
				substract->eType = NODE_OPERATOR;
				substract->eOperator = MATH_SUBSTRACT;

				//����2���˺�
				TNode *multiply1, *multiply2;
				multiply1 = new TNode;
				multiply2 = new TNode;
				ZeroMemory(multiply1, sizeof(TNode));
				ZeroMemory(multiply2, sizeof(TNode));
				multiply1->eType = NODE_OPERATOR;
				multiply2->eType = NODE_OPERATOR;
				multiply1->eOperator = MATH_MULTIPLY;
				multiply2->eOperator = MATH_MULTIPLY;

				//�����˷�
				TNode *power;
				power = new TNode;
				ZeroMemory(power, sizeof(TNode));
				power->eType = NODE_OPERATOR;
				power->eOperator = MATH_POWER;

				//���ӳ�������2���ڵ㣺-, ^
				divide->left = substract;
				substract->parent = divide;
				divide->right = power;
				power->parent = divide;

				//���Ӽ�������2���ڵ�
				substract->left = multiply1;
				multiply1->parent = substract;
				substract->right = multiply2;
				multiply2->parent = substract;

				//���ӳ˺�1����2���ڵ㣺u1, v1
				multiply1->left = u1;
				u1->parent = multiply1;
				multiply1->right = v1;
				v1->parent = multiply1;

				//����u2, v2
				TNode *u2, *v2;
				u2 = CopyNodeTree(u1);
				v2 = CopyNodeTree(v1);

				//���ӳ˺�2�����u2, v2
				multiply2->left = u2;
				u2->parent = multiply2;
				multiply2->right = v2;
				v2->parent = multiply2;

				//����v3, 2
				TNode *v3, *num2;
				v3 = CopyNodeTree(v1);
				num2 = new TNode;
				ZeroMemory(num2, sizeof(TNode));
				num2->eType = NODE_NUMBER;
				num2->eOperator = MATH_NOT_AVAILIALBE;
				num2->ValueOrName.value = 2;

				//����^�����v3��2
				power->left = v3;
				v3->parent = power;
				power->right = num2;
				num2->parent = power;

				Diff(u1, var);
				Diff(v2, var);

				return;
			}
		case MATH_POWER:
			return;
		}
	case NODE_FUNCTION:
	{
		//�����Ƕ�����
		//������Ϊ���ֵ�Ϊ0
		if (now->left->eType == NODE_NUMBER)
		{
			now->eType = NODE_NUMBER;
			now->eOperator = MATH_NOT_AVAILIALBE;
			now->ValueOrName.value = 0;
			DeleteNode(now->left);
			now->left = NULL;
		}

		TNode *function = now;
		switch (function->eOperator)
		{
		case MATH_COS:
			TNode *negative;
			negative = new TNode;
			ZeroMemory(negative, sizeof(TNode));
			negative->eType = NODE_OPERATOR;
			negative->eOperator = MATH_NEGATIVE;

			//������һ���͸���
			if (function != head)
			{
				if (function->parent->left == function)
				{
					function->parent->left = negative;
					negative->parent = function->parent;
				}
				if (function->parent->right == function)
				{
					function->parent->right = negative;
					negative->parent = function->parent;
				}
			}
			else
			{
				head = negative;
				negative->parent = NULL;
			}
		case MATH_SIN:
			TNode *multiply = new TNode;
			ZeroMemory(multiply, sizeof(TNode));
			multiply->eType = NODE_OPERATOR;
			multiply->eOperator = MATH_MULTIPLY;

			switch (function->eOperator)
			{
			case MATH_SIN:
				//������һ���ͳ˺�
				if (function != head)
				{
					if (function->parent->left == function)
					{
						function->parent->left = multiply;
						multiply->parent = function->parent;
					}
					if (function->parent->right == function)
					{
						function->parent->right = multiply;
						multiply->parent = function->parent;
					}
				}
				else
				{
					head = multiply;
					multiply->parent = NULL;
				}
				break;
			case MATH_COS:
				//���Ӹ��źͳ˺�
				negative->left = multiply;
				multiply->parent = negative;
				break;
			}

			//���ӳ˺ź�function
			multiply->left = function;
			function->parent = multiply;

			//���function
			switch (function->eOperator)
			{
			case MATH_SIN:
				function->eOperator = MATH_COS;
				break;
			case MATH_COS:
				function->eOperator = MATH_SIN;
				break;
			}

			//����u2�����ӳ˺�
			TNode *u2 = CopyNodeTree(function->left);
			multiply->right = u2;
			u2->parent = multiply;

			Diff(u2, var);
			return;
		}
	}
	}
}

TCHAR * TExpressionTree::Diff(TCHAR *var, int n, bool bOutput)
{
	if (eError == ERROR_NO)
	{
		TCHAR *Variable = FindVariableTable(var);
		//û���ҵ���Ӧ����������Ϊ�գ����޹ر�������0

		for (int i = 0; i < n; i++)
		{
			Diff(head, Variable);
		}
		//return Simplify(bOutput);
		return OutputStr();
	}

	return GetErrorInfo(eError);
}

TExpressionTree::enumError TExpressionTree::Simplify(TNode *now)
{
	TExpressionTree::enumError nowError = ERROR_NO;
	if (now->left != NULL)
		nowError = Simplify(now->left);

	if (now->right != NULL)
		nowError = Simplify(now->right);

	if (nowError != ERROR_NO)
		return nowError;

	if (GetOperateNum(now->eOperator) == 2)
	{
		//����ÿ��������뻥�⣬��Ϊÿ��������з���ֵ���漰ɾ��������������������ִ�н�����ָ�����
		//��������Ҷ����Ƿ���ڣ���Ϊ�˴��������2Ԫ�����
		bool LChildIs0 = (now->left->eType == NODE_NUMBER && abs(now->left->ValueOrName.value) < MIN_DOUBLE);
		bool RChildIs0 = (now->right->eType == NODE_NUMBER && abs(now->right->ValueOrName.value) < MIN_DOUBLE);
		bool LChildIs1 = (now->left->eType == NODE_NUMBER && abs(now->left->ValueOrName.value - 1) < MIN_DOUBLE);
		bool RChildIs1 = (now->right->eType == NODE_NUMBER && abs(now->right->ValueOrName.value - 1) < MIN_DOUBLE);

		//������Ϊ0
		if (now->eOperator == MATH_DIVIDE && RChildIs0)
			return ERROR_DIVIDE_ZERO;

		//0��0�η�
		if (now->eOperator == MATH_POWER && LChildIs0 && RChildIs0)
			return ERROR_ZERO_POWEROF_ZERO;

		//�����Ҷ��Ӷ������֣���������
		if (now->left->eType == NODE_NUMBER && now->right->eType == NODE_NUMBER)
		{
			CalcNode(now, now->left, now->right);
			delete now->left;
			delete now->right;
			now->eOperator = MATH_NOT_AVAILIALBE;
			now->left = NULL;
			now->right = NULL;
			return ERROR_NO;
		}

		//�κνڵ��0�η�������1������0��0�η�����ǰ�汨��
		if (now->eOperator == MATH_POWER && RChildIs0)
		{
			//�滻����ǰ����������1�ڵ㽫�ڻ���ʱ����
			//�½�һ��1�ڵ�
			TNode *temp;
			temp = new TNode;
			ZeroMemory(temp, sizeof(TNode));
			temp->eType = NODE_NUMBER;
			temp->ValueOrName.value = 1;

			//0�ڵ����ӵ�����
			if (now != head)
			{
				if (now->parent->left == now)
				{
					now->parent->left = temp;
					temp->parent = now->parent;
				}
				if (now->parent->right == now)
				{
					now->parent->right = temp;
					temp->parent = now->parent;
				}
			}
			else
				head = temp;

			DeleteNode(now);
			return ERROR_NO;
		}

		//�κ����˻򱻳�0����0����0�ĳ�0����κδη�������0
		if ((now->eOperator == MATH_MULTIPLY && (LChildIs0 || RChildIs0)) ||
			(now->eOperator == MATH_DIVIDE && LChildIs0) ||
			(now->eOperator == MATH_POWER && LChildIs0))
		{
			//�滻����ǰ����������0�ڵ㽫�ڻ���ʱ����
			//�½�һ��0�ڵ�
			TNode *temp;
			temp = new TNode;
			ZeroMemory(temp, sizeof(TNode));
			temp->eType = NODE_NUMBER;
			temp->ValueOrName.value = 0;

			//0�ڵ����ӵ�����
			if (now != head)
			{
				if (now->parent->left == now)
				{
					now->parent->left = temp;
					temp->parent = now->parent;
				}
				if (now->parent->right == now)
				{
					now->parent->right = temp;
					temp->parent = now->parent;
				}
			}
			else
				head = temp;

			DeleteNode(now);
			return ERROR_NO;
		}

		//0-x=-x
		if (now->eOperator == MATH_SUBSTRACT && LChildIs0)
		{
			TNode *LChild = now->left;
			TNode *RChild = now->right;
			now->eOperator = MATH_NEGATIVE;
			now->left = RChild;
			now->right = NULL;

			delete LChild;
		}

		//�κ����ӻ򱻼�0������0���˻򱻳�1����1������1�η�����������
		if (
			(now->eOperator == MATH_ADD && (LChildIs0 || RChildIs0)) ||
			(now->eOperator == MATH_SUBSTRACT && RChildIs0) ||
			(now->eOperator == MATH_MULTIPLY && (LChildIs1 || RChildIs1)) ||

			(now->eOperator == MATH_DIVIDE && RChildIs1) ||
			(now->eOperator == MATH_POWER && RChildIs1))
		{
			TNode *remain, *num;
			if (LChildIs1 || LChildIs0)
			{
				num = now->left;
				remain = now->right;
			}
			if (RChildIs1 || RChildIs0)
			{
				num = now->right;
				remain = now->left;
			}

			//���Ӹ�����ʣ����
			if (now != head)
			{
				if (now->parent->left == now)
				{
					now->parent->left = remain;
					remain->parent = now->parent;
				}
				if (now->parent->right == now)
				{
					now->parent->right = remain;
					remain->parent = now->parent;
				}
			}
			else
			{
				head = remain;
				head->parent = NULL;
			}
			delete num;
			delete now;
			return ERROR_NO;
		}


	}
	return nowError;
}

TCHAR * TExpressionTree::OutputEmptyStr()
{
	szInOrder = new TCHAR[1];
	_tcscpy(szInOrder, TEXT(""));
	return szInOrder;
}

TCHAR * TExpressionTree::Simplify(bool bOutput)
{
	if (eError == ERROR_NO)
	{
		TNode *temp = head;
		if ((eError = Simplify(head)) == ERROR_NO)
			if (bOutput)
				return OutputStr();
			else
				return OutputEmptyStr();
	}

	return GetErrorInfo(eError);
}

TCHAR * TExpressionTree::FindVariableTableFrom(TCHAR *varstr, std::vector<TCHAR *> newVariableTable)
{
	for (int i = 0; i < newVariableTable.size(); i++)
		if (_tcscmp(varstr, newVariableTable[i]) == 0)
			return newVariableTable[i];
	return NULL;
}

TCHAR * TExpressionTree::FindVariableTable(TCHAR *varstr)
{
	for (int i = 0; i < VariableTable.size(); i++)
		if (_tcscmp(varstr, VariableTable[i]) == 0)
			return VariableTable[i];
	return NULL;
}

void TExpressionTree::ReplaceNodeVariable(TNode *now, std::vector<TCHAR *> &newVariableTable)
{
	if (now->left != NULL)
		ReplaceNodeVariable(now->left, newVariableTable);
	if (now->right != NULL)
		ReplaceNodeVariable(now->right, newVariableTable);
	TCHAR *temp;
	if (now->eType == NODE_VARIABLE && (temp = FindVariableTableFrom(now->ValueOrName.varname, newVariableTable)))
		now->ValueOrName.varname = temp;
}

void TExpressionTree::GetVariablePos(const TCHAR *var, std::vector<TNode *> &VarsPos)
{
	GetVariablePos(head, var, VarsPos);
}

void TExpressionTree::GetVariablePos(TNode *now,const TCHAR *var, std::vector<TNode *> &VarsPos)
{
	if (now->eType == NODE_VARIABLE && now->ValueOrName.varname == var)
		VarsPos.push_back(now);
	if (now->left != NULL)
		GetVariablePos(now->left, var, VarsPos);
	if (now->right != NULL)
		GetVariablePos(now->right, var, VarsPos);
}

void TExpressionTree::CopyVariableTable(std::vector<TCHAR *> &Dest, const std::vector<TCHAR *> source)
{
	Dest.clear();
	for (int i = 0; i < source.size(); i++)
		Dest.push_back(source[i]);
}

TCHAR * TExpressionTree::Subs(TCHAR *vars, TCHAR *nums)
{
	if (eError == ERROR_NO)
	{
		if (_tcslen(vars) == 0 || _tcslen(nums) == 0)
			return GetErrorInfo(eError = ERROR_EMPTY_INPUT);
		TCHAR *nVars = new TCHAR[_tcslen(vars) + 1];
		TCHAR *nNums = new TCHAR[_tcslen(nums) + 1];
		_tcscpy(nVars, vars);
		_tcscpy(nNums, nums);
		Replace(nVars, TEXT(" "), TEXT(""));
		Replace(nNums, TEXT(" "), TEXT(""));
		std::vector<TCHAR *> VarsVector;
		std::vector<TCHAR *> NumsVector;
		Split(nVars, VarsVector, TEXT(","));
		Split(nNums, NumsVector, TEXT(","));

		if (VarsVector.size() == NumsVector.size())//�滻�뱻�滻Ԫ����Ŀ���
		{
			for (int i = 0; i < VarsVector.size(); i++)//�������滻����
			{
				//���ʶ���滻����
				TCHAR *var = FindVariableTable(VarsVector[i]);
				if (var != NULL)//��ʶ���
				{
						//�����滻�ڵ���
						TExpressionTree Expr;
						Expr.Read(NumsVector[i], false);
						std::vector<TCHAR *> newVariableTable;
						TCHAR *temp = NULL;
						for (int n = 0; n < Expr.VariableTable.size(); n++)
						{
							if (temp = FindVariableTable(Expr.VariableTable[n]))
							{
								newVariableTable.push_back(temp);
							}
							else
							{
								eError = ERROR_UNKNOWN_VARIABLE;//�滻���г���ԭ��������û�еı���������
								//Expr.Release();//��תʱ�����������Բ����˹�����
								goto FinishSubs;
							}
						}
					//�õ����б��滻������λ��
					std::vector<TNode *> VarsPos;
					GetVariablePos(var,VarsPos);
					for (int j = 0; j < VarsPos.size(); j++)
					{
						TNode *newNode = CopyNodeTree(Expr.head);
						ReplaceNodeVariable(newNode, newVariableTable);

						//���ӵ��½ڵ�
						if (VarsPos[j] != head)
						{
							if (VarsPos[j]->parent->left != NULL && VarsPos[j]->parent->left == VarsPos[j])
								VarsPos[j]->parent->left = newNode;
							if (VarsPos[j]->parent->right != NULL && VarsPos[j]->parent->right == VarsPos[j])
								VarsPos[j]->parent->right = newNode;
							newNode->parent = VarsPos[j]->parent;
						}
						else
							head = newNode;

						//ɾ���ɽڵ�
						delete VarsPos[j];
					}
				}//δ�ڱ������еı�����������
			}
		}
		else
			eError = ERROR_SUBS_NOT_EQUAL;

		FinishSubs:
		delete[] nVars;
		delete[] nNums;
		for (int i = 0; i < VarsVector.size(); i++)
			delete[] VarsVector[i];
		for (int i = 0; i < NumsVector.size(); i++)
			delete[] NumsVector[i];

		if (eError==ERROR_NO)
			return OutputStr();
	}
		return GetErrorInfo(eError);
}

TCHAR * TExpressionTree::Read(TCHAR *expression, bool bOutput)
{
	std::queue<TNode *> InOrder;
	std::vector<TNode *> PostOrder;
	TCHAR *expr = new TCHAR[_tcslen(expression) + 1];
	_tcscpy(expr, expression);
	expr[_tcslen(expression)] = TEXT('\0');

	if ((eError = ReadToInOrder(expr, InOrder)) == ERROR_NO)
		if ((eError = InQueue2PostQueue(InOrder, PostOrder)) == ERROR_NO)
			if ((eError = BuildExpressionTree(PostOrder)) == ERROR_NO)
			{
				delete[] expr;
				if (bOutput)
					return OutputStr();
				else
					return OutputEmptyStr();
			}

	delete[] expr;
	return GetErrorInfo(eError);
}

void Split(TCHAR *src, std::vector<TCHAR *> &result, TCHAR *sub)
{
	int srclen = _tcslen(src);
	int sublen = _tcslen(sub);
	TCHAR *end = src + srclen;

	//�õ�����λ��
	TCHAR *tempstr = NULL;
	TCHAR *prev = src;
	TCHAR *now = NULL;
	while ((now = _tcsstr(prev, sub)) != NULL)
	{
		tempstr = new TCHAR[now - prev+1];
		_tcsncpy(tempstr, prev, now - prev);
		tempstr[now - prev] = TEXT('\0');
		result.push_back(tempstr);
		prev = now + sublen;
	}
	tempstr = new TCHAR[end - prev + 1];
	_tcsncpy(tempstr, prev, end - prev);
	tempstr[end - prev] = TEXT('\0');
	result.push_back(tempstr);
}

int Replace(TCHAR *src, TCHAR *sub, TCHAR *dest)
{
	int srclen = _tcslen(src);
	if (srclen == 0) return 0;
	int sublen = _tcslen(sub);
	if (sublen == 0) return 0;
	int destlen = _tcslen(dest);
	std::vector<TCHAR *> pos;//����Ѱ�ҵ���λ��

	//�õ�����λ��
	TCHAR *temp = src;
	TCHAR *now = NULL;
	while ((now = _tcsstr(temp, sub)) != NULL)
	{
		pos.push_back(now);
		temp = now + sublen;
	}

	if (pos.size() == 0) return 0;
	int newsize = srclen + pos.size()*(destlen - sublen) + 1;//�´��Ĵ�С������ĩβ��\0
	TCHAR *newsrc = (TCHAR *)malloc(newsize*sizeof(TCHAR));
	newsrc[0] = TEXT('\0');
	TCHAR *prevpos = src;
	for (UINT i = 0; i < pos.size(); i++)
	{
		_tcsncat(newsrc, prevpos, pos[i] - prevpos);
		_tcsncat(newsrc, dest, destlen);
		prevpos = pos[i] + sublen;
	}
	_tcsncat(newsrc, prevpos, srclen - (prevpos - src));
	//�˴�newsrc��ȫ������

	_tcscpy(src, newsrc);
	free(newsrc);
	return pos.size();
}