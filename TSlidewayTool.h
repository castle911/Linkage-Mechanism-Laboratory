#pragma once
#include "TLineTool.h"

class TSlidewayTool :public TLineTool
{
private:
	void TSlidewayTool::AddCoincide(TConstraintCoincide *pCoincide, int id, TConfiguration *pConfig);//���м���Լ�������˴�
public:
	TSlidewayTool();
	~TSlidewayTool();
	TElement * TSlidewayTool::AddIntoShape(TRealLine &RealLine);
};

