#include <stdio.h>
#include "VoiceChange.h"
#include "AudioBuffer.h"

#define BUFF_SIZE           6720


////output callback function
//int GetProcessedData(FIFOAudioBuffer *outputBuffer) {
//   int outBytes, outBytesSum;
//   
//   if (outputBuffer == nullptr || outbuffer == nullptr || fpOutput == nullptr) {
//      return -1;
//   }
//   outBytesSum = 0;
//   outBytes = outputBuffer->GetBufPos();
//   if (outBytes > 0) {
//      if (outBytes > maxOutSize) {
//         outBytes = outputBuffer->Output(outbuffer, maxOutSize);
//         fwrite(outbuffer, 1, maxOutSize, fpOutput);
//         return maxOutSize;
//      }
//      outBytes = outputBuffer->Output(outbuffer, outBytes);
//      fwrite(outbuffer, 1, outBytes, fpOutput);
//      return outBytes;
//   }
//   
///*
//   while (outBytes > 0) {
//      if (outBytes <= maxOutSize) {
//         outBytes = outputBuffer->Output(outbuffer, outBytes);
//         fwrite(outbuffer, 1, outBytes, fpOutput);
//      }
//      else{
//         outBytes = outputBuffer->Output(outbuffer, maxOutSize);
//         fwrite(outbuffer, 1, outBytes, fpOutput);
//      }
//      outBytesSum += outBytes;
//      outBytes = outputBuffer->GetBufPos();
//   }*/
//   return outBytesSum;
//};


int main() {
   VoiceChange instanceVoiceChange;
   AudioParam param;
   FILE *fpInput, *fpOutput;
   ErrCode ret;
   int iChunkBytes;
   int iSize;
   char *chunk, *outbuffer;


   param.nChannel = 2;
   param.format = AFMT_SHORT;
   param.sampleRate = 44100;
   param.type = VOICETYPE_USRDEFINE;
   param.pitchChange = -80;

   iSize = 0;
   ret = AUDIOPRCS_ERROR_UNKNOWN;
   //iChunkBytes = (VOICECHANGE_CHUNKSIZE_MS * param.nChannel * param.sampleRate * (param.format == AFMT_SHORT ? 2 : 4))/1000;
   iChunkBytes = BUFF_SIZE * sizeof(short);
   chunk = new char[iChunkBytes];
   outbuffer = new char[iChunkBytes * 2]; // output size is not same with input size, even not a constance, but it will never larger than input double size
    
   fpInput = fopen("test_44100_short_stereo.pcm"/* input the input file path */, "rb"); 
   //fpInput = fopen("NoneAudio.pcm"/* input the input file path */, "rb");
   if (fpInput == nullptr) {
      printf("Inputfile open failed!\n");
      system("pause");
      return -1;
   }
   fpOutput = fopen("test_44100_short_stereo_out.pcm"/* input the output file path */, "w+b");
   //fpOutput = fopen("OutputAudio.pcm"/* output the input file path */, "w+b");
   if (fpOutput == nullptr) {
      printf("Outputfile open failed!\n");
      system("pause");
      return -1;
   }

   ret = instanceVoiceChange.Init(&param);
   if (ret != AUDIOPRCS_ERROR_SUCCESS) {
      printf("Error occured! Error code is %d\n", ret);
      system("pause");
      return -1;
   }
   
   int cnt = 0;

   int rd = 0, expt = 0;
   while (true)
   {
      /*if (cnt == 50) {
         ret = instanceVoiceChange.ChangeVoiceType(VOICETYPE_CUTE);
         if (ret == AUDIOPRCS_ERROR_NOTHING_DONE) {
            printf("process nothing!\n");
         }
         else if (ret != AUDIOPRCS_ERROR_SUCCESS) {
            printf("Error occured! Error code is %d\n", ret);
            system("pause");
            return -1;
         }
         do {
            iSize = instanceVoiceChange.GetOutputSize();
            if (iSize > iChunkBytes * 2) {
               iSize = instanceVoiceChange.GetOutputData(outbuffer, iChunkBytes * 2);
               fwrite(outbuffer, 1, iChunkBytes * 2, fpOutput);
            }
            else if (iSize > 0) {
               iSize = instanceVoiceChange.GetOutputData(outbuffer, iSize);
               fwrite(outbuffer, 1, iSize, fpOutput);
            }
         } while (iSize > 0);
         
      }*/
      iSize = fread(chunk, 1, iChunkBytes, fpInput);
      if (iSize <= 0) {
         break;
      }
      
      //printf("we have read %d\n", rd);
      ret = instanceVoiceChange.Process(chunk, iSize);
      if (ret == AUDIOPRCS_ERROR_NOTHING_DONE) {
         printf("process nothing!\n");
      }
      else if (ret != AUDIOPRCS_ERROR_SUCCESS) {
         printf("Error occured! Error code is %d\n", ret);
         system("pause");
         return -1;
      }
      do {
         iSize = instanceVoiceChange.GetOutputSize();
         if (iSize > iChunkBytes * 2) {
            iSize = instanceVoiceChange.GetOutputData(outbuffer, iChunkBytes * 2);
            fwrite(outbuffer, 1, iChunkBytes * 2, fpOutput);
         }
         else if (iSize > 0) {
            iSize = instanceVoiceChange.GetOutputData(outbuffer, iSize);
            fwrite(outbuffer, 1, iSize, fpOutput);
         }
      } while (iSize > 0);
     cnt++;
       /*if (cnt == 125) {
         cnt = 125;
      }*/
   }
   printf("Start to flush!!\n");
   instanceVoiceChange.Flush();
   do {
      iSize = instanceVoiceChange.GetOutputSize();
      if (iSize > iChunkBytes * 2) {
         iSize = instanceVoiceChange.GetOutputData(outbuffer, iChunkBytes * 2);
         fwrite(outbuffer, 1, iChunkBytes * 2, fpOutput);
      }
      else if (iSize > 0) {
         iSize = instanceVoiceChange.GetOutputData(outbuffer, iSize);
         fwrite(outbuffer, 1, iSize, fpOutput);
      }
   } while (iSize > 0);   
   printf("finish!\n");

   if (chunk != nullptr) {
      delete[]chunk;
      chunk = nullptr;
   }
   if (outbuffer != nullptr) {
      delete[]outbuffer;
      outbuffer = nullptr;
   }
   fclose(fpInput);
   fclose(fpOutput);
   system("pause");
   return 0;
}