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
	if (ErrorInfo != NULL)
		delete[] ErrorInfo;

	if (szPostOrder != NULL)
		delete[] szPostOrder;

	if (szInOrder != NULL)
		delete[] szInOrder;

	for (UINT i = 0; i < VariableTable.size(); i++)
	{
		delete[] VariableTable[i];
	}
}

TExpressionTree::enumError TExpressionTree::BuildExpressionTree()
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
		//case MATH_SQRT:
		//case MATH_SIN:
		//case MATH_COS:
		//case MATH_TAN:
		//case MATH_ARCSIN:
		//case MATH_ARCCOS:
		//case MATH_ARCTAN:
		//case MATH_LEFT_PARENTHESIS:
		//case MATH_RIGHT_PARENTHESIS:
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
TExpressionTree::enumError TExpressionTree::InQueue2PostQueue()//����0:���� 1:���Ų����
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
					if (temp.top()->eType == NODE_FUNCTION)
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
	if (GetOperateNum(now->eOperator) == 1)//һԪ�����
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

	if (now->left != NULL)//�����
	{
		TraverseInOrder(now->left, output, buffer);
	}

	if (GetOperateNum(now->eOperator) != 1)//��һԪ��������������һԪ����������˳���Ѹı�
	{
		if (now->eType == NODE_OPERATOR)//����Ϊ�����
			if (
				(now->left != NULL && now->left->eType == NODE_OPERATOR && Rank(now->eOperator) > Rank(now->left->eOperator))
				||
				(now->right != NULL && now->right->eType == NODE_OPERATOR && Rank(now->eOperator) > Rank(now->right->eOperator))
				)//����������������������������ȼ����ڱ���
			{
				_tcscat(output, TEXT("("));
				has_parenthesis = 1;
			}
		Node2Str(*now, buffer);
		_tcscat(output, buffer);
	}

	if (now->right != NULL)//�ұ���
	{
		TraverseInOrder(now->right, output, buffer);
	}

	//�ص�����ʱ����������
	if (has_parenthesis)
	{
		_tcscat(output, TEXT(")"));
	}
}

TCHAR * TExpressionTree::OutputStr()
{
	if (eError == ERROR_NO)
	{
		TCHAR buffer[MAX_VAR_NAME];
		if (szInOrder != NULL)
			delete[] szInOrder;
		szInOrder = new TCHAR[MAX_VAR_NAME * PostOrder.size()];
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
		szPostOrder = new TCHAR[MAX_VAR_NAME * PostOrder.size()];
		szPostOrder[0] = TEXT('\0');
		for (int i = 0; i < PostOrder.size(); i++)
		{
			Node2Str(*PostOrder[i], buffer);
			_tcscat(szPostOrder, buffer);
		}
		return szPostOrder;
	}
	else
		return GetErrorInfo(eError);
}

TExpressionTree::enumError TExpressionTree::ReadToInOrder(TCHAR *expression)
{
	if (_tcslen(expression) == 0)
		return ERROR_EMPTY_INPUT;
	Replace(expression, TEXT(" "), TEXT(""));
	Replace(expression, TEXT("\t"), TEXT(""));
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
		_tcscpy(ErrorInfo, TEXT("���ַǷ��ַ���"));
		break;
	case ERROR_PARENTHESIS_NOT_MATCH:
		_tcscpy(ErrorInfo, TEXT("���Ų�ƥ�䡣"));
		break;
	case ERROR_INVALID_VARNAME:
		_tcscpy(ErrorInfo, TEXT("����ȷ�ı��������������»���\"_\"��Ӣ����ĸ��ͷ����"));
		break;
	case ERROR_EMPTY_INPUT:
		_tcscpy(ErrorInfo, TEXT("���ʽΪ�ա�"));
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

void TExpressionTree::Simplify(TNode *now)
{
	if (now->left != NULL)
		Simplify(now->left);


	if (now->right != NULL)
		Simplify(now->right);

	if (GetOperateNum(now->eOperator) == 2)
	{
		if (now->left->eType == NODE_NUMBER && now->right->eType == NODE_NUMBER)
		{
			CalcNode(now, now->left, now->right);
			delete now->left;
			delete now->right;
			now->eOperator = MATH_NOT_AVAILIALBE;
			now->left = NULL;
			now->right = NULL;
		}
	}
}

TCHAR * TExpressionTree::Simplify()
{
	if (eError == ERROR_NO)
	{
		TNode *temp = head;
		Simplify(head);

		return OutputStr();
	}
	else
		return GetErrorInfo(eError);
}

TCHAR * TExpressionTree::Read(TCHAR *expression)
{
	if ((eError = ReadToInOrder(expression)) == ERROR_NO)
		if ((eError = InQueue2PostQueue()) == ERROR_NO)
			if ((eError = BuildExpressionTree()) == ERROR_NO)
				return OutputStr();

	return GetErrorInfo(eError);
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
	newsrc[0] = TEXT('');
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