#include "tchar_head.h"

#include "FileFunction.h"

#include <Windows.h>

//����index=1��õ������ļ���
//�ɹ�ȡ�÷���true 
bool GetCommandLineByIndex(int index, TCHAR *assigned)
{
	int iCmdLineCount = -1;
	int len = _tcslen(GetCommandLine());
	TCHAR *origin = new TCHAR[len + 1];
	TCHAR *s = origin;
	_tcscpy(s, GetCommandLine());
	bool inchar = false;
	TCHAR *start, *end;
	while ((s = _tcschr(s, TEXT('\"'))) != NULL)
	{
		s++;
		if (inchar == false)//����
		{
			start = s;
			inchar = true;
		}
		else
		{
			end = s - 1;
			iCmdLineCount++;
			if (iCmdLineCount == index)
			{
				_tcsncpy(assigned, start, end - start);
				assigned[end - start] = TEXT('\0');
				break;
			}
			inchar = false;
		}
	}
	delete[] origin;
	if (iCmdLineCount == index)
		return true;
	else
		return false;
}


bool GetFileExists(TCHAR filename[])
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(filename, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		return false;
	}
	else {
		FindClose(hFind);
		return true;
	}
}

//lpstrFilter��ʽ��TEXT("��������ļ�(*.mds)\0*.mds\0\0")
//ofn������ռ䱣���ļ���ַ���Ķ�szFile��ͬ�ڸĶ�ofn.lpstrFile
void InitialOpenFileName(OPENFILENAME *ofn, HWND hwnd, TCHAR szFile[], TCHAR lpstrFilter[], DWORD nMaxFile)
{
	// Initialize OPENFILENAME
	ZeroMemory(ofn, sizeof(OPENFILENAME));
	ofn->lStructSize = sizeof(OPENFILENAME);
	ofn->hwndOwner = hwnd;
	ofn->lpstrFile = szFile;
	ofn->lpstrFile[0] = TEXT('\0');
	ofn->nMaxFile = nMaxFile;
	ofn->lpstrFilter = lpstrFilter;//����\0��ʾ����
	ofn->nFilterIndex = 1;
	ofn->lpstrFileTitle = NULL;
	ofn->nMaxFileTitle = 0;
	ofn->lpstrInitialDir = NULL;
}

BOOL OpenFileDialog(HWND hWnd, TCHAR szFileName[], TCHAR lpstrFilter[])
{
	OPENFILENAME ofn;
	InitialOpenFileName(&ofn, hWnd, szFileName, lpstrFilter);
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;//�޶��ļ��������
	return GetOpenFileName(&ofn);
}


BOOL SaveFileDialog(HWND hWnd, TCHAR szFileName[], TCHAR lpstrFilter[],TCHAR lpstrDefExt[])
{
	OPENFILENAME ofn;
	InitialOpenFileName(&ofn, hWnd, szFileName, lpstrFilter);

	ofn.Flags = OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt = lpstrDefExt;

	return GetSaveFileName(&ofn);
}