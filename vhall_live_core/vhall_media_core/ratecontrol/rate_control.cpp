#include "rate_control.h"
#include "../common/vhall_log.h"
#include "talk/base/thread.h"
#include "live_define.h"

RateControl::RateControl():mLast_bitrate(0){
   mThreadRc = new talk_base::Thread();
   RateControlInit();
   mLogTag = "RateControl";
}

RateControl::~RateControl(){
   mThreadRc->Clear(this);
   VHALL_THREAD_DEL(mThreadRc);
}

void RateControl::setBufferState(BufferState *bs){
   bs_handler = bs;
}

void RateControl::setEncoderInfo(EncoderInfo *ei){
   ei_handler = ei;
}

void RateControl::start(){
   if (ei_handler) {
      mcurrent_encoder_bitrate = ei_handler->GetBitrate();
      mresolution = ei_handler->GetResolution();
   }
   if (!mThreadRc->started()) {
      mThreadRc->Start();
   }
   mThreadRc->Restart();
   mThreadRc->Post(this, SELF_MSG_RC);
}

void RateControl::stop(){
   mQueueNumLen.clear();
   mQueueTimeLen.clear();
   mQueueDatasize.clear();
   mThreadRc->Clear(this);
   mThreadRc->Stop();
}

bool RateControl::RateControlInit()
{
   mBirtate_Table_360.push_back(100);
   mBirtate_Table_360.push_back(150);
   mBirtate_Table_360.push_back(200);
   mBirtate_Table_360.push_back(250);
   mBirtate_Table_360.push_back(350);
   mBirtate_Table_360.push_back(425);
   mBirtate_Table_360.push_back(500);
   
   mBirtate_Table_480.push_back(150);
   mBirtate_Table_480.push_back(200);
   mBirtate_Table_480.push_back(300);
   mBirtate_Table_480.push_back(400);
   mBirtate_Table_480.push_back(525);
   mBirtate_Table_480.push_back(650);
   mBirtate_Table_480.push_back(850);
   
   mBirtate_Table_540.push_back(200);
   mBirtate_Table_540.push_back(300);
   mBirtate_Table_540.push_back(400);
   mBirtate_Table_540.push_back(500);
   mBirtate_Table_540.push_back(650);
   mBirtate_Table_540.push_back(850);
   mBirtate_Table_540.push_back(1100);
   
   mBirtate_Table_720.push_back(350);
   mBirtate_Table_720.push_back(500);
   mBirtate_Table_720.push_back(650);
   mBirtate_Table_720.push_back(800);
   mBirtate_Table_720.push_back(1000);
   mBirtate_Table_720.push_back(1400);
   mBirtate_Table_720.push_back(2000);
   
   mBirtate_Table_768.push_back(350);
   mBirtate_Table_768.push_back(500);
   mBirtate_Table_768.push_back(650);
   mBirtate_Table_768.push_back(800);
   mBirtate_Table_768.push_back(1100);
   mBirtate_Table_768.push_back(1500);
   mBirtate_Table_768.push_back(2200);
   
   mBirtate_Table_1080.push_back(700);
   mBirtate_Table_1080.push_back(100);
   mBirtate_Table_1080.push_back(1300);
   mBirtate_Table_1080.push_back(1500);
   mBirtate_Table_1080.push_back(2000);
   mBirtate_Table_1080.push_back(2700);
   mBirtate_Table_1080.push_back(3800);
   
   mBirtate_Table_2160.push_back(2500);
   mBirtate_Table_2160.push_back(3500);
   mBirtate_Table_2160.push_back(4800);
   mBirtate_Table_2160.push_back(6000);
   mBirtate_Table_2160.push_back(7500);
   mBirtate_Table_2160.push_back(10000);
   mBirtate_Table_2160.push_back(15000);
   
   return true;
}

void RateControl::OnMessage(talk_base::Message* msg){
   switch (msg->message_id) {
      case SELF_MSG_START:
         
         break;
      case SELF_MSG_RC:
         RateControlLoop();
         mThreadRc->PostDelayed(1000, this, SELF_MSG_RC);
         break;
      case SELF_MSG_STOP:
         
         break;
      default:
         break;
   }
   VHALL_DEL(msg->pdata);
}

bool RateControl::RateControlLoop()
{
   if (ei_handler==NULL) {
      return false;
   }
   mcurrent_encoder_bitrate = ei_handler->GetBitrate();
   mresolution = ei_handler->GetResolution();
   CollecHisInfo();
   setFeedbackRate(BufferBasedRc(), mresolution);
   return true;
}

