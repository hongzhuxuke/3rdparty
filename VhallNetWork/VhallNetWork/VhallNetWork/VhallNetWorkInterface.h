#pragma once

#ifdef  VHNETWORK_EXPORT
#define VHNETWORK_EXPORT     __declspec(dllimport)
#else
#define VHNETWORK_EXPORT     __declspec(dllexport)
#endif

#include <functional>



typedef std::function<void(const std::string& msg, int code, const std::string userData)> EventCallback;
typedef void(*HTTP_GET_CALLBACK_FUNCTION)(std::string, int);
typedef int(*HTTP_DOWNLOAD_PROGRESS)(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);


struct FtpFile {
   std::string filename;
   FILE *stream = NULL;
};

class HTTP_GET_REQUEST {
public:
    HTTP_GET_REQUEST(std::string url, std::string userData = std::string()) :
        httpUrl(url),
        mUserData(userData),
        mbHttpPost(false),
        mbOpenSSL(false),
        mbIsNeedSyncWork(false),
        mbIsDownLoadFile(false){
    };

    HTTP_GET_REQUEST(const HTTP_GET_REQUEST& right) {
        httpUrl = right.httpUrl,
        mUserData = right.httpUrl,
        bIsEnableProxy = right.bIsEnableProxy,
        proxyIP = right.proxyIP,
        proxyPort = right.proxyPort,
        proxyPwd = right.proxyPwd,
        proxyUser = right.proxyUser,
        mbHttpPost = right.mbHttpPost,
        mbOpenSSL = right.mbOpenSSL,
        mbIsNeedSyncWork = right.mbIsNeedSyncWork;
        mbIsDownLoadFile = right.mbIsDownLoadFile;
        mDownLoadFileParam.filename = right.mDownLoadFileParam.filename;
        mDownLoadFileParam.stream = right.mDownLoadFileParam.stream;
        mCacertPath = right.mCacertPath;
    };

    void SetHttpPost(bool httpPost) {
        mbHttpPost = httpPost;
    };
    //使用https,需要将cacert.pem文件拷贝到程序运行目录，acert.pem文件为curl官网下载的根证书文件  https://curl.haxx.se/docs/caextract.html
    // 目前已经上传到\\192.168.1.104\vhall_media_2\直播助手\thirdparty
    void SetHttps(bool enableHttps) {
        mbOpenSSL = enableHttps;
    }

    void SetProxy(bool enableProxy, std::string ip , int port , std::string pwd , std::string proxy_user) {
       bIsEnableProxy = enableProxy;
       proxyIP = ip;
       proxyPort = port;
       proxyPwd = pwd;
       proxyUser = proxy_user;
    }

    void SetEnableDownLoadFile(bool enable, const std::string fileName) {
       mDownLoadFileParam.filename = fileName;
       mbIsDownLoadFile = true;
    }

    void SetCacertPath(std::string path) {
       mCacertPath = path;
    }
public:
    std::string httpUrl; //请求的url
    bool bIsEnableProxy;
    std::string proxyIP;
    std::string mCacertPath;
    int proxyPort;
    std::string proxyPwd;
    std::string proxyUser;
    std::string mUserData;   //请求响应携带的自定义数据
    bool mbHttpPost;
    bool mbOpenSSL;
    bool mbIsNeedSyncWork; //有些请求需要同步处理，如果此值为true，则在同一个工作现场中处理。
    bool mbIsDownLoadFile;
    FtpFile mDownLoadFileParam;
};


class HttpManagerInterface
{
public:
    HttpManagerInterface() {};
    virtual ~HttpManagerInterface() {};
    //初始化接口必须在main函数中使用
    virtual void Init(std::wstring logPath) = 0;
    /*
    *  设置网络代理请求
    */
    virtual void SwitchHttpProxy(bool enable, const std::string proxy_ip = std::string(), const int proxy_port = 0, const std::string proxy_user_name = std::string(), std::string proxy_user_pwd = std::string()) = 0;
    //释放接口必须在main函数中使用
    virtual void Release() = 0;
    //请求的http响应与回调
    virtual void HttpGetRequest(HTTP_GET_REQUEST& httpGetParam, EventCallback) = 0;
};

VHNETWORK_EXPORT std::shared_ptr<HttpManagerInterface> GetHttpManagerInstance(std::string name = "default");
VHNETWORK_EXPORT void DestoryHttpManagerInstance();