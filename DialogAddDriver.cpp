#pragma once
#include <Windows.h>
#include <windowsx.h>

#include "DialogAddDriver.h"

#include "resource.h"

#include "TTransfer.h"
#include "TExpressionTree.h"
#include "TVariableTable.h"
#include "String.h"

#include "TMainWindow.h"
extern TMainWindow win;

namespace DialogAddDriver
{
	int iElementId;
	TEdit EditA;
	TEdit EditB;
	TEdit LabelA;
	TEdit LabelB;
	TEdit EditExprRight;
	TEdit EditExprLeft;
	TEdit LabelDriverUnit;
	TGraph *pGraph;
	HWND hComboDriverType;
	HWND hComboExprType;

	//�������ؼ����½���˴�
	void UpdateDlgAddDriver()
	{
		//��������ѡ��λ
		switch (ComboBox_GetCurSel(hComboDriverType))
		{
		case 0://λ��
			LabelDriverUnit.SetText(TEXT("deg"));
			break;
		case 1://�ٶ�
			LabelDriverUnit.SetText(TEXT("deg/sec"));
			break;
		case 2://���ٶ�
			LabelDriverUnit.SetText(TEXT("deg/sec^2"));
			break;
		}

		//���ݱ��ʽ���;�����ʾA,B
		switch (ComboBox_GetCurSel(hComboExprType))
		{
		case 0://����
			Edit_SetReadOnly(EditExprRight.m_hWnd, TRUE);
			EditA.SetVisible(true);
			LabelA.SetVisible(true);
			EditB.SetVisible(false);
			LabelB.SetVisible(false);
			switch (ComboBox_GetCurSel(hComboDriverType))
			{
			case 0://λ��
				EditExprRight.SetText(TEXT("%f"),TTransfer::TCHAR2double(EditA.GetText()));
				break;
			case 1://�ٶ�
				EditExprRight.SetText(TEXT("%f*t"), TTransfer::TCHAR2double(EditA.GetText()));
				break;
			case 2://���ٶ�
				EditExprRight.SetText(TEXT("0.5*%f*t^2"), TTransfer::TCHAR2double(EditA.GetText()));
				break;
			}
			break;
		case 1://ֱ��
			Edit_SetReadOnly(EditExprRight.m_hWnd, TRUE);
			EditA.SetVisible(true);
			LabelA.SetVisible(true);
			EditB.SetVisible(true);
			LabelB.SetVisible(true);
			switch (ComboBox_GetCurSel(hComboDriverType))
			{
			case 0://λ��
				EditExprRight.SetText(TEXT("%f*t+%f"), TTransfer::TCHAR2double(EditA.GetText()), TTransfer::TCHAR2double(EditB.GetText()));
				break;
			case 1://�ٶ�
				break;
			case 2://���ٶ�
				break;
			}
			break;
		case 2://�Զ���
			Edit_SetReadOnly(EditExprRight.m_hWnd, FALSE);
			EditA.SetVisible(false);
			LabelA.SetVisible(false);
			EditB.SetVisible(false);
			LabelB.SetVisible(false);
			switch (ComboBox_GetCurSel(hComboDriverType))
			{
			case 0://λ��
				break;
			case 1://�ٶ�
				break;
			case 2://���ٶ�
				break;
			}
			break;

		}

		EditExprLeft.SetText(TEXT("phi%d"), iElementId);
	}

