#include "VhallViewWidget.h"
#include <QPainter>
#include <QDebug>
#include <QFileInfo>
#include <QImageReader>
#include <QSettings>

bool YUV420TOARGB32( unsigned char* pARGB32, int iWidth, int iHeight,unsigned char **YUVData)
{
   unsigned char *BY=YUVData[0];
   unsigned char *UV=YUVData[1];
   int rgb[4];
   int  x, y;
   int YIndex=0;
   int UVIndex=0;
   for(y = 0; y < iHeight; y++)
   {
      for(x=0; x < iWidth; x++,YIndex++)
      {
         UVIndex=y/2*iWidth/2+x/2;
         unsigned char tuData = UV[UVIndex * 2];
         unsigned char tvData = UV[UVIndex * 2 + 1];

         rgb[3] = 0xFF;
         rgb[2] = int(BY[YIndex] + 1.370705 * (tvData - 128)); // r分量值
         rgb[1] = int(BY[YIndex] - 0.698001 * (tuData - 128)  - 0.703125 * (tvData - 128)); // g分量值
         rgb[0] = int(BY[YIndex] + 1.732446 * (tuData - 128)); // b分量值

         for(int j=0; j<4; j++)
         {
            int index=y*iWidth*4+x*4;
            if(rgb[j]>=0 && rgb[j]<=255)
            pARGB32[index + j] = rgb[j];
            else
            pARGB32[index + j] = (rgb[j] < 0) ? 0 : 255;
         }
      }
   }

   return true;
}
QImage CreateQImageFromARGB32(unsigned char *data,int width,int height)
{
    QImage img=QImage(width,height,QImage::Format_ARGB32);
    for(int y=0;y<height;y++)
    {
        for(int x=0;x<width;x++)
        {
#define STEP 4
            unsigned int pixel=0xFF000000;
            unsigned char b=data[y*width*STEP+x*STEP+0];
            unsigned char g=data[y*width*STEP+x*STEP+1];
            unsigned char r=data[y*width*STEP+x*STEP+2];
            unsigned char a=data[y*width*STEP+x*STEP+3];
            pixel|=a<<24;
            pixel|=r<<16;
            pixel|=g<<8;
            pixel|=b;
            img.setPixel(x,y,pixel);
        }
    }
    return img;
}

VHallViewWidget::VHallViewWidget(QWidget *parent):
   QWidget(parent)
{
   m_bIsSetMaxRenderRect=false;
   this->setAutoFillBackground(true);
   m_DrawMode=VIEWWIDGETPOS_CU;
   setMouseTracking(true);
   m_mouseIsDown=false;
   rectX=0;
   rectY=0;
   rectW=0;
   rectH=0;
   m_rectStatus=VHALLVIEWRECTSTATUS_OUT;

   QSettings reg("HKEY_CURRENT_USER\\Control Panel\\Desktop",QSettings::NativeFormat);
   float zoom = reg.value("LogPixels").toString().toInt();
   zoom/=96.0f;
   m_zoom = 1.0f/zoom;

   if(m_zoom>1.0f) {
      m_zoom=1.0f;
   }
}
   
void VHallViewWidget::SetViewMode(unsigned char VHALLVIEWWIDGETVIEWMODE viewMode)
{
   m_viewMode=viewMode;
}
VHallViewWidget::~VHallViewWidget()
{

}
void VHallViewWidget::SetBasePixmap(const QPixmap &basePixmap)
{
   int w=this->width();
   int h;
   float fh=basePixmap.height();
   m_fBaseScale=this->width();
   m_fBaseScale/=basePixmap.width();
   h=fh*m_fBaseScale;
   this->m_qBasePixmap=basePixmap.scaled(w,h);
   this->setMinimumSize(this->width(),(int)h);
   this->repaint();
}
void VHallViewWidget::SetCerterPixmap(int x,int y,int w,int h,int imgWidget,int imgHeight,unsigned char **buf,int baseWidth,int baseHeight)
{
   unsigned char *dataBuf=new unsigned char[imgWidget*imgHeight*4];
   YUV420TOARGB32(dataBuf,imgWidget,imgHeight,buf);
   QImage img=CreateQImageFromARGB32(dataBuf,imgWidget,imgHeight);
   delete dataBuf;
   dataBuf=NULL;

   m_cernterX=x*m_fBaseScale;
   m_cernterY=y*m_fBaseScale;
   m_cernterW=w*m_fBaseScale;
   m_cernterH=h*m_fBaseScale;

   m_qCenterPixmap=QPixmap::fromImage(img.scaled(m_cernterW,m_cernterH));
   m_qRenderPixmap=QPixmap(m_cernterW,m_cernterH);
   m_qRenderPixmap.fill(Qt::transparent);



   m_iBaseH=baseHeight;
   m_iBaseW=baseWidth;
}

