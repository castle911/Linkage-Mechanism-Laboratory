#include "FileFunction.h"

#include <tchar.h>
#include <Windows.h>

//�ɹ�ȡ�÷���true ����index=1��õ������ļ���
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

//�Ķ�szFile��ͬ�ڸĶ�ofn.lpstrFile
void InitialOpenFileName(OPENFILENAME *ofn, HWND hwnd, TCHAR szFile[], DWORD nMaxFile)
{
	// Initialize OPENFILENAME
	ZeroMemory(ofn, sizeof(OPENFILENAME));
	ofn->lStructSize = sizeof(OPENFILENAME);
	ofn->hwndOwner = hwnd;
	ofn->lpstrFile = szFile;
	ofn->lpstrFile[0] = TEXT('\0');
	ofn->nMaxFile = nMaxFile;
	ofn->lpstrFilter = TEXT("��������ļ�(*.mds)\0*.mds\0\0");
	ofn->nFilterIndex = 1;
	ofn->lpstrFileTitle = NULL;
	ofn->nMaxFileTitle = 0;
	ofn->lpstrInitialDir = NULL;
}