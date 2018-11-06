#pragma once
#include <vector>
#include <Windows.h>

#include "TWindow.h"
#include "..\Common\DPOINT.h"
#include "..\Element\TLine.h"

	using std::vector;

class TConfiguration;
class TGraph :
	public TWindow
{
private:
	//double x_max=21.3703, x_min=21.202, y_max=0.0, y_min=0.0;

	//�������߼��̶���������
	struct TGridLineAndScale
	{
		POINT ptGridStart;//�����������
		POINT ptGridEnd;
		POINT ptScaleBegin;//�̶����
		POINT ptScaleEnd;//�̶��յ�
		RECT rcScaleText;//�̶����ְ�Χ��
		String sScale;
	};

	//С��������������
	struct TGridScaleSmall
	{
		POINT ptBegin;//С���������
		POINT ptEnd;//С�������յ�
	};

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
		std::vector<TGridLineAndScale> vecGridLineAndScale;//�������߼��̶��߼���
		std::vector<TGridScaleSmall> vecGridScaleSmall;//С�����߼���
		LONG lScaleLeftWidth;//�̶�+���+�̶����� �ܿ��
		String sLabelY;
		bool bShowGridBig = false;
		bool bShowGridSmall = false;
		RECT rcLabelY;

		POINT ptAxisBegin, ptAxisEnd;
	};
		LONG lScaleBottomHeight;//�̶�+���+�̶����� �ܸ߶�

	std::vector<TPointData> vecPointData;

	RECT rcGraph;//�ڴ����ڵ���ʾ��Χ

	TConfiguration *pConfig;
	TLine LineMouseX, LineMouseY;
	POINT ptMouse;
	DPOINT dptMouse;
	int iPickPointDataIndex;
	int iPick;

	int iMarginTop,iMarginBottom,iMarginLeft,iMarginRight;
	LONG lTextHeight;
	LONG lInterval=2;//�̶���̶����ּ��


	HMENU hMenuData;

	void TGraph::OnSize(WPARAM wParam, LPARAM lParam);
	void TGraph::OnDraw(HDC hdc);
	bool TGraph::OnClose();
	void TGraph::OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void TGraph::OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void TGraph::OnCommand(WPARAM wParam, LPARAM lParam);
	void TGraph::CalcPointArray(const RECT &rcGraph);
	void TGraph::DrawLegend(HDC hdc, const LOGPEN &logpenBorder); 
	void TGraph::DrawGridAndScale(HDC hdc);
	bool TGraph::SaveToCSV(TCHAR szFileName[]);

	void TGraph::CalcGridAndScale(HDC hdc, TPointData &PointData, LONG &lBottomSize, LONG &lRightSize,
		const RECT &rcGraph, LONG &lPrevAllWidth, LONG lScaleLong, LONG lInterval);
	void TGraph::AttachPoint();
public:
	bool bDraw = true;//���Ϊfalse����OnDraw�¼��в�����
	bool bRealClose = true;//���Ϊtrue����ر�ʱ�رմ��ڡ�����ֻ���ش���
	bool bShowMouseLine = false;
	bool bAdaptiveMargin = true;
	bool bShowTitle = true;
	bool bShowLabelX = true;
	bool bShowLegend = true;//��ʾͼ��

	//x,y���ǩ
	String sLabelX;

	TGraph(TConfiguration *pConfig);
	~TGraph();
	void TGraph::InputDptVector(const std::vector<double> &vecX, const std::vector<double> &vecY, const LOGPEN &logpen, bool visible, const TCHAR szLegend[] = TEXT(""), const TCHAR szUnitLabel[] = TEXT(""));
	void TGraph::InputDptVector(const std::vector<DPOINT> &dptInputVector, const LOGPEN &logpen, bool visible, const TCHAR szLegend[] = TEXT(""),const TCHAR szUnitLabel[]=TEXT(""));
	void TGraph::Clear();
	void TGraph::Refresh();//ˢ�� ��ͬ�ڵ���OnSize
	void TGraph::SetMargin(int iMargin);
	void TGraph::SetPointDataVisible(int index,bool visible);
};