void VHallViewWidget::SetMaxRenderRect(QRect rect)
{
   this->m_qRenderRect=rect;
   this->m_qBasePixmap=QPixmap(rect.width(),rect.height());
   this->setMinimumSize(rect.width(),rect.height());
   m_bIsSetMaxRenderRect=true;
}
QPixmap VHallViewWidget::CreateTextPIC(float&x,float&y,float&w,float&h)
{   
   if(m_text.length()==0)
   {
      return QPixmap();
   }
   x=m_textFx;
   y=m_textFy;
   w=m_textFw;
   h=m_textFh;
   
   int pw;
   QPixmap textPixmap=CreateTextPixmap(200,m_text,m_qTextCharFmt,pw);
   return textPixmap;
}
QPixmap VHallViewWidget::CreateTextPixmap(int pointSize,QString str,QTextCharFormat fmt,int &textW)
{
   QFont f=fmt.font();  
   f.setPointSize(pointSize * 0.8 * m_zoom);
   f.setUnderline(fmt.fontUnderline());
   f.setLetterSpacing(QFont::AbsoluteSpacing,0);

   QFontMetrics fm=QFontMetrics(f);
   textW=fm.width(str)+pointSize/4;
   
   int textH=pointSize;
   QPixmap textPixmap=QPixmap(textW,textH*1.25);
   textPixmap.fill(QColor(0,0,0,127));
   QPainter pText(&textPixmap);
   pText.setFont(f);

   QPen pen;
   pen.setColor(fmt.foreground().color());
   pText.setPen(pen);
      
   pText.drawText(0,textH*0.9,str);

   int lineWidth=textH/10>5?5:textH/10;
   pen.setWidth(lineWidth);
   pen.setColor(QColor(0x28,0x28,0x28,0xFF));
   pText.setPen(pen);
   pText.drawRect(1,lineWidth/2,textW-lineWidth-1,textH*1.25-lineWidth*1.5);

   return textPixmap;

}

