#ifndef __IMAGE_ENHANCE_H__
#define __IMAGE_ENHANCE_H__
#include<stdint.h>

//#define IS_DEBUG

typedef enum PixelFmt {
   PixelFmt_RGB,        // actually ARGB32 0xAARRGGBB
   //PixelFmt_RGB32,
   //PixelFmt_ARGB32,

   //planar 4:2:0
   PixelFmt_I420,  //Y1Y2Y3Y4U1V1
   PixelFmt_YV12,  //Y1Y2Y3Y4V1U1

   //packed 4:2:2
   PixelFmt_YVYU,  //Y1V1Y2U1 Y3V3Y4U3
   PixelFmt_YUY2,  //Y1U1Y2V1 Y3U3Y4V3
   PixelFmt_UYVY,  //U1Y1V1Y2 U3Y3V3Y4
   PixelFmt_HDYC,
   ////��ʱ ������  
   //PixelFmt_Y41P,
   //PixelFmt_YVU9,
   //PixelFmt_MPEG2_VIDEO,
   //PixelFmt_H264,
   //PixelFmt_dvsl,
   //PixelFmte_dvsd,
   // PixelFmt_dvhd,
   //PixelFmte_MJPG,
   PixelFmt_UNKNOWN

}PixelFmt;

typedef class VideoFilterParam {
public:
   int width;
   int height;
   VideoFilterParam() {
      width = -1;
      height = -1;
   };
}VideoFilterParam;

typedef unsigned char pixel;

class ImageEnhance
{
public:

	ImageEnhance();
	~ImageEnhance();

	bool Init(VideoFilterParam * param);

   //��̬�������������������ͷ������Դ���ҽ���������������������
   //for camera input and need to be used at first
   bool Denoise(pixel *inputFrame, pixel *outputFrame, PixelFmt type);  

   //������������һ����������ͷ������
   //usually for camera input
   bool Brighter(pixel *inputFrame, pixel *outputFrame, PixelFmt type, float alpha = 0.5);

   //��Ե��ǿ��һ�����Ҫ������ʶ���ೡ��ʹ��(�����湲��)
   //for screen input
   bool EdgeEnhance(pixel *inputFrame, pixel *outputFrame, PixelFmt type, float alpha = 0.5);//for YUV420
   bool EdgeEnhance(uint32_t *inputFrame, uint32_t *outputFrame, PixelFmt type, float alpha = 0.5);//for RGB32
private:
   void Clear();

private:
   bool mIsInited;
   int mFramePixels;
   VideoFilterParam mCtx;
	pixel *mReferenceFrame;

};
#endif //__IMAGE_ENHANCE_H__