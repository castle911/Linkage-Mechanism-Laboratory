#pragma once

#include <tchar.h>
#include <queue>
#include <stack>

class TExpressionTree
{
private:
#define MAX_VAR_NAME 32//ͬʱҲ�Ǹ�����ת�ַ�������󳤶�
#define MIN_DOUBLE  1e-6
	enum enumError{
		ERROR_NO, 
		ERROR_ILLEGALCHAR,//���ַǷ��ַ�
		ERROR_PARENTHESIS_NOT_MATCH,//���Ų�ƥ��
		ERROR_INVALID_VARNAME,//����ȷ�ı�����
		ERROR_EMPTY_INPUT,
		ERROR_DIVIDE_ZERO,
		ERROR_UNKNOWN_VARIABLE
	} eError;
	enum enumMathOperator{
		MATH_NOT_AVAILIALBE,
		MATH_POSITIVE, MATH_NEGATIVE,
		MATH_SIN, MATH_COS, MATH_TAN,
		MATH_ARCSIN, MATH_ARCCOS, MATH_ARCTAN,MATH_SQRT,
		MATH_POWER,MATH_AND,MATH_OR,MATH_MOD,
		MATH_ADD, MATH_SUBSTRACT, 
		MATH_MULTIPLY, MATH_DIVIDE,
		MATH_LEFT_PARENTHESIS, MATH_RIGHT_PARENTHESIS
	};

	enum enumNodeType{NODE_NUMBER,NODE_OPERATOR,NODE_VARIABLE,NODE_FUNCTION};

	/* ����Ԫ�� */
	struct TNode
	{
		enumNodeType eType;
		enumMathOperator eOperator;
		union {
			double value;//���ֵ�ֵ
			TCHAR *varname;//������
		}ValueOrName;
		//int op_num;//���������� eg. һԪ����Ԫ�����
		TNode *parent;
		TNode *left, *right;
	};

	void TExpressionTree::Release();

	TCHAR *ErrorInfo;
	TCHAR * TExpressionTree::GetErrorInfo(TExpressionTree::enumError eError);

	bool TExpressionTree::isAlphaCharOrUnderline(TCHAR c);
	bool TExpressionTree::isBaseOperator(TCHAR c);
	bool TExpressionTree::isDoubleChar(TCHAR c);
	bool TExpressionTree::isLegal(TCHAR c);
	int TExpressionTree::GetOperateNum(TExpressionTree::enumMathOperator eOperator);
	TExpressionTree::enumMathOperator TExpressionTree::BaseOperatorCharToEnum(TCHAR c);
	TCHAR TExpressionTree::EnumOperatorToTChar(TExpressionTree::enumMathOperator eOperator);
	bool TExpressionTree::isLeft2Right(enumMathOperator eOperator);
	TExpressionTree::enumMathOperator TExpressionTree::Str2Function(TCHAR *start, TCHAR *end);
	void TExpressionTree::Function2Str(TExpressionTree::enumMathOperator eOperator, TCHAR *buffer);

	TCHAR *szPostOrder,*szInOrder;
	std::vector<TCHAR *> VariableTable;
	TNode *head;
	int TExpressionTree::Rank(enumMathOperator eOperator);
	TExpressionTree::enumError TExpressionTree::InQueue2PostQueue(std::queue<TNode *> &InOrder, std::vector<TNode *> &PostOrder);
	TExpressionTree::enumError TExpressionTree::ReadToInOrder(TCHAR *expression, std::queue<TNode *> &InOrder);
	void TExpressionTree::Node2Str(TNode &node, TCHAR *result);
	TExpressionTree::enumError TExpressionTree::BuildExpressionTree(std::vector<TNode *> &PostOrder);
	void TExpressionTree::TraverseInOrder(TNode *now, TCHAR *output, TCHAR *buffer);
	void TExpressionTree::CalcNode(TNode *Operator, TNode *Node1, TNode *Node2);
	TExpressionTree::enumError TExpressionTree::Simplify(TNode *now);
	void TExpressionTree::GetNodeNum(TNode *now, int &n);
	int TExpressionTree::GetNodeNum(TNode *head);
	void TExpressionTree::DeleteNode(TNode *node, std::vector<TNode *> DeleteList);
	void TExpressionTree::DeleteNode(TNode *node);
	TCHAR * TExpressionTree::OutputEmptyStr();
	void TExpressionTree::Diff(TNode *now, TCHAR *var);
	TExpressionTree::TNode * TExpressionTree::CopyNodeTree(TNode *oldNode);
public:
	TCHAR * TExpressionTree::Read(TCHAR *expression, bool bOutput);
	TCHAR * TExpressionTree::OutputStr();//������ʽ
	TCHAR * TExpressionTree::OutputPostOrderStr();//���PostOrder���ʽ
	TCHAR * TExpressionTree::Simplify(bool bOutput);//����
	TCHAR * TExpressionTree::Diff(TCHAR *var, int n, bool bOutput);//�Ա�����
	//void TExpressionTree::Output(TCHAR *buffer){};

	TExpressionTree();
	~TExpressionTree();
};

//�滻ȫ�� srcԴ�ַ�����sub���滻���ݣ�dest�滻������
//�����滻��������
//src��С�����б�֤�������ڲ���ָ֤��Խ��
int Replace(TCHAR *src, TCHAR *sub, TCHAR *dest);