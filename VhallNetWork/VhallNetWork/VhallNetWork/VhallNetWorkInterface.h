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
    //ʹ��https,��Ҫ��cacert.pem�ļ���������������Ŀ¼��acert.pem�ļ�Ϊcurl�������صĸ�֤���ļ�  https://curl.haxx.se/docs/caextract.html
    // Ŀǰ�Ѿ��ϴ���\\192.168.1.104\vhall_media_2\ֱ������\thirdparty
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
    std::string httpUrl; //�����url
    bool bIsEnableProxy;
    std::string proxyIP;
    std::string mCacertPath;
    int proxyPort;
    std::string proxyPwd;
    std::string proxyUser;
    std::string mUserData;   //������ӦЯ�����Զ�������
    bool mbHttpPost;
    bool mbOpenSSL;
    bool mbIsNeedSyncWork; //��Щ������Ҫͬ�����������ֵΪtrue������ͬһ�������ֳ��д���
    bool mbIsDownLoadFile;
    FtpFile mDownLoadFileParam;
};


class HttpManagerInterface
{
public:
    HttpManagerInterface() {};
    virtual ~HttpManagerInterface() {};
    //��ʼ���ӿڱ�����main������ʹ��
    virtual void Init(std::wstring logPath) = 0;
    /*
    *  ���������������
    */
    virtual void SwitchHttpProxy(bool enable, const std::string proxy_ip = std::string(), const int proxy_port = 0, const std::string proxy_user_name = std::string(), std::string proxy_user_pwd = std::string()) = 0;
    //�ͷŽӿڱ�����main������ʹ��
    virtual void Release() = 0;
    //�����http��Ӧ��ص�
    virtual void HttpGetRequest(HTTP_GET_REQUEST& httpGetParam, EventCallback) = 0;
};

VHNETWORK_EXPORT std::shared_ptr<HttpManagerInterface> GetHttpManagerInstance(std::string name = "default");
VHNETWORK_EXPORT void DestoryHttpManagerInstance();