#pragma once

#include <mutex>
#include "VhallNetWorkInterface.h"
#include <curl/curl.h>
#include <list>
#include <atomic>
#include <vector>
#include <memory>

#define MAX_WORK_THREAD_SIZE    4
#define MAX_THREAD_WAIT_TIME    1000


class HttpGetRunTask
{
public:
    HttpGetRunTask(const HTTP_GET_REQUEST task, EventCallback fun = nullptr);
    ~HttpGetRunTask();

    void RunTask();
    void SetCacertPath(std::string path);
    bool IsNeedSyncProcess();
     
private:
   void HttpRequest();
   void HttpDownLoadFile();
   void CopyDownLoadFile(const char* src, const char* dst, int headLen);
private:
    HTTP_GET_REQUEST mTaskParam;
    EventCallback mCallBackFuntion = nullptr;
    int mResponeCode = -1;
    std::mutex mHttpMuext;
    std::string mCacertPath;
    CURL *pHttpGetCurl;

};


class HttpDownLoadRunTask 
{
public:
    HttpDownLoadRunTask(const HTTP_GET_REQUEST task, HTTP_GET_CALLBACK_FUNCTION fun = nullptr, HTTP_DOWNLOAD_PROGRESS progress_callback = nullptr) :
        mTaskParam(task),
        mCallBackFuntion(fun) {
    };
    ~HttpDownLoadRunTask() {};

private:
    HTTP_GET_REQUEST mTaskParam;
    HTTP_GET_CALLBACK_FUNCTION mCallBackFuntion = nullptr;
    int mResponeCode = -1;

    CURL *pHttpGetCurl = NULL;
    std::mutex mHttpGetCurlMutex;
    FtpFile mDownLoadFileParam;
    std::string mFileName;
};


class HttpManager : public HttpManagerInterface
{
public:
    HttpManager();
    virtual ~HttpManager();

    void Init(std::wstring logPath);
    void Release();
    void HttpGetRequest(HTTP_GET_REQUEST& httpGetParam, EventCallback);
    void SwitchHttpProxy(bool enable, const std::string proxy_ip = std::string(), const int proxy_port = 0, const std::string proxy_user_name = std::string(), std::string proxy_user_pwd = std::string());
    void ProcessTask();
    static DWORD WINAPI ThreadProcess(LPVOID p);

private:
    void ReleaseTask();

private:
    std::mutex mTaskListMutex;
   // std::list<HttpGetRunTask*> mTaskList;
    std::list<std::shared_ptr<HttpGetRunTask>> mTaskList;

    std::vector<std::thread*> mThreadHandleList;
    std::thread::id  mMainWorkThreadId;

    static HANDLE gTaskEvent[MAX_WORK_THREAD_SIZE];
    static std::atomic_bool bManagerThreadRun;

    std::atomic_int mPostToThreadIndex;

    std::atomic_bool mbIsInit;

    bool bIsEnableProxy = false;
    int proxyPort;
    std::string proxyIP;
    std::string proxyPwd;
    std::string proxyUser;
};
