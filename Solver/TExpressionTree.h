#pragma once

#include "..\Common\tchar_head.h"
#include <queue>
#include <stack>
#include "..\Solver\TVariableTable.h"
#include "..\enumError.h"

class TExpressionTree
{
private:
#define MAX_VAR_NAME 32//ͬʱҲ�Ǹ�����ת�ַ�������󳤶�
#define MIN_DOUBLE  1e-6
	enumError eError;
	enum enumMathOperator{
		MATH_NOT_AVAILIALBE,
		//һԪ
		MATH_POSITIVE, MATH_NEGATIVE,

		//����
		MATH_SIN, MATH_COS, MATH_TAN,
		MATH_ARCSIN, MATH_ARCCOS, MATH_ARCTAN, MATH_SQRT,
		MATH_LN, MATH_LOG10, MATH_EXP,

		//��Ԫ
		MATH_ADD, MATH_SUBSTRACT,
		MATH_MULTIPLY, MATH_DIVIDE,
		MATH_POWER, MATH_AND, MATH_OR, MATH_MOD,

		MATH_LEFT_PARENTHESIS, MATH_RIGHT_PARENTHESIS
	};

	enum enumNodeType{ NODE_NUMBER, NODE_OPERATOR, NODE_VARIABLE, NODE_FUNCTION };

	/* ����Ԫ�� */
	union TValueOrName{
		double value;//���ֵ�ֵ
		TCHAR *varname;//������
	};
	struct TNode
	{
		enumNodeType eType;
		enumMathOperator eOperator;
		TValueOrName ValueOrName;
		TNode *parent;
		TNode *left, *right;
	};

	void TExpressionTree::Release();

	//��������ʺ���
	int TExpressionTree::GetOperateNum(TExpressionTree::enumMathOperator eOperator);
	TCHAR TExpressionTree::EnumOperatorToTChar(TExpressionTree::enumMathOperator eOperator);
	bool TExpressionTree::isLeft2Right(enumMathOperator eOperator);
	bool TExpressionTree::inAssociativeLaws(enumMathOperator eOperator);
	int TExpressionTree::Rank(enumMathOperator eOperator);

	TCHAR *ErrorInfo;
	bool TExpressionTree::isBaseOperator(TCHAR c);
	bool TExpressionTree::isDoubleChar(TCHAR c);
	bool TExpressionTree::isLegal(TCHAR c);
	TExpressionTree::enumMathOperator TExpressionTree::BaseOperatorCharToEnum(TCHAR c);

	TExpressionTree::enumMathOperator TExpressionTree::Str2Function(TCHAR *start, TCHAR *end);
	void TExpressionTree::Function2Str(TExpressionTree::enumMathOperator eOperator, TCHAR *buffer);

