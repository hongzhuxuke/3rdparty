#include "stdafx.h"
#include "SocketClient.h"
#include "Defs.h"
#include "json.h"

LPWSTR GetLastErrorText(DWORD);
SocketClient::SocketClient(const char* addr, u_short port) {
   mClientSocket = NULL;
   mServerAddr = addr;
   mPort = port;
   mStart = false;
   InitializeCriticalSection(&mSendCS);
   mTimeoutEvent = NULL;
}
SocketClient::~SocketClient() {
   DeleteCriticalSection(&mSendCS);
}
bool SocketClient::Start() {
   if (initSocket() == false)
      return false;
   mRunning = true;
   mStart = true;
   mSendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
   mTimeoutEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
   if (mSendEvent == NULL || mTimeoutEvent == NULL) {
      TRACE6("Server::startServer CreateEvent failed  %ws", GetLastErrorText(GetLastError()));
      mRunning = false;
      mStart = false;
      return false;
   }
   mWorkerThread = CreateThread(0, 0, SocketClient::workerThread, this,
                                NULL, NULL);
   //accept client, and create thread.
   if (mWorkerThread == NULL) {
      TRACE6("Server::startServer  _beginthread acceptThread failed %d - %ws", mPort,GetLastErrorText(GetLastError()));
      mRunning = false;
      mStart = false;
      return false;
   }
   return true;
}
void SocketClient::Stop() {
	TRACE6("%s", __FUNCTION__);
   shutdown(mClientSocket, 2);
   mStart = false;
   SetEvent(mSendEvent);
}
bool SocketClient::IsConnected() {
   return mRunning;
}
bool SocketClient::Send(const char *sendBuff, const int&) {

   EnterCriticalSection(&mSendCS);
   if (mSendMsg.size() < 10) {
	   TRACE6("%s %s", __FUNCTION__, sendBuff);
	   mSendMsg.push_back(sendBuff);
      SetEvent(mSendEvent);

   }
   LeaveCriticalSection(&mSendCS);
   return true;
}

void SocketClient::WaitEvent(const DWORD& timeoutInMs) {
	if (mTimeoutEvent)
	{
		TRACE6("%s WaitEvent %lu", __FUNCTION__, timeoutInMs);
		WaitForSingleObject(mTimeoutEvent, timeoutInMs);
   }
   else {
	   TRACE6("%s Sleep %lu", __FUNCTION__, timeoutInMs);
      Sleep(timeoutInMs);
   }
}

bool SocketClient::initSocket() {
   int ret = false;
   struct sockaddr_in serverAddr;
   SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (INVALID_SOCKET == sock) {
      TRACE6("Server::initSocket  socket() failed %d - %ws", mPort,GetLastErrorText(GetLastError()));
      return false;
   }
   //unsigned long ul = 1;
   //ret = ioctlsocket(sock, FIONBIO, &ul);
   int netTimeout = 1000;
   //setsockopt(sock, SO_SNDTIMEO,(char *)&netTimeout, sizeof(int));
   setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&netTimeout, sizeof(int));
   setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&netTimeout, sizeof(int));

   serverAddr.sin_family = AF_INET;
   serverAddr.sin_addr.s_addr = inet_addr(mServerAddr.c_str());
   serverAddr.sin_port = htons(mPort);
   memset(serverAddr.sin_zero, 0x00, 8);
   ret = connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
   if (SOCKET_ERROR == ret) {
      TRACE6("Server::initSocket  connect %s failed  - %ws", mServerAddr.c_str(),GetLastErrorText(GetLastError()));
      closesocket(sock);
      return false;
   }
   mClientSocket = sock;
   return true;
}
static int VHallDebug(const char * format, ...) {
#define LOGBUGLEN 1024
   char logBuf[LOGBUGLEN];
   va_list arg_ptr;
   va_start(arg_ptr, format);
   //int nWrittenBytes = sprintf_s(logBuf, format, arg_ptr);
   int nWrittenBytes = vsnprintf_s(logBuf, LOGBUGLEN, format, arg_ptr);
   va_end(arg_ptr);
   WCHAR   wstr[LOGBUGLEN * 2] = { 0 };
   MultiByteToWideChar(CP_ACP, 0, logBuf, -1, wstr, sizeof(wstr));
   OutputDebugString(wstr);
   return nWrittenBytes;
}
bool SocketClient::StopImmediately(){
   return mStopImmediately;
}
#define SIZE_RECV 35536
DWORD __stdcall SocketClient::workerThreadProc() {
   char recvBuffer[SIZE_RECV];
   int len;
   DWORD waitResult;
   int ret=0;
   while (mStart) {
      string willSend;
      EnterCriticalSection(&mSendCS);
      if (mSendMsg.size() > 0) {
         willSend = mSendMsg.front();
         mSendMsg.erase(mSendMsg.begin());
      }
      LeaveCriticalSection(&mSendCS);
      if (willSend.length()) {
         ret = send(mClientSocket, willSend.c_str(), willSend.length(), 0);
         if (ret == SOCKET_ERROR) {
			 //int ret2 = WSAGetLastError();

            TRACE6("SocketClient::serviceThreadProc() send failed  - %ws", GetLastErrorText(GetLastError()));
            //break; //ÐèÒª¶à²âÊÔ
         }
         VHallDebug("Send ret=%d\n",ret);
      } else {
         waitResult = WaitForSingleObject(mSendEvent, -1);
         continue;
      }
      memset(recvBuffer, 0, SIZE_RECV);
      len = recv(mClientSocket, (char *)recvBuffer, SIZE_RECV, 0);
      waitResult = GetLastError();
      if (len == SOCKET_ERROR && EAGAIN == waitResult) {
         TRACE6("Conn::serviceThreadProc() recv failed  - %ws", GetLastErrorText(waitResult));
         break;
      }

      VHJson::Reader reader;
      VHJson::Value value;
      if (reader.parse(recvBuffer, value)){
         string msgType = value["type"].asString();
         if(MSG_TYPE_ENGINE_STOP == msgType) {   
            mStopImmediately = true;
            closesocket(mClientSocket);
            break;
         }
      }
      
#ifndef _DEBUG
      ;
      //g_pLogger->logInfo(" SocketClient::workerThreadProc recv data");
#else
      TRACE6(" SocketClient::workerThreadProc recv %s", recvBuffer);
#endif
   }
   mRunning = false;
   SetEvent(mTimeoutEvent);
   return 0;
}
DWORD __stdcall SocketClient::workerThread(void *pParam) {
   SocketClient *s = (SocketClient *)pParam;
   return  s->workerThreadProc();
}
