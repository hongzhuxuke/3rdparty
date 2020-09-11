#ifndef _SOCKET_CLIENT_INCLUDE_H__
#define _SOCKET_CLIENT_INCLUDE_H__
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h> 
#include <windows.h> 
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <process.h>
#include <string> 
#include <fstream>     
#include <map>
#include <vector>
using namespace std;
class SocketClient {
public:
   SocketClient(const char* addr, u_short port);
   ~SocketClient();
   bool Start();
   void Stop();
   bool IsConnected();
   bool Send(const char *sendBuff, const int& dataSize);
   void WaitEvent(const DWORD& timeoutInMs);
   bool StopImmediately();
private:
   bool initSocket();
   DWORD __stdcall workerThreadProc();
   static DWORD __stdcall workerThread(void *);
private:
   SOCKET mClientSocket;
   string mServerAddr;
   u_short mPort;
   HANDLE mWorkerThread;
   HANDLE mSendEvent;
   HANDLE mTimeoutEvent;
   bool mRunning = false;
   bool mStart = false;
   vector<string> mSendMsg;
   CRITICAL_SECTION mSendCS;
   bool mStopImmediately = false;
};
#endif