void VHallViewWidget::mouseMoveEvent(QMouseEvent *e)
{
   if(m_mouseIsDown)
   {
      float x=m_pressX;
      float y=m_pressY;
      float w=m_pressW;
      float h=m_pressH;
      if(h<=0||w<=0)
      {
         return ;
      }
      
      float k=w/h;
   
      int xDiff=e->pos().x()-m_pressPoint.x();
      int yDiff=e->pos().y()-m_pressPoint.y();

      float xfDiff=xDiff;
      xfDiff/=m_cernterW;
      float yfDiff=yDiff;
      yfDiff/=m_cernterH;
      float kfDiff;
      if(yfDiff==0)
      {
         kfDiff=k;
         yfDiff=xfDiff/k;
      }
      else
      {
         kfDiff=xfDiff/yfDiff;
      }
   
      float pressX=m_pressPoint.x();
      float pressY=m_pressPoint.y()-m_cernterY;
      pressX/=m_cernterW;
      pressY/=m_cernterH;

      switch(m_rectStatus)
      {
         case VHALLVIEWRECTSTATUS_IN:
            if(VHALLVIEWWIDGETVIEWMODE_PIC==m_viewMode)
            {
               x+=xfDiff;
               y+=yfDiff;
               if(x<0)
               {
                  x=0;
               }
               if(x>0.95)
               {
                  x=0.95;
               }
               if(y<0)
               {
                  y=0;
               }
               if(y>0.95)
               {
                  y=0.95;
               }

            }
            else if(VHALLVIEWWIDGETVIEWMODE_TEXT==m_viewMode)
            {
               x+=xfDiff;
               y+=yfDiff;
               if(x<0)
               {
                  x=0;
               }
               if(x>0.95)
               {
                  x=0.95;
               }
               if(y<0)
               {
                  y=0;
               }
               if(y>0.95)
               {
                  y=0.95;
               }
            }
            break;
         case VHALLVIEWRECTSTATUS_OUT:

            break;
         case VHALLVIEWRECTSTATUS_LU:
            if(x+xfDiff<=0)
            {
               xfDiff=-x;
            }
            
            if(xfDiff>w-VIEW_DEFAULT_WIDTH)
            {
               xfDiff=w-VIEW_DEFAULT_WIDTH;                 
            }
            x+=xfDiff;
            w-=xfDiff;
            yfDiff=xfDiff/k;
            h-=yfDiff;
            y+=yfDiff;


            break;
         case VHALLVIEWRECTSTATUS_RU:
            if(x+w+xfDiff>1)
            {
               xfDiff=1-w-x;
            }

            if(xfDiff+w<VIEW_DEFAULT_WIDTH)
            {
               xfDiff=VIEW_DEFAULT_WIDTH-w;                 
            }

            
            w+=xfDiff;
            yfDiff=xfDiff/k;
            h+=yfDiff;
            y-=yfDiff;



            break;
         case VHALLVIEWRECTSTATUS_LD:

            if(x+xfDiff<=0)
            {
               xfDiff=-x;
            }

            if(xfDiff>w-VIEW_DEFAULT_WIDTH)
            {
               xfDiff=w-VIEW_DEFAULT_WIDTH;                 
            }

            yfDiff=xfDiff/k;
            
            x+=xfDiff;
            w-=xfDiff;
            h-=yfDiff;

            break;
         case VHALLVIEWRECTSTATUS_RD:

            if(x+w+xfDiff>1)
            {
               xfDiff=1-w-x;
            }
            
            if(xfDiff+w<VIEW_DEFAULT_WIDTH)
            {
               xfDiff=VIEW_DEFAULT_WIDTH-w;                 
            }
            yfDiff=xfDiff/k;

            w+=xfDiff;
            h+=yfDiff;
               
            break;
         default:
            break;
      }
      
      if(VHALLVIEWWIDGETVIEWMODE_PIC==m_viewMode)
      {
         DrawRenderPixmap(x,y,w,h,m_imgFilename);
      }
      else if(VHALLVIEWWIDGETVIEWMODE_TEXT==m_viewMode)
      { 
         PaintText(x,y,w,h,m_text,m_qTextCharFmt);
      }
      
      m_DrawMode=VIEWWIDGETPOS_CO;
      this->repaint();
   }
   else
   {
      unsigned char status =GetViewStatus(e->pos());
      switch (status)
      {
         case VHALLVIEWRECTSTATUS_IN:
            this->setCursor(QCursor(Qt::SizeAllCursor));
            break;
         case VHALLVIEWRECTSTATUS_OUT:
            this->setCursor(QCursor(Qt::ArrowCursor));
            break;            
         case VHALLVIEWRECTSTATUS_RD:
         case VHALLVIEWRECTSTATUS_LU:
            this->setCursor(QCursor(Qt::SizeFDiagCursor));
            break;
         case VHALLVIEWRECTSTATUS_RU:
         case VHALLVIEWRECTSTATUS_LD:
            this->setCursor(QCursor(Qt::SizeBDiagCursor));
            break;
            break;
         default:
            break;
      }
      m_rectStatus=status;

   }

   
   QWidget::mouseMoveEvent(e);
}
void VHallViewWidget::mousePressEvent(QMouseEvent *e)
{
   m_mouseIsDown=true;
   m_pressPoint=e->pos();

   if(VHALLVIEWWIDGETVIEWMODE_PIC==m_viewMode)
   {
      QString imgPos;
      GetImageInfo(m_pressX,m_pressY,m_pressW,m_pressH,m_imgFilename,imgPos);
   }
   else if(VHALLVIEWWIDGETVIEWMODE_TEXT==m_viewMode)
   {
      m_pressX=m_textFx;
      m_pressY=m_textFy;
      m_pressW=m_textFw;
      m_pressH=m_textFh;
   }
   
   QWidget::mousePressEvent(e);   
}
void VHallViewWidget::mouseReleaseEvent(QMouseEvent *e)
{
   m_mouseIsDown=false;
   QWidget::mouseReleaseEvent(e);
}
unsigned char VHALLVIEWRECTSTATUS VHallViewWidget::GetViewStatus(QPoint mousePoint)
{
   
   int mouseX=mousePoint.x();
   int mouseY=mousePoint.y();
  
   mouseY-=m_cernterY;
   
   if(rectX<=mouseX&&mouseX<=rectW+rectX&&rectY<=mouseY&&mouseY<=rectY+rectH)
   {
      int edge=10;
      if(rectW<=22||rectH<=22)
      {
         edge=rectW/2;
         if(edge>rectH/2)
         {
            edge=rectH/2;
         }
      }
      
      mouseX-=rectX;
      mouseY-=rectY;

      if(mouseX<=edge&&mouseY<=edge)
      {
         return VHALLVIEWRECTSTATUS_LU;
      }
      else if(mouseX<=edge&&mouseY>rectH-edge)
      {
         return VHALLVIEWRECTSTATUS_LD;
      }
      else if(mouseX>rectW-edge&&mouseY<edge)
      {
         return VHALLVIEWRECTSTATUS_RU;
      }
      else if(mouseX>rectW-edge&&mouseY>rectH-edge)
      {
         return VHALLVIEWRECTSTATUS_RD;
      }

      return VHALLVIEWRECTSTATUS_IN;
   }
   return VHALLVIEWRECTSTATUS_OUT;
}

