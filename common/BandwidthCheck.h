#ifndef __BANDWIDTH_CHECK_INCLUDE__
#define __BANDWIDTH_CHECK_INCLUDE__
#ifndef MAX_BUFFER
#define MAX_BUFFER 256
#endif
class BandwidthCheck {
public:
   BandwidthCheck();
   ~BandwidthCheck();
   bool StratCheck(const char* rtmpUrl);
   void GetBandwidth(int& playKbps, int& publishKbps);                           
private:                                                                    
   bool CheckLoop();
   static DWORD __stdcall CheckThread(LPVOID lpUnused);   
private:
   char mRtmpUrl[MAX_BUFFER];
   bool mIsChecking;
   HANDLE mBWCheckThread;
   int mPlayKbps;
   int mPublishKbps;
};

#endif

