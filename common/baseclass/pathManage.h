#ifndef __PATH_MANAGE_H__
#define __PATH_MANAGE_H__
#include <iostream>
#include <string>

#include <Windows.h>
using namespace std;



wstring GetAppPath();
wstring GetAppDataPath();
__int64 CompareFileTime(FILETIME time1, FILETIME time2);

void initPreData();
bool GetSysMemory(int& nMemTotal, int& nMemUsed);
double CalCpuUtilizationRate();

#endif //__PATH_MANAGE_H__