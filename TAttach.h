#pragma once
#include <vector>
#include <Windows.h>
#include "DPOINT.h"
#include "TShape.h"
#include "TConfiguration.h"
#include "TDraw.h"

class TCanvas;
class TAttach
{
private:
	//bool bAttachPoint;
	TCanvas *pCanvas;
	TShape *pShape;
	TConfiguration *pConfig;
	int iIvoryLine;
	TRealLine *XAssistLine, *YAssistLine;//X,Y�Ḩ����
	bool bShowXAssist, bShowYAssist;//��ʾX,Y�Ḩ����

	TRealLine *ExtensionLine;


	int iAttachPixel = 10;
	bool TAttach::DPTisApproached(DPOINT dpt1, DPOINT dpt2);
	bool TAttach::AttachLine(POINT ptNowPos);
	bool TAttach::AttachPointSelf(DPOINT dptPos);
	bool TAttach::AttachLine_Element(POINT ptNowPos, const std::vector<DPOINT> vecdptAbsolute);
	bool TAttach::AttachLineSelf(POINT ptNowPos);
public:
	std::vector<DPOINT> vecdpt;//�ݴ�㣬����PolylineBar֮ǰ�����ĵ㣬Ϊ��������

	int iAttachLinePointIndex[2];//�����߶�ʱ���������ߵ�P Q����vecDpt�е����
	bool bShowExtensionLine;//����ʾ�ӳ��ߣ�����ExtensionLine����Ϊ�ж��Ƿ�����ֱ�ߵ�����

	bool bAttachedEndpoint;//�����˵㣬Ϊtrue����������ȷ�д��ڵĵ�
	bool bAttachedEndpointSelf;//��������vecdpt��Ķ˵㣬����PolylineBar����
	EnumElementType eAttachElementType;

	bool bShowAttachPoint;//�ử��
	TElement *pAttachElement;
	int iAttachElementPointIndex;
	DPOINT dptAttach;

	TAttach(TCanvas *pCanvas, TShape *shape, TConfiguration *config);
	~TAttach();
	void TAttach::InitialLine(POINT ptPos);
	void TAttach::AttachAll(POINT ptNowPos, DPOINT dptCheckPos);
	void TAttach::AttachAll(POINT ptNowPos);
	bool TAttach::AttachAxis(DPOINT dptNowPos, DPOINT dptCheckPos);
	bool TAttach::AttachPoint(DPOINT dptPos);
	void TAttach::Draw(HDC hdc);
	int TAttach::GetiIvoryLine()
	{
		return iIvoryLine;
	}
};

