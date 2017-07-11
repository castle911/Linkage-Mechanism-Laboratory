#pragma once
#include <vector>
#include <Windows.h>

#include "TWindow.h"
#include "DPOINT.h"
#include "TLine.h"
class TConfiguration;
class TGraph :
	public TWindow
{
private:
	double x_max=21.3703, x_min=21.202, y_max=0.0, y_min=0.0;
	struct TPointData
	{
		bool Show;
		std::vector<DPOINT> dptVector;//��ʵ����
		String sLegend;
		LONG lLegendWidth, lLegendHeight;//��Ӧͼ���Ŀ��
		int iPtCount;//������
		POINT *ptArray;//�����
		double x_max, y_max, x_min, y_min;
		double x_len, y_len;
		LOGPEN logpen;
	};
	std::vector<TPointData> vecPointData;

	TConfiguration *pConfig;
	TLine LineMouseX, LineMouseY;
	POINT ptMouse;
	DPOINT dptMouse;
	RECT rcGraph;//�ڴ����ڵ���ʾ��Χ
	int iPickPointDataIndex;
	int iPick;

	int iMarginTop,iMarginBottom,iMarginLeft,iMarginRight;
	LONG lTextHeight;
	LONG lInterval=2;//�̶���̶����ּ��
	LONG lScaleBottomHeight;//�̶�+���+�̶����� �ܸ߶�
	LONG lScaleLeftWidth;//�̶�+���+�̶����� �ܿ��

	struct TGridLineAndScale
	{
		POINT ptGridStart;
		POINT ptScaleBegin;
		POINT ptScaleEnd;
		RECT rcScaleText;
		String sScale;
	};
	std::vector<TGridLineAndScale> vecGridLineAndScale;//�������߼��̶��߼���

	struct TGridScaleSmall
	{
		POINT ptBegin;
		POINT ptEnd;
	};
	std::vector<TGridScaleSmall> vecGridScaleSmall;//С�����߼���

	HMENU hMenuData;

	void TGraph::OnSize(WPARAM wParam, LPARAM lParam);
	void TGraph::OnDraw(HDC hdc);
	bool TGraph::OnClose();
	void TGraph::OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void TGraph::OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void TGraph::OnCommand(WPARAM wParam, LPARAM lParam);
	void TGraph::CalcPointArray(const RECT &rcGraph);
	void TGraph::DrawLegend(HDC hdc, const LOGPEN &logpenBorder);
	void TGraph::CalcGridAndScale(HDC hdc, LONG &lBottomSize, LONG &lLeftSize,LONG &lRightSize, LONG lScaleLong, LONG lInterval);
	void TGraph::DrawGridAndScale(HDC hdc);
	bool TGraph::SaveToCSV(TCHAR szFileName[]);
public:
	bool bDraw = true;//���Ϊfalse����OnDraw�¼��в�����
	bool bRealClose = true;//���Ϊtrue����ر�ʱ�رմ��ڡ�����ֻ���ش���
	bool bShowMouseLine = false;
	bool bAdaptiveMargin = true;
	bool bShowTitle = true;
	bool bShowLabelX = true;
	bool bShowLabelY = true;
	bool bShowLegend = true;//��ʾͼ��
	bool bShowGridBig = true;
	bool bShowGridSmall = true;

	//x,y���ǩ
	String sLabelX;
	String sLabelY;

	TGraph(TConfiguration *pConfig);
	~TGraph();
	void TGraph::InputDptVector(const std::vector<DPOINT> &dptInputVector, const LOGPEN &logpen, bool visible, const TCHAR szLegend[] = TEXT(""));
	void TGraph::InputDptVector(const std::vector<double> &vecX, const std::vector<double> &vecY, const LOGPEN &logpen, bool visible, const TCHAR szLegend[] = TEXT(""));
	void TGraph::Clear();
	void TGraph::Refresh();//ˢ�� ��ͬ�ڵ���OnSize
	void TGraph::AttachPoint();
	void TGraph::SetMargin(int iMargin);
	void TGraph::SetPointDataVisible(int index,bool visible);
};