	BOOL CALLBACK DlgAddDriverProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
	{

		switch (message)
		{
		case WM_INITDIALOG:
		{

			//
			pGraph = new TGraph(&(win.m_Configuration));
			pGraph->CreateEx(0, TEXT("ͼ��"), TEXT("ͼ��"),
				WS_CHILD,
				300,
				50,
				300,
				200,
				hDlg, (HMENU)0, (HINSTANCE)GetWindowLong(hDlg, GWL_HINSTANCE));
			pGraph->SetDoubleBuffer(true);
			pGraph->ShowWindow(SW_SHOWNORMAL);
			pGraph->UpdateWindow();

			//std::vector<DPOINT> dptVector;
			//DPOINT dpt;
			//for (double x = 0; x < 1000; ++x)
			//{
			//	dpt.x = x;
			//	dpt.y = sin(x / 50);
			//	dptVector.push_back(dpt);
			//}
			//pGraph->InputDptVector(dptVector);

			//
			hComboDriverType = GetDlgItem(hDlg, IDC_COMBO_DRIVER_TYPE);
			ComboBox_AddString(hComboDriverType, TEXT("λ��"));
			ComboBox_AddString(hComboDriverType, TEXT("�ٶ�"));
			ComboBox_AddString(hComboDriverType, TEXT("���ٶ�"));
			ComboBox_SetCurSel(hComboDriverType, 0);

			//
			hComboExprType = GetDlgItem(hDlg, IDC_COMBO_EXPR_TYPE);
			ComboBox_AddString(hComboExprType, TEXT("����"));
			ComboBox_AddString(hComboExprType, TEXT("ֱ��"));
			ComboBox_AddString(hComboExprType, TEXT("�Զ���"));
			ComboBox_SetCurSel(hComboExprType, 0);

			//
			EditA.LinkControl(GetDlgItem(hDlg, IDC_EDIT_A));
			EditB.LinkControl(GetDlgItem(hDlg, IDC_EDIT_B));

			LabelA.LinkControl(GetDlgItem(hDlg, IDC_STATIC_A));
			LabelB.LinkControl(GetDlgItem(hDlg, IDC_STATIC_B));

			//
			LabelDriverUnit.LinkControl(GetDlgItem(hDlg, IDC_STATIC_DRIVER_UNIT));


			EditExprLeft.LinkControl(GetDlgItem(hDlg, IDC_EDIT_FORMULAR_L));

			////
			EditExprRight.LinkControl(GetDlgItem(hDlg, IDC_EDIT_FORMULAR_R));
			EditExprRight.SetText(TEXT("0.1*t"));



			UpdateDlgAddDriver();

			return TRUE;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{

			//�Ķ�Edit����
			case IDC_EDIT_A:
			case IDC_EDIT_B:
			{
				if (HIWORD(wParam) == EN_CHANGE)
				{
					UpdateDlgAddDriver();
				}
				break;
			}

			//Expr�䶯
			case IDC_EDIT_FORMULAR_R:
			{
				if (HIWORD(wParam) == EN_CHANGE)
				{
					TCHAR buf[32];
					TExpressionTree Expr;
					//TVariableTable VariableTable;
					//VariableTable.Define(NULL, TEXT("t"));
					//Expr.LinkVariableTable(&VariableTable);

					double value;

					String s = EditExprRight.GetText();
					String s_temp;

					std::vector<DPOINT> dptVector;
					//enumError err;
					for (double t = 0; t <= 10; t += 0.1)
					{
						s_temp = s;
						if (s.find(TEXT("t"))!=String::npos)
							s_temp.replace(s.find(TEXT("t")), 1, TTransfer::double2TCHAR(t, buf));

						try
						{
							Expr.Read(s_temp.c_str(), false);

							value = Expr.Value(true);
						}
						catch (enumError err)
						{
							break;
						}
						//if ((err = Expr.GetError()) != ERROR_NO)
						//	break;
						dptVector.push_back({ t, value });
					}
					pGraph->InputDptVector(dptVector);
					
				}
				break;
			}

			//�Ķ�ComboBoxѡ��
			case IDC_COMBO_EXPR_TYPE:
			case IDC_COMBO_DRIVER_TYPE:
			{
				WORD notification_code = HIWORD(wParam);
				if (notification_code == CBN_SELCHANGE)
				{
					UpdateDlgAddDriver();
				}
				break;
			}
			case IDOK:
				//���Լ��

				//win.pSolver->AddMouseConstraint
			case IDCANCEL:
				EndDialog(hDlg, 0);
				return TRUE;
			}
			return TRUE;
		case WM_CLOSE:
			delete pGraph;
			EndDialog(hDlg, 0);
			return TRUE;
		}
		return FALSE;
	}
}