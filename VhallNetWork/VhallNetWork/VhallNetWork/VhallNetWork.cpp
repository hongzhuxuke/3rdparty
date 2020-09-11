#include "VhallNetWork.h"
#include <mutex>
#include "vhall_log.h"
#include <iostream>
#include <unordered_map>
#include <fstream>

//https://windows.php.net/downloads/php-sdk/deps/vc15/x86/

static std::unordered_map<std::string, std::shared_ptr<HttpManager>> gGlobalManager;
static std::mutex gGlobalMutex;

HANDLE HttpManager::gTaskEvent[MAX_WORK_THREAD_SIZE] = { nullptr };
std::atomic_bool HttpManager::bManagerThreadRun = false;

static size_t WriteFunction(void *input, size_t uSize, size_t uCount, void *avg)
{
    if (input == nullptr || avg == nullptr) {
        return 0;
    }
    // �����󷵻�����input������avg��(avgΪһ��ʼcurl_easy_setopt���õĲ�)
    size_t uLen = uSize * uCount;
    // string &sBuffer = *reinterpret_cast<string *>(avg);
    // sBuffer.append(reinterpret_cast<const char *>(input), uLen);
    std::string *pStr = (std::string *)(avg);
    pStr->append((char *)(input), uLen);

    // ֻ�з���uSize*uCount�Ż᷵�سɹ�
    return uLen;
}

int my_progress_callback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
    static int i = 0;
    i++;
    if (i % 100 == 0 && dltotal > 0)
    {
        int nPersent = (int)(100.0*dlnow / dltotal);
    }
    return 0;
}

static size_t my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
    struct FtpFile *out = (struct FtpFile *)stream;
    if (out && !out->stream) {
        /* open file for writing */
        out->stream = fopen(out->filename.c_str(), "wb");
        if (!out->stream)
            return -1; /* failure, can't open file to write */
    }
    return fwrite(buffer, size, nmemb, out->stream);
}

HttpGetRunTask::HttpGetRunTask(const HTTP_GET_REQUEST task, EventCallback fun /*= nullptr*/) :
    mTaskParam(task),
    mCallBackFuntion(fun){
};

HttpGetRunTask::~HttpGetRunTask() {

}

bool HttpGetRunTask::IsNeedSyncProcess() {
    return  mTaskParam.mbIsNeedSyncWork;
}