	TCHAR *szOutput;
	TVariableTable *pVariableTable;
	int iVarAppearedCount;
	TNode *LastVarNode;
	//TVariableTable SelfVariableTable;
	bool TExpressionTree::IsIntAndEven(double n);
	TExpressionTree::enumError TExpressionTree::InQueue2PostQueue(std::queue<TNode *> &InOrder, std::vector<TNode *> &PostOrder);
	TExpressionTree::enumError TExpressionTree::ReadToInOrder(TCHAR *expression, std::queue<TNode *> &InOrder);
	void TExpressionTree::Node2Str(const TNode &node, TCHAR *result);
	TExpressionTree::enumError TExpressionTree::BuildExpressionTree(std::vector<TNode *> &PostOrder);
	void TExpressionTree::TraverseInOrder(TNode *now, TCHAR *output, TCHAR *buffer);
	void TExpressionTree::CalcNode(TNode *Operator, const TNode *Node1, const TNode *Node2);//����ڵ�ֵ��֧������������ͺ�����������Operator
	TExpressionTree::enumError TExpressionTree::Simplify(TNode *now);
	void TExpressionTree::GetNodeNum(TNode *now, int &n);
	int TExpressionTree::GetNodeNum(TNode *head);
	void TExpressionTree::DeleteNode(TNode *node);
	void TExpressionTree::DeleteNodeTraversal(TNode *node);
	TCHAR * TExpressionTree::OutputEmptyStr();
	void TExpressionTree::Diff(TNode *now, TCHAR *var);
	TExpressionTree::TNode * TExpressionTree::CopyNodeTree(TNode *oldNode);
	TCHAR * TExpressionTree::FindVariableTableFrom(TCHAR *varstr, std::vector<TCHAR *> newVariableTable);//���ұ����Ƿ��ڱ������У�û���򷵻�NULL
	void TExpressionTree::GetVariablePos(TNode *now, const TCHAR *var, std::vector<TNode *> &VarsPos);
	void TExpressionTree::GetVariablePos(const TCHAR *var, std::vector<TNode *> &VarsPos);
	void TExpressionTree::CopyVariableTable(std::vector<TCHAR *> &Dest, const std::vector<TCHAR *> source);
	void TExpressionTree::ReplaceNodeVariable(TNode *now, std::vector<TCHAR *> &newVariableTable);
	bool TExpressionTree::CanCalc(TNode *now);
	void TExpressionTree::Calc(TNode *now);
	void TExpressionTree::LinkParent(TNode *child, TNode *ignore);
	TExpressionTree::TNode * TExpressionTree::NewNode(enumNodeType eType, enumMathOperator eOperator = MATH_NOT_AVAILIALBE);
	void TExpressionTree::ReleaseVectorTNode(std::vector<TNode *> vec);
	void TExpressionTree::Vpa_inner(TNode *now);
	void TExpressionTree::Solve(TNode *now, TNode *&write_pos);
	void TExpressionTree::CheckOnlyOneVar(TNode *now);
	void TExpressionTree::Subs_inner(TNode *node, TCHAR *ptVar, double value);
public:
	TNode *head;
	enumError TExpressionTree::GetError();
	TCHAR * TExpressionTree::GetErrorInfo();
	void TExpressionTree::Reset();
	TCHAR * TExpressionTree::Vpa(bool bOutput);
	TCHAR * TExpressionTree::LinkVariableTable(TVariableTable *p);//���ӱ�����
	TCHAR * TExpressionTree::Read(const TCHAR *expression, bool bOutput);
	TCHAR * TExpressionTree::Read(double num, bool bOutput);//����ֻ��1�����ֵı��ʽ

	//���в�����δ���
	TCHAR * TExpressionTree::Solve(TCHAR *&var, double &value);//��ⵥ�������� ����֤������ԣ�����ǰ����HasOnlyOneVarȷ�� ���Ķ����ʽ����
	TCHAR * TExpressionTree::OutputStr(bool bIgnoreError = false);
	TCHAR * TExpressionTree::Simplify(bool bOutput);//����
	TCHAR * TExpressionTree::Diff(TCHAR *var, int n, bool bOutput);//�Ա�����
	TCHAR * TExpressionTree::Subs(TCHAR *ptVar, double value, bool output);
	TCHAR * TExpressionTree::Subs(const TCHAR *vars,const TCHAR *nums, bool output);//varsΪ���滻������numsΪ�滻���ʽ���Կո�ָ�
	TCHAR * TExpressionTree::Subs(std::vector<TCHAR *> VarsVector, std::vector<double> NumsVector, bool output);
	bool TExpressionTree::CanCalc();//����Ƿ��б������ڣ����Լ����򷵻�true
	bool TExpressionTree::IsSingleVar();//����Ƿ�ΪһԪ(not used)
	bool TExpressionTree::HasOnlyOneVar();//ֻ��һ��������ֻ�и�read����Ч��
	bool TExpressionTree::CheckOnlyOneVar();//ֻ��һ��������ʵʱ��֤��
	double TExpressionTree::Value(bool operateHeadNode);//����֤�ɼ����ԣ�������CanCalc����
	TCHAR * TExpressionTree::Calc(double *result = NULL);//������ʽ��ֵ����������result��ѽ�����롣����ֵΪ����ַ�������ʽ����

	TExpressionTree& TExpressionTree::operator=(const TExpressionTree &expr);
	TExpressionTree& TExpressionTree::operator+(const TExpressionTree &expr);
	TExpressionTree & TExpressionTree::operator*(double value);
	TExpressionTree & TExpressionTree::operator+(double value);
	TExpressionTree & TExpressionTree::operator-(double value);

	TExpressionTree();
	~TExpressionTree();
};

void GetErrorInfo(enumError err, TCHAR result[]);