void VHallViewWidget::DrawRectPixmap(int x,int y,int w,int h)
{
   m_qRectPixmap=QPixmap(w,h);
   m_qRectPixmap.fill(Qt::transparent);
   QPainter p(&m_qRectPixmap);
   p.setPen(Qt::red);
   p.drawRect(0,0,w-1,h-1);

   if(w>22&&h>22)
   {
      p.drawRect(0   ,0   ,10,10);
      p.drawRect(w-11,0   ,10,10);
      p.drawRect(w-11,h-11,10,10);
      p.drawRect(0   ,h-11,10,10);
   }

   rectX=x;
   rectY=y;
   rectW=w;
   rectH=h;
   
}

void VHallViewWidget::DrawRenderPixmap(float fx,float fy,float fw,float fh,QString filename)
{
   int x=fx*m_cernterW;
   int y=fy*m_cernterH;
   int w=fw*m_cernterW;
   int h=fh*m_cernterH;
   Clear();

   QPixmap filePixmap;
   filePixmap.load(filename);

   if(!filePixmap.isNull())
   {
      if(w==0)
      {
         w=100;
      }

      if (fh ==1.0f) {
         float scale = filePixmap.height();
         scale /= filePixmap.width();
         w = h/scale;
      }
      else {
         float scale = filePixmap.height();
         scale /= filePixmap.width();
         h = w*scale;
      }

      filePixmap=filePixmap.scaled(w,h);

      DrawRectPixmap(x,y,w,h);

      
      m_qRenderMutex.lock();
      QPainter p(&m_qRenderPixmap);
      p.drawPixmap(x,y,w,h,filePixmap);
      
      p.drawPixmap(x,y,w,h,m_qRectPixmap);
      
      m_qRenderMutex.unlock();
      
      m_imgFilename=filename;
      
      m_imgX=x;
      m_imgY=y;
      m_imgW=w;
      m_imgH=h;


      m_imgX/=m_cernterW;
      m_imgY/=m_cernterH;
      m_imgW/=m_cernterW;
      m_imgH/=m_cernterH;
   }

}
void VHallViewWidget::PaintImage(float fx,float fy,float fw,float fh,QString filename)
{
   
   float centerK=m_cernterW;
   if(!m_cernterH)
   {
      return ;
   }

   centerK/=m_cernterH;

   //自定义
   if(VIEWWIDGETPOS_CO==m_DrawMode)
   {
      DrawRenderPixmap(fx,fy,fw,fh,filename);
   }
   //全屏
   else if(VIEWWIDGETPOS_FU==m_DrawMode)
   {
      QPixmap filePixmap;
      filePixmap.load(filename);
      if(filePixmap.isNull())
      {
         return ;
      }
      float centerK= m_cernterW;
      if(!m_cernterH)
      {
         return ;
      }
      centerK/=m_cernterH;

      float pixmapK=filePixmap.width();
      if(!filePixmap.height())
      {
         return ;
      }
      float pixmapH = filePixmap.height();
      pixmapK /= pixmapH;
      if(pixmapK==centerK)
      {
         DrawRenderPixmap(0,0,1,1,filename);
      }
      else if(pixmapK>centerK)
      {
         
         float w=1.0f;
         float h=1.0f/pixmapK*centerK;
         float x=0;
         float y=(1.0f-h)/2.0f;
         DrawRenderPixmap(x,y,w,h,filename);
      }
      else
      {
         float w=1.0f;
         float h=1.0f;
         w=h*pixmapK/centerK;
         DrawRenderPixmap((1.0f-w)/2.0f,0,w,h,filename);
      }

   }
   //预设位置
   else
   {
      float x;
      float y;
      float w;
      float h;
      float pixmapK;
      
      QPixmap filePixmap;
      filePixmap.load(filename);
      if(filePixmap.isNull())
      {
         return ;
      }
      
      pixmapK=filePixmap.width();
      pixmapK/=filePixmap.height();


      w=VIEW_DEFAULT_WIDTH;
      h=w/pixmapK*centerK;

      if (h>1.0f - 2 * VIEW_DEFAULT_EDGE_HEIGHT) {
         h = 1.0f - 2 * VIEW_DEFAULT_EDGE_HEIGHT;
         w = h*pixmapK / centerK;
      }
      if (w<0.01339f) {
         h = 1.0f - 2 * VIEW_DEFAULT_EDGE_HEIGHT;
         w = h*pixmapK / centerK;
      }
      if (h < 0.01339f) {
         w = 1.0f - 2 * VIEW_DEFAULT_EDGE_WEIGHT;
         h = w / pixmapK*centerK;
      }
      
      //左上角
      if(VIEWWIDGETPOS_LU==m_DrawMode)
      {
         x=VIEW_DEFAULT_EDGE_WEIGHT;
         y=VIEW_DEFAULT_EDGE_HEIGHT;
      }
      //左下角
      else if(VIEWWIDGETPOS_LD==m_DrawMode)
      {
         x=VIEW_DEFAULT_EDGE_WEIGHT;
         y=1-VIEW_DEFAULT_EDGE_HEIGHT-h;
      }
      //中上位置
      else if(VIEWWIDGETPOS_CU==m_DrawMode)
      {
         x=(1-w)/2;
         y=VIEW_DEFAULT_EDGE_HEIGHT;
      }
      //中下位置
      else if(VIEWWIDGETPOS_CD==m_DrawMode)
      {
         x=(1-w)/2;
         y=1-VIEW_DEFAULT_EDGE_HEIGHT-h;
      }
      //右上角
      else if(VIEWWIDGETPOS_RU==m_DrawMode)
      {
         x=1-VIEW_DEFAULT_EDGE_WEIGHT-w;
         y=VIEW_DEFAULT_EDGE_HEIGHT;

      }
      //右下角
      else if(VIEWWIDGETPOS_RD==m_DrawMode)
      {
         x=1-VIEW_DEFAULT_EDGE_WEIGHT-w;
         y=1-VIEW_DEFAULT_EDGE_HEIGHT-h;
      }
      
      DrawRenderPixmap(x,y,w,h,filename);
   }
   m_imgFilename=filename;
   this->repaint();
}

