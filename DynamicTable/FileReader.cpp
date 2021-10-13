#include "FileReader.h"

void GetFileContent(LPCSTR fileName, TCHAR* buffer)
{
	memset(buffer, 0, strlen(buffer));
	DWORD read = -1;
	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (!ReadFile(hFile, static_cast<LPVOID>(buffer), 4096, &read, NULL))
	{
		return;
	}
}

#define _CRT_SECURE_NO_WARNINGS

void DeleteAllSpaces(char* strSrc)
{
	char* dst = strSrc;
	while (*dst = *strSrc++) if (*dst != ' ') dst++;
}