void HttpGetRunTask::HttpRequest() {
   CURLcode code;
   /* CURL *curl_easy_init()
   @ ��ʼ��curl����CURL *curlָ��
   */
   pHttpGetCurl = curl_easy_init();
   if (pHttpGetCurl == NULL) {
      mResponeCode = -1;
      return;
   }

   curl_slist *pHeaders = NULL;
   std::string sBuffer;

   /* struct curl_slist *curl_slist_append(struct curl_slist * list, const char * string);
   @ ���Http��Ϣͷ
   @ ����string����ʽΪname+": "+contents
   */
   std::string header = "Connection: Keep-Alive";
   pHeaders = curl_slist_append(pHeaders, header.c_str());
   header = "Accept-Encoding: gzip, deflate";
   pHeaders = curl_slist_append(pHeaders, header.c_str());
   header = "Accept-Language: zh-CN,en,*";
   pHeaders = curl_slist_append(pHeaders, header.c_str());
   /* CURLcode curl_easy_setopt(CURL *handle, CURLoption option, parameter);
   @ �����������Լ����ò���
   */

   std::string httpUrl = mTaskParam.httpUrl;
   std::string postData;
   if (mTaskParam.mbHttpPost) {
      int pos = httpUrl.find("?");
      std::string postUrl = httpUrl.substr(0, pos);
      mTaskParam.httpUrl = postUrl;
      postData = httpUrl.substr(pos + 1, httpUrl.length());
   }

   LOGD("curl_easy_perform success url:%s", mTaskParam.httpUrl.c_str());
   curl_easy_setopt(pHttpGetCurl, CURLOPT_URL, mTaskParam.httpUrl.c_str());

   if (mTaskParam.bIsEnableProxy){
      std::string proxyIP = mTaskParam.proxyIP + std::string(":") + std::to_string(mTaskParam.proxyPort);
      curl_easy_setopt(pHttpGetCurl, CURLOPT_PROXY, proxyIP.c_str());
      curl_easy_setopt(pHttpGetCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
      curl_easy_setopt(pHttpGetCurl, CURLOPT_PROXYUSERPWD, "user:password");
      std::string proxyUserPwd = mTaskParam.proxyUser + std::string(":") + mTaskParam.proxyPwd;
      curl_easy_setopt(pHttpGetCurl, CURLOPT_PROXYUSERPWD, proxyUserPwd.c_str());
   }

   curl_easy_setopt(pHttpGetCurl, CURLOPT_HTTPHEADER, pHeaders);  // ����ͷ��(Ҫ��pHeader��û����)
   curl_easy_setopt(pHttpGetCurl, CURLOPT_TIMEOUT, 10); 			// ��ʱ(��λS)
   curl_easy_setopt(pHttpGetCurl, CURLOPT_HEADER, 1); 			// �������ݰ���HTTPͷ��
   curl_easy_setopt(pHttpGetCurl, CURLOPT_NOSIGNAL, (long)1);  // ���̴߳�����Ҫ��Ӵ����ã������±�����https://blog.csdn.net/weiyuefei/article/details/51866296
   curl_easy_setopt(pHttpGetCurl, CURLOPT_WRITEFUNCTION, &WriteFunction);	// !���ݻص�����
   curl_easy_setopt(pHttpGetCurl, CURLOPT_WRITEDATA, &sBuffer);			// !���ݻص������ĲΣ�һ��ΪBuffer���ļ�fd
   curl_easy_setopt(pHttpGetCurl, CURLOPT_ACCEPT_ENCODING, "gzip");
   curl_easy_setopt(pHttpGetCurl, CURLOPT_SSL_VERIFYPEER, 0L);
   //httpsʹ��
   if (mTaskParam.mbOpenSSL) {
      curl_easy_setopt(pHttpGetCurl, CURLOPT_SSL_VERIFYPEER, 1L);
      curl_easy_setopt(pHttpGetCurl, CURLOPT_SSL_VERIFYHOST, 2L);
      curl_easy_setopt(pHttpGetCurl, CURLOPT_SSLENGINE_DEFAULT);
      curl_easy_setopt(pHttpGetCurl, CURLOPT_CAINFO,mTaskParam.mCacertPath.c_str());
   }
   //libcur�����POST������
   if (mTaskParam.mbHttpPost) {
      curl_easy_setopt(pHttpGetCurl, CURLOPT_POST, 1L);
      curl_easy_setopt(pHttpGetCurl, CURLOPT_POSTFIELDS, postData.c_str());
      curl_easy_setopt(pHttpGetCurl, CURLOPT_POSTFIELDSIZE, postData.size());
   }

   /*��ʼ��������*/
   code = curl_easy_perform(pHttpGetCurl);
   if (code != CURLE_OK) {
      mResponeCode = -1;
      mHttpMuext.lock();
      if (pHeaders) {
         curl_slist_free_all(pHeaders);
         pHeaders = nullptr;
      }
      if (pHttpGetCurl) {
         curl_easy_cleanup(pHttpGetCurl);
      }
      pHttpGetCurl = nullptr;
      mHttpMuext.unlock();
      LOGD("curl_easy_perform err:%d", code);
      if (mCallBackFuntion) {
         (void)(mCallBackFuntion)("", code, mTaskParam.mUserData);
      }
      return;
   }

   /* CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ... );
   @ ��ȡ������������Ϣ
   @ info��CURLINFO_RESPONSE_CODE  	// ��ȡ���ص�Http��
   CURLINFO_TOTAL_TIME  	// ��ȡ�ܵ���������ʱ��
   CURLINFO_SIZE_DOWNLOAD  	// ��ȡ���ص��ļ���С
   ......
   */
   long retCode = -1;
   long headSize = 0;
   char *url = NULL;
   code = curl_easy_getinfo(pHttpGetCurl, CURLINFO_RESPONSE_CODE, &retCode);
   if (code != CURLE_OK) {
      mResponeCode = code;
      mHttpMuext.lock();
      if (pHeaders) {
         curl_slist_free_all(pHeaders);
         pHeaders = nullptr;
      }
      if (pHttpGetCurl) {
         curl_easy_cleanup(pHttpGetCurl);
      }
      pHttpGetCurl = nullptr;
      mHttpMuext.unlock();
      LOGD("curl_easy_getinfo err:%d", code);
      if (mCallBackFuntion) {
         (void)(mCallBackFuntion)("curl_easy_getinfo err CURLINFO_RESPONSE_CODE", code, mTaskParam.mUserData);
      }
      return;
   }
   if (retCode != 200 && retCode != 0) {
      mResponeCode = retCode;
      mHttpMuext.lock();
      if (pHeaders) {
         curl_slist_free_all(pHeaders);
         pHeaders = nullptr;
      }
      if (pHttpGetCurl) {
         curl_easy_cleanup(pHttpGetCurl);
      }
      pHttpGetCurl = nullptr;
      mHttpMuext.unlock();
      LOGD("retCode err :%d", retCode);
      if (mCallBackFuntion) {
         (void)(mCallBackFuntion)("http request err", retCode, mTaskParam.mUserData);
      }
      return;
   }

   code = curl_easy_getinfo(pHttpGetCurl, CURLINFO_HEADER_SIZE, &headSize);
   if (code != CURLE_OK) {
      mResponeCode = code;
      mHttpMuext.lock();
      if (pHeaders) {
         curl_slist_free_all(pHeaders);
         pHeaders = nullptr;
      }
      if (pHttpGetCurl) {
         curl_easy_cleanup(pHttpGetCurl);
      }
      pHttpGetCurl = nullptr;
      mHttpMuext.unlock();
      LOGD("curl_easy_getinfo err :%d", code);
      if (mCallBackFuntion) {
         (void)(mCallBackFuntion)("curl_easy_getinfo CURLINFO_HEADER_SIZE", code, mTaskParam.mUserData);
      }
      return;
   }

   int size = sBuffer.length();
   if (size - headSize <= 0) {
      mResponeCode = -1;
      mHttpMuext.lock();
      if (pHeaders) {
         curl_slist_free_all(pHeaders);
         pHeaders = nullptr;
      }
      if (pHttpGetCurl) {
         curl_easy_cleanup(pHttpGetCurl);
      }
      pHttpGetCurl = nullptr;
      mHttpMuext.unlock();
      LOGD("size is empty size:%d headSize:%d", size, headSize);
      if (mCallBackFuntion) {
         (void)(mCallBackFuntion)("size is empty", code, mTaskParam.mUserData);
      }
      return;
   }
   char *p = (char *)sBuffer.c_str() + headSize;
   std::string strHttpResponeData = std::string(p, size - headSize);
   if (mCallBackFuntion) {
      (void)(mCallBackFuntion)(strHttpResponeData, code, mTaskParam.mUserData);
   }
   /* void curl_easy_cleanup(CURL * handle);
   @ �ͷ�CURL *curlָ��
   */
   mHttpMuext.lock();
   if (pHeaders) {
      curl_slist_free_all(pHeaders);
      pHeaders = nullptr;
   }
   if (pHttpGetCurl) {
      curl_easy_cleanup(pHttpGetCurl);
   }
   pHttpGetCurl = nullptr;
   mHttpMuext.unlock();
   mResponeCode = 0;
   LOGD("curl_easy_perform success");
}

void HttpGetRunTask::HttpDownLoadFile() {
   CURLcode code;
   /* CURL *curl_easy_init()
   @ ��ʼ��curl����CURL *curlָ��
   */
   pHttpGetCurl = curl_easy_init();
   if (pHttpGetCurl == NULL) {
      mResponeCode = -1;
      return;
   }

   curl_slist *pHeaders = NULL;
   std::string sBuffer;

   /* struct curl_slist *curl_slist_append(struct curl_slist * list, const char * string);
   @ ���Http��Ϣͷ
   @ ����string����ʽΪname+": "+contents
   */
   std::string header = "Connection: Keep-Alive";
   pHeaders = curl_slist_append(pHeaders, header.c_str());
   header = "Accept-Encoding: gzip, deflate";
   pHeaders = curl_slist_append(pHeaders, header.c_str());
   header = "Accept-Language: zh-CN,en,*";
   pHeaders = curl_slist_append(pHeaders, header.c_str());
   /* CURLcode curl_easy_setopt(CURL *handle, CURLoption option, parameter);
   @ �����������Լ����ò���
   */
   curl_easy_setopt(pHttpGetCurl, CURLOPT_URL, mTaskParam.httpUrl.c_str());

   if (mTaskParam.bIsEnableProxy)
   {
      curl_easy_setopt(pHttpGetCurl, CURLOPT_PROXY, mTaskParam.proxyIP.c_str());
      curl_easy_setopt(pHttpGetCurl, CURLOPT_PROXYPORT, mTaskParam.proxyPort);
      curl_easy_setopt(pHttpGetCurl, CURLOPT_PROXYUSERPWD, "user:password");
      std::string proxyUserPwd = mTaskParam.proxyUser + std::string(":") + mTaskParam.proxyPwd;
      curl_easy_setopt(pHttpGetCurl, CURLOPT_PROXYUSERPWD, proxyUserPwd.c_str());
      curl_easy_setopt(pHttpGetCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
   }
   std::string tmp = mTaskParam.mDownLoadFileParam.filename + std::string("_tmp");

   FtpFile fileParam;
   fileParam.filename = tmp;
   curl_easy_setopt(pHttpGetCurl, CURLOPT_HTTPHEADER, pHeaders);  // ����ͷ��(Ҫ��pHeader��û����)
   curl_easy_setopt(pHttpGetCurl, CURLOPT_HEADER, 1); 			// �������ݰ���HTTPͷ��
   curl_easy_setopt(pHttpGetCurl, CURLOPT_NOSIGNAL, (long)1);  // ���̴߳�����Ҫ��Ӵ����ã������±�����https://blog.csdn.net/weiyuefei/article/details/51866296
   curl_easy_setopt(pHttpGetCurl, CURLOPT_WRITEFUNCTION, &my_fwrite);	// !���ݻص�����
   curl_easy_setopt(pHttpGetCurl, CURLOPT_WRITEDATA, &fileParam);
   curl_easy_setopt(pHttpGetCurl, CURLOPT_ACCEPT_ENCODING, "gzip");
   curl_easy_setopt(pHttpGetCurl, CURLOPT_PROGRESSFUNCTION, my_progress_callback);
   curl_easy_setopt(pHttpGetCurl, CURLOPT_PROGRESSDATA, NULL);
   curl_easy_setopt(pHttpGetCurl, CURLOPT_NOPROGRESS, 0L);
   curl_easy_setopt(pHttpGetCurl, CURLOPT_VERBOSE, 1L);
   curl_easy_setopt(pHttpGetCurl, CURLOPT_SSL_VERIFYPEER, 0L);
   //httpsʹ��
   if (mTaskParam.mbOpenSSL) {
      curl_easy_setopt(pHttpGetCurl, CURLOPT_SSL_VERIFYPEER, 1L);
      curl_easy_setopt(pHttpGetCurl, CURLOPT_SSL_VERIFYHOST, 2L);
      curl_easy_setopt(pHttpGetCurl, CURLOPT_SSLENGINE_DEFAULT);
      curl_easy_setopt(pHttpGetCurl, CURLOPT_CAINFO, mTaskParam.mCacertPath.c_str());
   }
   /* CURLcode curl_easy_perform(CURL *handle);
   @ ��ʼ����
   */
   code = curl_easy_perform(pHttpGetCurl);
   if (code != CURLE_OK) {
      mResponeCode = code;
      mHttpMuext.lock();
      if (pHeaders) {
         curl_slist_free_all(pHeaders);
         pHeaders = nullptr;
      }
      if (pHttpGetCurl) {
         curl_easy_cleanup(pHttpGetCurl);
      }
      pHttpGetCurl = nullptr;
      mHttpMuext.unlock();
      LOGD("curl_easy_perform err:%d", code);
      if (mCallBackFuntion) {
         (void)(mCallBackFuntion)("curl_easy_perform", code, mTaskParam.httpUrl);
      }
      return;
   }

   /* CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ... );
   @ ��ȡ������������Ϣ
   @ info��CURLINFO_RESPONSE_CODE  	// ��ȡ���ص�Http��
   CURLINFO_TOTAL_TIME  	// ��ȡ�ܵ���������ʱ��
   CURLINFO_SIZE_DOWNLOAD  	// ��ȡ���ص��ļ���С
   ......
   */
   long retCode = -1;
   long headSize = 0;
   char *url = NULL;
   code = curl_easy_getinfo(pHttpGetCurl, CURLINFO_RESPONSE_CODE, &retCode);
   if (code != CURLE_OK) {
      mResponeCode = code;
      mHttpMuext.lock();
      if (pHeaders) {
         curl_slist_free_all(pHeaders);
         pHeaders = nullptr;
      }
      if (pHttpGetCurl) {
         curl_easy_cleanup(pHttpGetCurl);
      }
      pHttpGetCurl = nullptr;
      mHttpMuext.unlock();
      LOGD("curl_easy_getinfo err:%d", code);
      if (mCallBackFuntion) {
         (void)(mCallBackFuntion)("curl_easy_getinfo err CURLINFO_RESPONSE_CODE", code, mTaskParam.httpUrl);
      }
      return;
   }
   if (retCode != 200 && retCode != 0) {
      mResponeCode = code;
      mHttpMuext.lock();
      if (pHeaders) {
         curl_slist_free_all(pHeaders);
         pHeaders = nullptr;
      }
      if (pHttpGetCurl) {
         curl_easy_cleanup(pHttpGetCurl);
      }
      pHttpGetCurl = nullptr;
      mHttpMuext.unlock();
      LOGD("curl_easy_getinfo err:%d", code);
      LOGD("retCode err :%d", retCode);
      if (mCallBackFuntion) {
         (void)(mCallBackFuntion)("http request err", retCode, mTaskParam.httpUrl);
      }
      return;
   }

   code = curl_easy_getinfo(pHttpGetCurl, CURLINFO_HEADER_SIZE, &headSize);
   if (code != CURLE_OK) {
      mResponeCode = code;
      mHttpMuext.lock();
      if (pHeaders) {
         curl_slist_free_all(pHeaders);
         pHeaders = nullptr;
      }
      if (pHttpGetCurl) {
         curl_easy_cleanup(pHttpGetCurl);
      }
      pHttpGetCurl = nullptr;
      mHttpMuext.unlock();
      LOGD("curl_easy_getinfo err :%d", code);
      if (mCallBackFuntion) {
         (void)(mCallBackFuntion)("curl_easy_getinfo CURLINFO_HEADER_SIZE", code, mTaskParam.httpUrl);
      }
      return;
   }
   if (fileParam.stream)
      fclose(fileParam.stream); /* close the local file */

/* void curl_easy_cleanup(CURL * handle);
@ �ͷ�CURL *curlָ��
*/
   mHttpMuext.lock();
   if (pHeaders) {
      curl_slist_free_all(pHeaders);
      pHeaders = nullptr;
   }
   if (pHttpGetCurl) {
      curl_easy_cleanup(pHttpGetCurl);
   }
   pHttpGetCurl = nullptr;
   mHttpMuext.unlock();
   CopyDownLoadFile(tmp.c_str(), mTaskParam.mDownLoadFileParam.filename.c_str(), headSize);
   LOGD("curl_easy_getinfo err :%d", code);
   if (mCallBackFuntion) {
      (void)(mCallBackFuntion)("download success", code, mTaskParam.httpUrl);
   }
   mResponeCode = 0;
}

void HttpGetRunTask::CopyDownLoadFile(const char* src, const char* dst, int headLen)
{
   using namespace std;
   ifstream in(src, ios::binary);
   ofstream out(dst, ios::binary);
   if (!in.is_open()) {
      return;
   }
   if (!out.is_open()) {
      return;
   }
   if (src == dst) {
      return;
   }
   char buf[2048];
   long long totalBytes = 0;
   in.seekg(headLen, ios::beg);
   while (in)
   {
      //read��in���ж�ȡ2048�ֽڣ�����buf�����У�ͬʱ�ļ�ָ������ƶ�2048�ֽ�
      //������2048�ֽ������ļ���β������ʵ����ȡ�ֽڶ�ȡ��
      in.read(buf, 2048);
      //gcount()������ȡ��ȡ���ֽ�����write��buf�е�����д��out����
      out.write(buf, in.gcount());
      totalBytes += in.gcount();
   }
   in.close();
   out.close();
   int nRet = remove(src);
   return;
}

void HttpGetRunTask::RunTask() {
   if (mTaskParam.mbIsDownLoadFile) {
      HttpDownLoadFile();
   } else {
      HttpRequest();
   }
}

void HttpGetRunTask::SetCacertPath(std::string path) {
   mCacertPath = path;
}

HttpManager::HttpManager()
{
  mPostToThreadIndex = 0;
  mbIsInit = false;
}

HttpManager::~HttpManager()
{
    LOGD("HttpManager::~HttpManager");
    //Release();

    LOGD("HttpManager::~HttpManager  end");
}

void HttpManager::Init(std::wstring logPath) {
    if (!mbIsInit) {
        InitLog(logPath);
        /* CURLcode curl_global_init(long flags)
        @ ��ʼ��libcurl��ȫ��ֻ���һ��
        @ flags:	CURL_GLOBAL_DEFAULT 	// ��ͬ��CURL_GLOBAL_ALL
        CURL_GLOBAL_ALL     	// ��ʼ�����еĿ��ܵĵ���
        CURL_GLOBAL_SSL     	// ��ʼ��֧�ְ�ȫ�׽��ֲ�
        CURL_GLOBAL_WIN32   	// ��ʼ��win32�׽��ֿ�
        CURL_GLOBAL_NOTHING 	// û�ж���ĳ�ʼ��
        ......
        //https://windows.php.net/downloadS/php-sdk/deps/vc15/x86/
        */

        LOGD("Init");
        CURLcode code;
        code = curl_global_init(CURL_GLOBAL_DEFAULT);
        if (mThreadHandleList.size() == 0) {
            LOGD("CreateThread");
            bManagerThreadRun = true;
            for (int i = 0; i < MAX_WORK_THREAD_SIZE; i++) {
                gTaskEvent[i] = ::CreateEvent(NULL, FALSE, FALSE, NULL);
                std::thread *newThread = new std::thread(HttpManager::ThreadProcess, this);
                if (newThread) {
                    LOGD("CreateThread success %d", i);
                    mThreadHandleList.push_back(newThread);
                    mMainWorkThreadId = newThread->get_id();
                }
            }
        }
    }

    LOGD("HttpManager::Init end");
}

void HttpManager::Release() {
    /* void curl_global_cleanup(void);
    @ ��ʼ��libcurl��ȫ��Ҳֻ���һ��
    */
    LOGD("Release");
    bManagerThreadRun = false;    
    for (int i = 0; i < MAX_WORK_THREAD_SIZE; i++) {
        ::SetEvent(gTaskEvent[i]);
        std::thread *taskThread = mThreadHandleList.at(i);
        if (taskThread) {
            LOGD("taskThread->join()");
            taskThread->join();
            LOGD("taskThread->join() end");
            delete taskThread;
            taskThread = nullptr;
        }
        ::CloseHandle(gTaskEvent[i]);
        gTaskEvent[i] = nullptr;
    }
    mThreadHandleList.clear();
    ReleaseTask();
    LOGD("end");
    curl_global_cleanup();
    LOGD("curl_global_cleanup end");
}

void HttpManager::ProcessTask() {
    std::shared_ptr<HttpGetRunTask> event = nullptr;
    if (bManagerThreadRun) {
        std::unique_lock<std::mutex> lock(mTaskListMutex);
        if (mTaskList.size() > 0) {
            event = mTaskList.front();
            if (event->IsNeedSyncProcess()) {
                std::thread::id curThreadId = std::this_thread::get_id();
                if (curThreadId != mMainWorkThreadId) {
                    return;
                }
            }
            mTaskList.pop_front();
        }
        else {
            return;
        }
    }
    if (event) {
       event->RunTask();
    }
}

void HttpManager::SwitchHttpProxy(bool enable, const std::string proxy_ip , const int proxy_port, const std::string proxy_user_name, std::string proxy_user_pwd) {
   bIsEnableProxy = enable;
   proxyPort = proxy_port;
   proxyIP = proxy_ip;
   proxyPwd = proxy_user_pwd;
   proxyUser = proxy_user_name;
}

DWORD WINAPI HttpManager::ThreadProcess(LPVOID p) {
    LOGD("ThreadProcess");
    while (bManagerThreadRun) {
        DWORD ret = WaitForMultipleObjects(MAX_WORK_THREAD_SIZE, gTaskEvent, TRUE, MAX_THREAD_WAIT_TIME);
        if (p) {
            HttpManager* sdk = (HttpManager*)(p);
            if (sdk) {
                sdk->ProcessTask();
            }
        }
    }
    LOGD("ThreadProcess end");
    return 0;
}

void HttpManager::ReleaseTask() {
    std::unique_lock<std::mutex> lock(mTaskListMutex);
    std::list<std::shared_ptr<HttpGetRunTask>>::iterator iter = mTaskList.begin();
    if (mTaskList.size() > 0) {
        while (iter != mTaskList.end()) {
            LOGD("exitThread");
            std::shared_ptr<HttpGetRunTask> event = *iter;
            event.reset();
            event = nullptr;
            iter = mTaskList.erase(iter);
        }
    }
}

void HttpManager::HttpGetRequest(HTTP_GET_REQUEST& httpGetParam, EventCallback fun) {
    LOGD("httpGetParam : %s", httpGetParam.httpUrl.c_str())    
    if (bManagerThreadRun) {
        std::unique_lock<std::mutex> lock(mTaskListMutex);
        httpGetParam.SetProxy(bIsEnableProxy, proxyIP, proxyPort, proxyPwd, proxyUser);
        std::shared_ptr<HttpGetRunTask> task = std::make_shared<HttpGetRunTask>(httpGetParam, fun);
        LOGD("push back task");
        mTaskList.push_back(task);
        for (int i = 0; i < MAX_WORK_THREAD_SIZE; i++) {
            if (gTaskEvent[i]) {
                ::SetEvent(gTaskEvent[i]);
            }
        }
    }
}

VHNETWORK_EXPORT std::shared_ptr<HttpManagerInterface> GetHttpManagerInstance(std::string name) {
    std::unique_lock<std::mutex> lock(gGlobalMutex);
    auto it_manager = gGlobalManager.find(name);
    if (it_manager == gGlobalManager.end() || it_manager->second == nullptr) {
      std::shared_ptr<HttpManager> manager(new HttpManager());
      gGlobalManager[name] = manager;
      return manager;
    }
    return it_manager->second;
}

VHNETWORK_EXPORT void DestoryHttpManagerInstance() {
    std::unique_lock<std::mutex> lock(gGlobalMutex);
    for (const auto& iter : gGlobalManager) {
      if (iter.second) {
        gGlobalManager.erase(iter.first);
      }
    }
}
