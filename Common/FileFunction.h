#pragma once

#include <tchar.h>
#include <Windows.h>

bool GetCommandLineByIndex(int index, TCHAR *assigned);
bool GetFileExists(TCHAR filename[]);
void InitialOpenFileName(OPENFILENAME *ofn, HWND hwnd, TCHAR szFile[], TCHAR lpstrFilter[], DWORD nMaxFile= MAX_PATH);


//szFileName�����ļ�����λ��
//lpstrFilterʾ���� TEXT("��������ļ�(*.lml)\0*.lml\0\0")
BOOL OpenFileDialog(HWND hWnd, TCHAR szFileName[], TCHAR lpstrFilter[]);

//szFileName�����ļ�����λ��
//lpstrFilterʾ���� TEXT("��������ļ�(*.lml)\0*.lml\0\0")
//lpstrDefExtʾ���� TEXT("lml")
BOOL SaveFileDialog(HWND hWnd, TCHAR szFileName[], TCHAR lpstrFilter[], TCHAR lpstrDefExt[]);
