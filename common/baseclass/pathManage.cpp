
#include "pathManage.h"
#include <stdio.h>
#include <ShlObj.h>
#include<iostream>

static FILETIME g_preidleTime;
static FILETIME g_prekernelTime;
static FILETIME g_preuserTime;

__int64 CompareFileTime(FILETIME time1, FILETIME time2)
{
	__int64 a = time1.dwHighDateTime << 32 | time1.dwLowDateTime;
	__int64 b = time2.dwHighDateTime << 32 | time2.dwLowDateTime;
	return   (b - a);
}

bool GetSysMemory(int& nMemTotal, int& nMemUsed)
{
	MEMORYSTATUSEX memsStat;
	memsStat.dwLength = sizeof(memsStat);
	if (!GlobalMemoryStatusEx(&memsStat))//如果获取系统内存信息不成功，就直接返回  
	{
		nMemTotal = -1;
		nMemUsed = -1;
		return false;
	}
	int nMemFree = memsStat.ullAvailPhys / (1024.0*1024.0);
	nMemTotal = memsStat.ullTotalPhys / (1024.0*1024.0);
	nMemUsed = nMemTotal - nMemFree;
	return true;
}


wstring  GetAppPath() {
   WCHAR tmp[1024 * 10];
   tmp[0] = 0;
   if (GetModuleFileNameW(NULL, tmp, sizeof(tmp)) > 0) {
      WCHAR* dst = NULL;
      WCHAR* p = tmp;
      while (*p) {
         if (*p == L'\\')
            dst = p;
         p = CharNextW(p);
      }

      if (dst)
         *(dst + 1) = 0;
   }
   return tmp;
}


wstring  GetAppDataPath() {
   WCHAR tmp[1024 * 10];
   tmp[0] = 0;
   SHGetFolderPathW(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, tmp);
   wcscat_s(tmp, _MAX_PATH, L"\\VHallHelper\\");
   return tmp;
}

double CalCpuUtilizationRate()
{
	FILETIME idleTime;
	FILETIME kernelTime;
	FILETIME userTime;
	GetSystemTimes(&idleTime, &kernelTime, &userTime);


	__int64 idle = CompareFileTime(g_preidleTime, idleTime);
	__int64 kernel = CompareFileTime(g_prekernelTime, kernelTime);
	__int64 user = CompareFileTime(g_preuserTime, userTime);


	double cpuf = kernel;
	cpuf += user;
	cpuf -= idle;
	cpuf *= 100.0f;
	double base = kernel + user;
	cpuf /= base;

	g_preidleTime = idleTime;
	g_prekernelTime = kernelTime;
	g_preuserTime = userTime;
	return cpuf;
}

void initPreData()
{
	memset(&g_preidleTime, 0, sizeof(FILETIME));
	memset(&g_prekernelTime, 0, sizeof(FILETIME));
	memset(&g_preuserTime, 0, sizeof(FILETIME));
}