void VHallViewWidget::GetImageInfo(float& x,float&y,float&w,float&h,QString&filename,QString &imagePos)
{
   filename=m_imgFilename;
   x=m_imgX;
   y=m_imgY;
   w=m_imgW;
   h=m_imgH;
   imagePos=m_DrawMode;
}
void VHallViewWidget::SetImagePos(float x,float y)
{
   PaintImage(x,y,m_imgW,m_imgW,m_imgFilename);
}
void VHallViewWidget::ResetImageFile(QString filename)
{
   PaintImage(m_imgX,m_imgY,m_imgW,m_imgW,filename);
}

void VHallViewWidget::SetTextDrawMode(QString VIEWWIDGETPOS textPos)
{
   PaintText(textPos,m_textFw,m_textFh,m_text,m_qTextCharFmt);   
}
void VHallViewWidget::SetImageDrawMode(QString VIEWWIDGETPOS imagePos)
{
   m_DrawMode=imagePos;
   PaintImage(0,0,0,0,m_imgFilename);
}

int GetTextWidth(int h,QString str,QTextCharFormat fmt)
{
   int w=0;
   QFont f=fmt.font();  
   f.setPointSize(h);
   f.setUnderline(fmt.fontUnderline());
   f.setLetterSpacing(QFont::AbsoluteSpacing,0);
   QFontMetrics fm=QFontMetrics(f);
   w=fm.width(str)+h/4;
   return w;
}
void VHallViewWidget::PaintText(QString VIEWWIDGETPOS textPos,float fw,float fh,QString str,QTextCharFormat fmt)
{
   m_DrawMode=textPos;

   QSize renderSize=m_qRenderPixmap.size();
   int renderWidth=renderSize.width()*(1-VIEW_DEFAULT_EDGE_WEIGHT*2);
   int renderHeight=renderSize.height()*(1-VIEW_DEFAULT_EDGE_HEIGHT*2);
   int textHeight=renderHeight*VIEW_DEFAULT_TEXTHEIGHT;
   int textWidth=renderWidth+1;
   
   //全屏
   if(VIEWWIDGETPOS_FU==textPos)
   {
      textHeight=renderHeight;
      do
      {
         textHeight*=0.95;
         textWidth=GetTextWidth(textHeight,str,fmt);
      }
      while(textWidth>renderWidth);


      float fw=textWidth;
      float fh=textHeight*1.25;

      fw/=renderSize.width();
      fh/=renderSize.height();

      float fx=1;
      float fy=1;
      fx-=fw;
      fx/=2;

      fy-=fh;
      fy/=2;
      
      PaintText(fx,fy,fw,fh,str,fmt);
      return ;
   }
   //左上角
   else if(VIEWWIDGETPOS_LU==textPos)
   {
      float fx=VIEW_DEFAULT_EDGE_WEIGHT;
      float fy=VIEW_DEFAULT_EDGE_HEIGHT;
      float fh=VIEW_DEFAULT_TEXTHEIGHT;
      float fw=0;

      textWidth=GetTextWidth(textHeight,str,fmt);
      fw=textWidth;
      fw/=renderSize.width();
      fh*=1.25;
      PaintText(fx,fy,fw,fh,str,fmt);
   }
   //左下角
   else if(VIEWWIDGETPOS_LD==textPos)
   {
      float fx=VIEW_DEFAULT_EDGE_WEIGHT;
      float fy=0;
      float fh=VIEW_DEFAULT_TEXTHEIGHT;
      float fw=0;

      textWidth=GetTextWidth(textHeight,str,fmt);
      fw=textWidth;
      fw/=renderSize.width();
      fh*=1.25;
      
      fy=1-fh-VIEW_DEFAULT_EDGE_HEIGHT;
      PaintText(fx,fy,fw,fh,str,fmt);

   }
   //中上位置
   else if(VIEWWIDGETPOS_CU==textPos)
   {  
      do
      {
         textWidth=GetTextWidth(textHeight,str,fmt);
         if(textWidth>renderWidth)
         {
            textHeight*=0.95;
            continue;
         }
         break;
      }
      while(true);


      float fw=textWidth;
      float fh=textHeight*1.25;

      fw/=renderSize.width();
      fh/=renderSize.height();

      float fx=1;
      float fy=VIEW_DEFAULT_EDGE_HEIGHT;
      fx-=fw;
      fx/=2;
      
      PaintText(fx,fy,fw,fh,str,fmt);

   }
   //中下位置
   else if(VIEWWIDGETPOS_CD==textPos)
   {
      do
      {
         textWidth=GetTextWidth(textHeight,str,fmt);
         if(textWidth>renderWidth)
         {
            textHeight*=0.95;
            continue;
         }
         break;
      }
      while(true);


      float fw=textWidth;
      float fh=textHeight*1.25;

      fw/=renderSize.width();
      fh/=renderSize.height();

      float fx=1;
      float fy=1-VIEW_DEFAULT_EDGE_HEIGHT-fh;
      fx-=fw;
      fx/=2;
      
      PaintText(fx,fy,fw,fh,str,fmt);


   }
   //右上位置
   else if(VIEWWIDGETPOS_RU==textPos)
   {
      float fx=1-VIEW_DEFAULT_EDGE_WEIGHT;
      float fy=VIEW_DEFAULT_EDGE_HEIGHT;
      float fh=VIEW_DEFAULT_TEXTHEIGHT;
      float fw=0;

      textWidth=GetTextWidth(textHeight,str,fmt);
      fw=textWidth;
      fw/=renderSize.width();
      fh*=1.25;

      PaintText(fx,fy,fw,fh,str,fmt);
      fx=1-m_textFw-VIEW_DEFAULT_EDGE_WEIGHT;
      PaintText(fx,fy,fw,fh,str,fmt);
   }
   //右下位置
   else if(VIEWWIDGETPOS_RD==textPos)
   {
      float fx=1-VIEW_DEFAULT_EDGE_WEIGHT;
      float fy=1-VIEW_DEFAULT_EDGE_HEIGHT;
      float fh=VIEW_DEFAULT_TEXTHEIGHT;
      float fw=0;

      textWidth=GetTextWidth(textHeight,str,fmt);
      fw=textWidth;
      fw/=renderSize.width();
      fh*=1.25;
      fy-=fh;

      PaintText(fx,fy,fw,fh,str,fmt);
      fx=1-m_textFw-VIEW_DEFAULT_EDGE_WEIGHT;
      PaintText(fx,fy,fw,fh,str,fmt);
   }
}
void VHallViewWidget::PaintText(float fx,float fy,float fw,float fh,QString str,QTextCharFormat fmt)

