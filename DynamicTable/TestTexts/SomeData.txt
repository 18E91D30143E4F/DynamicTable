#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>

#ifndef FILEREADER_H
#define FILEREADER_H

void GetFileContent(LPCSTR fileName, TCHAR* buffer);
void DeleteAllSpaces(char* strSrc);

#endif /* FILEREADER_H */