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
	bool TAttach::AttachLine_inner(DPOINT dptNowPos);
	bool TAttach::AttachPointSelf(DPOINT dptPos);

	bool TAttach::AttachLineSelf(DPOINT dptNowPos);//��vecdpt����
	bool TAttach::AttachAxis(DPOINT dptNowPos, DPOINT dptCheckPos);
	bool TAttach::AttachPoint(DPOINT dptPos);
public:
	std::vector<DPOINT> vecdpt;//�ݴ�㣬����PolylineBar֮ǰ�����ĵ㣬Ϊ��������

	int iAttachLinePointIndex[2];//�����߶�ʱ���������ߵ�P Q����vecDpt�е����
	bool bShowExtensionLine;//����ʾ�ӳ��ߣ�����ExtensionLine����Ϊ�ж��Ƿ�����ֱ�ߵ�����

	bool bAttachedEndpoint;//�����˵㣬Ϊtrue����������ȷ�д��ڵĵ�
	bool bAttachedEndpointSelf;//��������vecdpt��Ķ˵㣬����PolylineBar����
	//EnumElementType eAttachElementType;

	bool bShowAttachPoint;//�ử��
	TElement *pAttachElement;
	int iAttachElementPointIndex;
	DPOINT dptAttach;

	TAttach(TCanvas *pCanvas, TShape *shape, TConfiguration *config);
	~TAttach();
	void TAttach::InitialLine(POINT ptPos);
	void TAttach::AttachAll(POINT ptNowPos, DPOINT dptCheckPos);
	void TAttach::AttachAll(POINT ptNowPos);
	void TAttach::AttachLine(POINT ptNowPos);
	void TAttach::Draw(HDC hdc);
	int TAttach::GetiIvoryLine()
	{
		return iIvoryLine;
	}

	bool TAttach::AttachPointByElement(DPOINT dptNowPos, DPOINT &dpt, int iPointIndex,const TElement *pElement, const TConfiguration *pConfig);
	bool TAttach::AttachLineByRelativeVecPt(DPOINT dptNowPos, TElement *pElement, const std::vector<DPOINT> &vecdptRelative, const TConfiguration *pConfig);
	bool TAttach::AttachLineByAbsoluteVecPt(DPOINT dptNowPos, TElement *pElement, const std::vector<DPOINT> &vecdptAbsolute);
	bool TAttach::AttachLine_Element_inner(DPOINT dptNowPos, DPOINT dptAbsolute1, DPOINT dptAbsolute2,const TElement *pElement, int PointIndex[2],
		const TConfiguration *pConfig);

	bool TAttach::DPTisApproached(DPOINT dpt1, DPOINT dpt2);
};