/*
 *收集历史信息
 */
bool RateControl::CollecHisInfo()
{
   if (bs_handler==NULL) {
      return false;
   }
   uint32_t duration = bs_handler->GetQueueDataDuration();
   int datasize = bs_handler->GetQueueDataSize();
   int queuenum = bs_handler->GetQueueSize();
   //LOGI("%s duration:%d datasize:%d queuenum:%d",mLogTag.c_str(),duration,datasize,queuenum);
   mQueueNumLen.push_front(queuenum);
   mQueueTimeLen.push_front(duration);
   mQueueDatasize.push_front(datasize);
   while (mQueueNumLen.size() > MAX_HISTORY_INFO)
   {
      mQueueNumLen.pop_back();
   }
   while (mQueueTimeLen.size() > MAX_HISTORY_INFO)
   {
      mQueueTimeLen.pop_back();
   }
   while (mQueueDatasize.size() > MAX_HISTORY_INFO)
   {
      mQueueDatasize.pop_back();
   }
   return true;
}

int RateControl::BufferBasedRc()
{
   int step = 0;
   if (mQueueDatasize.size() < MAX_HISTORY_INFO || mQueueNumLen.size() < MAX_HISTORY_INFO || mQueueTimeLen.size() < MAX_HISTORY_INFO)
   {
      return 0;
   }
   
   /*时间维度，最大最小值，均值*/
   uint32_t max_dur = 0;
   uint32_t min_dur = 0; /*_UI32_MAX; modify*/
   uint32_t avr_dur = 0;
   for (std::list<uint32_t>::iterator it = mQueueTimeLen.begin(); it != mQueueTimeLen.end(); it++)
   {
      if (*it > max_dur)
      {
         max_dur = *it;
      }
      if (*it < min_dur)
      {
         min_dur = *it;
      }
      avr_dur = avr_dur + *it;
   }
   if (mQueueTimeLen.size() > 3)
   {
      avr_dur = (avr_dur - min_dur - max_dur - *(mQueueTimeLen.begin())) / (mQueueTimeLen.size() - 3);
   }
   /*历史均值与当前值，不同权重的均值*/
   avr_dur = (*(mQueueTimeLen.begin()) * 3 + avr_dur * 7) / 10;
   
   /*数据包数量维度，最大最小值，均值*/
   int max_qsize = INT_MIN;
   int min_qsize = INT_MAX;
   int avr_qsize = 0;
   for (std::list<int>::iterator it = mQueueNumLen.begin(); it != mQueueNumLen.end(); it++)
   {
      if (*it > max_qsize)
      {
         max_qsize = *it;
      }
      if (*it < min_qsize)
      {
         min_qsize = *it;
      }
      avr_qsize = avr_qsize + *it;
   }
   if (mQueueNumLen.size() > 3)
   {
      avr_qsize = (avr_qsize - min_qsize - max_qsize - *(mQueueNumLen.begin())) / (mQueueNumLen.size() - 3);
   }
   avr_qsize = (*(mQueueNumLen.begin()) * 3 + avr_qsize * 7) / 10;
   
   /*size维度，最大最小值，均值*/
   int max_dsize = INT_MIN;
   int min_dsize = INT_MAX;
   int avr_dsize = 0;
   for (std::list<int>::iterator it = mQueueDatasize.begin(); it != mQueueDatasize.end(); it++)
   {
      if (*it > max_dsize)
      {
         max_dsize = *it;
      }
      if (*it < min_dsize)
      {
         min_dsize = *it;
      }
      avr_dsize = avr_dsize + *it;
   }
   if (mQueueDatasize.size() > 3)
   {
      avr_dsize = (avr_dsize - min_qsize - max_qsize - *(mQueueDatasize.begin())) / (mQueueDatasize.size() - 3);
   }
   avr_dsize = (*(mQueueDatasize.begin()) * 3 + avr_dsize * 7) / 10;
   
   int max_q_num = bs_handler->GetMaxNum();
   uint32_t max_q_time = (uint32_t)bs_handler->GetMaxNum() / 45;
   int max_q_data = max_q_time * mcurrent_encoder_bitrate / 8;
   
   /*buffer较空，可以增大码率*/
   if ((avr_dur < max_q_time * 2 / 10 &&  *(mQueueTimeLen.begin()) < avr_dur * 12 / 10) || \
       (avr_qsize < max_q_num * 2 / 10 && *(mQueueNumLen.begin()) < avr_qsize * 12 / 10) || \
       (avr_dsize < max_q_data * 2 / 10 && *(mQueueDatasize.begin()) < avr_dsize * 12 / 10))
   {
      step = 1;
   }
   /*buffer较满，需要降低码率*/
   else if (avr_dur > max_q_time * 7 / 10  || avr_qsize > max_q_num * 7 / 10  || avr_dsize > max_q_data * 7 / 10)
   {
      step = -1;
   }
   else if (avr_dur > max_q_time * 8 / 10 || avr_qsize > max_q_num * 8 / 10 || avr_dsize > max_q_data * 8 / 10)
   {
      step = -2;
   }
   else
   {
      /*码率保持不变*/
      step = 0;
   }
   
   switch (mLast_rateswitch_status)
   {
      case UP:
         if (step > 0)
         {
            step = 0;
         }
         break;
      case DOWN:
         if (step > 0)
         {
            step = 0;
         }
         if (step < 0)
         {
            step = -1;
         }
         break;
      default:
         break;
   }
   if (step > 0)
   {
      mLast_rateswitch_status = UP;
   } else if (step < 0)
   {
      mLast_rateswitch_status = DOWN;
   } else
   {
      mLast_rateswitch_status = MAINTAIN;
   }
   return step;
}