{
   int x=fx*m_cernterW;
   int y=fy*m_cernterH;
   int w=fw*m_cernterW;
   float th=fh*m_cernterH*0.8;
   int h=th;
   if(th>h)
   {
      h++;
   }
   
   QRect rect=QRect(0,0,w,h);

   Clear();
   m_text=str;
   m_qTextCharFmt=fmt;
   if(str.length()!=0)
   {
      m_qRenderMutex.lock();
      
      int pointSize=h;
      if(pointSize==0)
      {
         pointSize=10;
      }


      QPainter p(&m_qRenderPixmap);
      m_qTextPixmap=CreateTextPixmap(pointSize,str,fmt,m_textW);
      m_textH=pointSize;
      QRect textRect=m_qTextPixmap.rect();
      p.drawPixmap(x,y,m_qTextPixmap.width(),m_qTextPixmap.height(),m_qTextPixmap);
      
      m_qRenderMutex.unlock();


      m_textFw=m_qTextPixmap.width();
      m_textFh=m_qTextPixmap.height();
  
      m_textFw/=m_cernterW;
      m_textFh/=m_cernterH;
      
      DrawRectPixmap(x,y,m_qTextPixmap.width(),m_qTextPixmap.height());

      p.drawPixmap(rectX,rectY,rectW,rectH,m_qRectPixmap);

   }
   m_textFx=fx;
   m_textFy=fy;
   

   this->repaint();
   
   emit TextRectChanged(m_textFx ,m_textFy ,m_textFw ,m_textFh);
}
void VHallViewWidget::GetTextPos(float &x,float &y)
{
   x=m_textFx;
   y=m_textFy;
}
void VHallViewWidget::SetTextPos(float x,float y)
{
   m_textFx=x;
   m_textFy=y;
}

