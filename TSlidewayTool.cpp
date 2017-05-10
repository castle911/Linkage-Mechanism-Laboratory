#pragma once
#include "TSlidewayTool.h"

#include "TShape.h"
#include "TSlideway.h"

#include "TConfiguration.h"

TSlidewayTool::TSlidewayTool()
{
	myElementType = ELEMENT_SLIDEWAY;

	delete MoveLine;
	MoveLine = new TSlideway;
	MoveLine->SetStyle(pConfig);

	sType = TEXT("����");
	bCanBuildCoincide = false;
}


TSlidewayTool::~TSlidewayTool()
{
}


TElement * TSlidewayTool::AddIntoShape(TRealLine &RealLine)
{
	RealLine.eType = myElementType;
	TSlideway Slideway;
	Slideway= RealLine;
	return pShape->AddElement(&Slideway);
}

//void TSlidewayTool::AddCoincide(TConstraintCoincide *pCoincide, int id, TConfiguration *pConfig)//���м���Լ�������˴�
//{
//
//}