int RateControl::getMaxIndex(int resolution)
{
   switch (resolution)
   {
      case RESOLUTION_LEVEL_ONE:
         return (int)mBirtate_Table_360.size() - 1;
      case RESOLUTION_LEVEL_TWO:
         return (int)mBirtate_Table_480.size() - 1;
      case RESOLUTION_LEVEL_THREE:
         return (int)mBirtate_Table_540.size() - 1;
      case RESOLUTION_LEVEL_FOUR:
         return (int)mBirtate_Table_720.size() - 1;
      case RESOLUTION_LEVEL_FIVE:
         return (int)mBirtate_Table_768.size() - 1;
      case RESOLUTION_LEVEL_SIX:
         return (int)mBirtate_Table_1080.size() - 1;
      case RESOLUTION_LEVEL_SEVEN:
         return (int)mBirtate_Table_2160.size() - 1;
      default:
         return -1;
   }
}

/*码率index与码率的转换*/
int RateControl::index2rate(int index, int resolution)
{
   int rate = 0;
   switch (resolution)
   {
      case RESOLUTION_LEVEL_ONE:
         if (index<0) {
            index = 0;
         }else if(index > getMaxIndex(resolution)){
            index = getMaxIndex(resolution);
         }
         rate = mBirtate_Table_360.at(index);
         break;
      case RESOLUTION_LEVEL_TWO:
         if (index<0) {
            index = 0;
         }else if(index > getMaxIndex(resolution)){
            index = getMaxIndex(resolution);
         }
         rate = mBirtate_Table_480.at(index);
         break;
      case RESOLUTION_LEVEL_THREE:
         if (index<0) {
            index = 0;
         }else if(index > getMaxIndex(resolution)){
            index = getMaxIndex(resolution);
         }
         rate = mBirtate_Table_540.at(index);
         break;
      case RESOLUTION_LEVEL_FOUR:
         if (index<0) {
            index = 0;
         }else if(index > getMaxIndex(resolution)){
            index = getMaxIndex(resolution);
         }
         rate = mBirtate_Table_720.at(index);
         break;
      case RESOLUTION_LEVEL_FIVE:
         if (index<0) {
            index = 0;
         }else if(index > getMaxIndex(resolution)){
            index = getMaxIndex(resolution);
         }
         rate = mBirtate_Table_768.at(index);
         break;
      case RESOLUTION_LEVEL_SIX:
         if (index<0) {
            index = 0;
         }else if(index > getMaxIndex(resolution)){
            index = getMaxIndex(resolution);
         }
         rate = mBirtate_Table_1080.at(index);
         break;
      case RESOLUTION_LEVEL_SEVEN:
         if (index<0) {
            index = 0;
         }else if(index > getMaxIndex(resolution)){
            index = getMaxIndex(resolution);
         }
         rate = mBirtate_Table_2160.at(index);
         break;
      default:{
         rate = 0;
      }
         break;
   }
   return rate;
}