QString VIEWWIDGETPOS VHallViewWidget::GetTextDrawMode()
{
   return m_DrawMode;
}

void VHallViewWidget::Clear()  
{
   m_qRenderPixmap.fill(Qt::transparent);
   m_qRectPixmap.fill(Qt::transparent);
   this->repaint();
}
void VHallViewWidget::resizeEvent(QResizeEvent *e)
{
   QWidget::resizeEvent(e);
}

void VHallViewWidget::paintEvent(QPaintEvent *e)
{
   if(!m_bIsSetMaxRenderRect)
   {
      return ;
   }

   m_qRenderMutex.lock();
   QPixmap paintPixmap=QPixmap(this->size());

   QPainter pPaint(&paintPixmap);
   pPaint.drawPixmap(this->m_qBasePixmap.rect(),this->m_qBasePixmap);
   pPaint.drawPixmap(m_cernterX,m_cernterY,m_cernterW+1,m_cernterH+1,this->m_qCenterPixmap);   
   pPaint.drawPixmap(m_cernterX,m_cernterY,m_cernterW,m_cernterH,this->m_qRenderPixmap);

   m_qRenderMutex.unlock();
   
   QPainter p(this);
   p.drawPixmap(this->rect(),paintPixmap);

   //p.drawRect(this->rect());

   QWidget::paintEvent(e);
}
