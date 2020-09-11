#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "BandwidthCheck.h"
#include "srs-librtmp\src\srs_librtmp.h"
#include "common/Logging.h"

//#pragma comment (lib, "srs-librtmp.lib")


BandwidthCheck::BandwidthCheck() {
   mIsChecking = false;
   mBWCheckThread = NULL;
   mPlayKbps = 0;
   mPublishKbps = 0;
}


BandwidthCheck::~BandwidthCheck() {
   if (mBWCheckThread) {
      DWORD waitResult = WaitForSingleObject(mBWCheckThread, 5000);
      if (waitResult == WAIT_OBJECT_0) {
         gLogger->logInfo("BWCheckThread  thread exit.");
      } else if (waitResult == WAIT_TIMEOUT) {
         gLogger->logError("BWCheckThread  thread shutdown timeout.");
      } else {
         gLogger->logError("BWCheckThread thread shutdown failed.");
      }
      CloseHandle(mBWCheckThread);
      mBWCheckThread = NULL;
   }
}
bool BandwidthCheck::StratCheck(const char* rtmpUrl) {
   if (mIsChecking) {
      return false;
   }
   strcpy_s(mRtmpUrl, MAX_BUFFER, rtmpUrl);
   mIsChecking = true;
   mBWCheckThread = CreateThread(NULL, 0, BandwidthCheck::CheckThread, this, 0, NULL);
   if (!mBWCheckThread) {
      DWORD error = GetLastError();
       gLogger->logError("BandwidthCheck::StratCheck: CreateThread failed, error = 0x%08x.",
      error);
      mIsChecking = false;
      return false;
   }
   return true;
}
void BandwidthCheck::GetBandwidth(int& playKbps, int& publishKbps) {
   playKbps = mPlayKbps;
   publishKbps = mPublishKbps;
}

bool BandwidthCheck::CheckLoop() {
   int  ret = 0;
   srs_rtmp_t rtmp;
   // srs debug info.
   char srs_server_ip[128];
   char srs_server[128];
   char srs_primary[128];
   char srs_authors[128];
   char srs_version[32];
   int srs_id = 0;
   int srs_pid = 0;
   // bandwidth test data.
   int64_t start_time = 0;
   int64_t end_time = 0;
   int play_kbps = 0;
   int publish_kbps = 0;
   int play_bytes = 0;
   int publish_bytes = 0;
   int play_duration = 0;
   int publish_duration = 0;

   // set to zero.
   srs_server_ip[0] = 0;
   srs_server[0] = 0;
   srs_primary[0] = 0;
   srs_authors[0] = 0;
   srs_version[0] = 0;

   printf("RTMP bandwidth check/test with server.\n");
   printf("srs(simple-rtmp-server) client librtmp library.\n");
   printf("version: %d.%d.%d\n", srs_version_major(), srs_version_minor(), srs_version_revision());


   rtmp = srs_rtmp_create2(mRtmpUrl);

   srs_human_trace("bandwidth check/test url: %s", mRtmpUrl);

   if ((ret = srs_rtmp_handshake(rtmp)) != 0) {
      srs_human_trace("simple handshake failed.");
      goto rtmp_destroy;
   }
   srs_human_trace("simple handshake success");

   if ((ret = srs_rtmp_connect_app2(rtmp,
      srs_server_ip, srs_server, srs_primary, srs_authors, srs_version,
      &srs_id, &srs_pid)) != 0) {
      srs_human_trace("connect vhost/app failed.");
      goto rtmp_destroy;
   }
   srs_human_trace("connect vhost/app success");

   if ((ret = srs_rtmp_bandwidth_check(rtmp,
      &start_time, &end_time, &play_kbps, &publish_kbps,
      &play_bytes, &publish_bytes, &play_duration, &publish_duration)) != 0
      ) {
      srs_human_trace("bandwidth check/test failed.");
      goto rtmp_destroy;
   }
   srs_human_trace("bandwidth check/test success");

   gLogger->logInfo("\n%s, %s, %s\n"
                    "%s, %s, srs_pid=%d, srs_id=%d\n"
                    "duration: %dms(%d+%d)\n"
                    "play: %dkbps\n"
                    "publish: %dkbps",
                    (char*)srs_server, (char*)srs_primary, (char*)srs_authors,
                    (char*)srs_server_ip, (char*)srs_version, srs_pid, srs_id,
                    (int)(end_time - start_time), play_duration, publish_duration,
                    play_kbps,
                    publish_kbps);
   mPlayKbps = play_kbps;
   mPublishKbps = publish_kbps;

   srs_rtmp_destroy(rtmp);
   mIsChecking = false;
   return true;;
rtmp_destroy:
   srs_rtmp_destroy(rtmp);
   gLogger->logError("{\"code\":%d,"
                     "\"srs_server\":\"%s\", "
                     "\"srs_primary\":\"%s\", "
                     "\"srs_authors\":\"%s\", "
                     "\"srs_server_ip\":\"%s\", "
                     "\"srs_version\":\"%s\", "
                     "\"srs_pid\":%d, "
                     "\"srs_id\":%d, "
                     "\"duration\":%d, "
                     "\"play_duration\":%d, "
                     "\"publish_duration\":%d,"
                     "\"play_kbps\":%d, "
                     "\"publish_kbps\":%d"
                     "}",
                     ret,
                     (char*)srs_server, (char*)srs_primary, (char*)srs_authors,
                     (char*)srs_server_ip, (char*)srs_version, srs_pid, srs_id,
                     (int)(end_time - start_time), play_duration, publish_duration,
                     play_kbps, publish_kbps);

   srs_human_trace(" ");
   srs_human_trace("completed");
   mIsChecking = false;
   return false;
}
DWORD __stdcall BandwidthCheck::CheckThread(LPVOID lpUnused) {
   BandwidthCheck* thisObj = (BandwidthCheck*)lpUnused;
   thisObj->CheckLoop();
   return 0;
}

