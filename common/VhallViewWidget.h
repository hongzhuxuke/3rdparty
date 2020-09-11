#ifndef _VHALLVIEWWIDGET_H_
#define _VHALLVIEWWIDGET_H_
#include <QWidget>
#include <QPixmap>
#include <QPaintEvent>
#include <QMutex>
#include <QImage>
#include <QTextCharFormat>

#include <QResizeEvent>
#include <QMouseEvent>
#define VIEWWIDGETPOS
#define VIEWWIDGETPOS_CO QString::fromWCharArray(L"自定义")
#define VIEWWIDGETPOS_LU QString::fromWCharArray(L"左上角")
#define VIEWWIDGETPOS_LD QString::fromWCharArray(L"左下角")
#define VIEWWIDGETPOS_CU QString::fromWCharArray(L"中上位置")
#define VIEWWIDGETPOS_CD QString::fromWCharArray(L"中下位置")
#define VIEWWIDGETPOS_RU QString::fromWCharArray(L"右上角")
#define VIEWWIDGETPOS_RD QString::fromWCharArray(L"右下角")
#define VIEWWIDGETPOS_FU QString::fromWCharArray(L"全屏")

const float VIEW_DEFAULT_WIDTH    =    0.1333333;
const float VIEW_DEFAULT_EDGE_HEIGHT   =    0.02;
const float VIEW_DEFAULT_EDGE_WEIGHT   =    0.0111111;
const float VIEW_DEFAULT_TEXTHEIGHT = 0.05;

#define VHALLVIEWWIDGETVIEWMODE
#define VHALLVIEWWIDGETVIEWMODE_TEXT 0
#define VHALLVIEWWIDGETVIEWMODE_PIC  1

#define VHALLVIEWRECTSTATUS
#define VHALLVIEWRECTSTATUS_IN   0
#define VHALLVIEWRECTSTATUS_OUT  1
#define VHALLVIEWRECTSTATUS_LU   2
#define VHALLVIEWRECTSTATUS_RU   3
#define VHALLVIEWRECTSTATUS_LD   4
#define VHALLVIEWRECTSTATUS_RD   5

class VHallViewWidget:public QWidget
{
   Q_OBJECT
public:
   explicit VHallViewWidget(QWidget *parent=0);
   ~VHallViewWidget();
   void SetViewMode(unsigned char VHALLVIEWWIDGETVIEWMODE viewMode);
   void SetBasePixmap(const QPixmap&);
   void SetCerterPixmap(int ,int ,int ,int ,int ,int ,unsigned char **,int,int);
   void SetMaxRenderRect(QRect);
   void PaintText(float,float,float,float,QString,QTextCharFormat);
   void PaintText(QString VIEWWIDGETPOS textPos,float,float,QString,QTextCharFormat);
   void PaintImage(float,float,float,float,QString);
   void GetTextPos(float &,float &);
   void SetTextPos(float ,float);
   QString VIEWWIDGETPOS GetTextDrawMode();
   void Clear();   
   QPixmap CreateTextPIC(float&,float&,float&,float&);
   QPixmap CreateTextPixmap(int,QString,QTextCharFormat,int &);
   void GetImageInfo(float&,float&,float&,float&,QString&,QString &);
   void SetImagePos(float,float);
   void SetImageDrawMode(QString VIEWWIDGETPOS imagePos);
   void SetTextDrawMode(QString VIEWWIDGETPOS textPos);
   void ResetImageFile(QString);
protected:
   void paintEvent(QPaintEvent *);
   void resizeEvent(QResizeEvent *);
   void DrawRenderPixmap(float x,float y,float w,float h ,QString);
   void mousePressEvent(QMouseEvent *e);
   void mouseReleaseEvent(QMouseEvent *e);
   void mouseMoveEvent(QMouseEvent *e);   
signals:
   void TextRectChanged(float ,float ,float ,float);

private:
   void DrawRectPixmap(int x,int y,int w,int h);
   unsigned char VHALLVIEWRECTSTATUS GetViewStatus(QPoint mousePoint);
   
private:   
   QTextCharFormat m_qTextCharFmt;
   QString m_text;
   
   QPixmap m_qRenderPixmap;
   QPixmap m_qBasePixmap;
   QPixmap m_qCenterPixmap;
   QPixmap m_qTextPixmap;
   QPixmap m_qRectPixmap;
   
   int rectX;
   int rectY;
   int rectW;
   int rectH;
   

   QRect m_qRenderRect;
   QMutex m_qRenderMutex;
   bool m_bIsSetMaxRenderRect;

   int m_cernterX;
   int m_cernterY;
   int m_cernterW;
   int m_cernterH;
   
   int m_renderW;
   int m_renderH;
   int m_textW;
   int m_textH;
   
   float m_textFx;
   float m_textFy;
   float m_textFw;
   float m_textFh;

   int m_baseW;
   int m_baseH;

   //助手主窗口尺寸
   int m_iBaseW;
   int m_iBaseH;
   float m_fBaseScale;

   float m_imgX;
   float m_imgY;
   float m_imgW;
   float m_imgH;
   QString m_imgFilename;

   QString VIEWWIDGETPOS m_DrawMode;   
   unsigned char VHALLVIEWWIDGETVIEWMODE m_viewMode;
   bool m_mouseIsDown;
   unsigned VHALLVIEWRECTSTATUS m_rectStatus;
   QPoint m_pressPoint;
   float m_pressX;
   float m_pressY;
   float m_pressW;
   float m_pressH;
   float m_zoom = 1.0f;
};

#endif
