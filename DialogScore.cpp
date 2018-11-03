#pragma once

#if (defined _STUDENT) || (defined _TEACHER)

#include "DialogScore.h"

#include <Windows.h>
#include "String.h"

#include "resource.h"
#include "TEdit.h"
#include "ShowMessage.h"

#include "global.h"
namespace DialogScore
{
	TEdit editScoreClass;
	TEdit editScoreName;
	TEdit editScoreNumber;
	TEdit editScore;
	BOOL CALLBACK DlgScoreProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_INITDIALOG:
		{
			editScoreClass.LinkControl(hDlg, IDC_EDIT_SCORE_CLASS);
			editScoreName.LinkControl(hDlg, IDC_EDIT_SCORE_NAME);
			editScoreNumber.LinkControl(hDlg, IDC_EDIT_SCORE_NUMBER);
			editScore.LinkControl(hDlg, IDC_EDIT_SCORE);

			editScoreClass.SetText(sStudentClass);
			editScoreName.SetText(sStudentName);
			editScoreNumber.SetText(sStudentNumber);
			editScore.SetText(sStudentScore);
			return TRUE;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDOK:
				//sStudentClass = editScoreClass.GetText();
				//if (sStudentClass.length() == 0)
				//{
				//	ShowMessage(TEXT("�༶����Ϊ�գ�"));
				//	return TRUE;
				//}
				//sStudentName = editScoreName.GetText();
				//if (sStudentName.length() == 0)
				//{
				//	ShowMessage(TEXT("��������Ϊ�գ�"));
				//	return TRUE;
				//}
				//sStudentNumber = editScoreNumber.GetText();
				//if (sStudentNumber.length() == 0)
				//{
				//	ShowMessage(TEXT("ѧ�Ų���Ϊ�գ�"));
				//	return TRUE;
				//}

				sStudentScore = editScore.GetText();
				if (sStudentScore.length() != 0)
				{
					ShowMessage(TEXT("¼��ɹ���"));
				}

				EndDialog(hDlg, 0);
				return TRUE;
			case IDCANCEL:
				EndDialog(hDlg, 0);
				return TRUE;
			}
			return TRUE;
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return TRUE;
		}
		return FALSE;
	}
}

#endif