/*码率与码率index的转换*/
long RateControl::rate2index(int rate, int resolution)
{
   switch (resolution)
   {
      case RESOLUTION_LEVEL_ONE:
         if (rate <= mBirtate_Table_360.at(0))
         {
            return 0;
         }
         if (rate >= mBirtate_Table_360.at(getMaxIndex(resolution)))
         {
            return mBirtate_Table_360.size() - 1;
         }
         for (int i = 1; i < mBirtate_Table_360.size(); i++)
         {
            if (rate >= mBirtate_Table_360.at(i - 1) && rate < mBirtate_Table_360.at(i))
            {
               return i - 1;
            }
         }
         break;
      case RESOLUTION_LEVEL_TWO:
         if (rate <= mBirtate_Table_480.at(0))
         {
            return 0;
         }
         if (rate >= mBirtate_Table_480.at(getMaxIndex(resolution)))
         {
            return mBirtate_Table_480.size() - 1;
         }
         for (int i = 1; i < mBirtate_Table_480.size(); i++)
         {
            if (rate >= mBirtate_Table_480.at(i - 1) && rate < mBirtate_Table_480.at(i))
            {
               return i - 1;
            }
         }
         break;
      case RESOLUTION_LEVEL_THREE:
         if (rate <= mBirtate_Table_540.at(0))
         {
            return 0;
         }
         if (rate >= mBirtate_Table_540.at(getMaxIndex(resolution)))
         {
            return mBirtate_Table_540.size() - 1;
         }
         for (int i = 1; i < mBirtate_Table_540.size(); i++)
         {
            if (rate >= mBirtate_Table_540.at(i - 1) && rate < mBirtate_Table_540.at(i))
            {
               return i - 1;
            }
         }
         break;
      case RESOLUTION_LEVEL_FOUR:
         if (rate <= mBirtate_Table_720.at(0))
         {
            return 0;
         }
         if (rate >= mBirtate_Table_720.at(getMaxIndex(resolution)))
         {
            return mBirtate_Table_720.size() - 1;
         }
         for (int i = 1; i < mBirtate_Table_720.size(); i++)
         {
            if (rate >= mBirtate_Table_720.at(i - 1) && rate < mBirtate_Table_720.at(i))
            {
               return i - 1;
            }
         }
         break;
      case RESOLUTION_LEVEL_FIVE:
         if (rate <= mBirtate_Table_768.at(0))
         {
            return 0;
         }
         if (rate >= mBirtate_Table_768.at(getMaxIndex(resolution)))
         {
            return mBirtate_Table_768.size() - 1;
         }
         for (int i = 1; i < mBirtate_Table_768.size(); i++)
         {
            if (rate >= mBirtate_Table_768.at(i - 1) && rate < mBirtate_Table_768.at(i))
            {
               return i - 1;
            }
         }
         break;
      case RESOLUTION_LEVEL_SIX:
         if (rate <= mBirtate_Table_1080.at(0))
         {
            return 0;
         }
         if (rate >= mBirtate_Table_1080.at(getMaxIndex(resolution)))
         {
            return mBirtate_Table_1080.size() - 1;
         }
         for (int i = 1; i < mBirtate_Table_1080.size(); i++)
         {
            if (rate >= mBirtate_Table_1080.at(i - 1) && rate < mBirtate_Table_1080.at(i))
            {
               return i - 1;
            }
         }
         break;
      case RESOLUTION_LEVEL_SEVEN:
         if (rate <= mBirtate_Table_2160.at(0))
         {
            return 0;
         }
         if (rate >= mBirtate_Table_2160.at(getMaxIndex(resolution)))
         {
            return mBirtate_Table_2160.size() - 1;
         }
         for (int i = 1; i < mBirtate_Table_2160.size(); i++)
         {
            if (rate >= mBirtate_Table_2160.at(i - 1) && rate < mBirtate_Table_2160.at(i))
            {
               return i - 1;
            }
         }
         break;
      default:
         break;
   }
   return -1;
}

bool RateControl::setFeedbackRate(int step, int resolution)
{
   if (mLast_bitrate == 0)
   {
      mLast_bitrate = mcurrent_encoder_bitrate;
      return true;
   }
   mLast_bitrate = index2rate(step + (int)rate2index(mLast_bitrate, resolution), resolution);
   //LOGI("%s rate control bitrate:%d step:%d",mLogTag.c_str(),mLast_bitrate,step);
   ei_handler->SetBitrate(mLast_bitrate);
   return true;
}
