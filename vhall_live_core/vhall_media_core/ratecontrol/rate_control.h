#ifndef __RATECONTROL_H__
#define __RATECONTROL_H__
#include <limits.h>
#include "buffer_state.h"
#include "../encoder/encoder_info.h"
#include "talk/base/messagehandler.h"
#include "../common/live_define.h"
#include <list>
#include <vector>

#define MAX_HISTORY_INFO 10
#define RESOLUTION_LEVEL_ONE 360
#define RESOLUTION_LEVEL_TWO 480
#define RESOLUTION_LEVEL_THREE 540
#define RESOLUTION_LEVEL_FOUR 720
#define RESOLUTION_LEVEL_FIVE 768
#define RESOLUTION_LEVEL_SIX 1080
#define RESOLUTION_LEVEL_SEVEN 2160

namespace talk_base {
   class Thread;
}

typedef enum{
	UP,
	DOWN,
	MAINTAIN
}RateControlStatus;

class RateControl
	: public talk_base::MessageHandler
{
public:
	RateControl();
	~RateControl();
   void setBufferState(BufferState *bs);
   void setEncoderInfo(EncoderInfo *ei);
   void start();
   void stop();
private:
   /*
    * initial
    */
   bool RateControlInit();
   /*
    * collect history buffer states
    */
   bool CollecHisInfo();
   /*
    * rate control loop
    */
   bool RateControlLoop();
   /*
    * rate control logic
    */
   int BufferBasedRc();
   /*
    * set the feedback bitrate for encoder
    */
   bool setFeedbackRate(int step, int resolution);
   /*
    * common functions
    */
   int index2rate(int index, int resolution);
   int getMaxIndex(int resolution);
   long rate2index(int rate, int resolution);
	virtual void OnMessage(talk_base::Message* msg);
private:
   enum SELF_MSG{
      SELF_MSG_START = 0,
      SELF_MSG_STOP,
      SELF_MSG_RC,
   };
   talk_base::Thread   *mThreadRc;
   std::vector<int> mBirtate_Table_360;
   std::vector<int> mBirtate_Table_480;
   std::vector<int> mBirtate_Table_540;
   std::vector<int> mBirtate_Table_720;
   std::vector<int> mBirtate_Table_768;
   std::vector<int> mBirtate_Table_1080;
   std::vector<int> mBirtate_Table_2160;

   int              mresolution;
   std::list<uint32_t> mQueueTimeLen;
   std::list<int> mQueueNumLen;
   std::list<int> mQueueDatasize;

   RateControlStatus mLast_rateswitch_status;
   int mcurrent_encoder_bitrate;
   int mLast_bitrate;
   
   BufferState *bs_handler;
   EncoderInfo *ei_handler;
	
   std::string mLogTag;
};

#endif
