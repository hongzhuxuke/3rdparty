#ifndef START_BUTTON_H
#define START_BUTTON_H

#include <QPushButton>
class QPainter;
class QMouseEvent;

class StartButton : public QPushButton
{
   Q_OBJECT

   public:
      explicit StartButton(QWidget *parent = 0);
       ~StartButton();
       void loadPixmap(QString startPicPath, QString stopPicPath);
       // be called ,to update the icon status(start or stop)
       void updateLiveStatus(bool liveStatus);
       bool GetLiveStatus();

   protected:
      void enterEvent(QEvent *);
      void leaveEvent(QEvent *);
      void mousePressEvent(QMouseEvent *event);
      void mouseReleaseEvent(QMouseEvent *event);
      void paintEvent(QPaintEvent *);

   private:
      enum ButtonStatus{ NORMAL, ENTER, PRESS, NOSTATUS };
      ButtonStatus mStatus;
      QPixmap mStartPixmap;
      QPixmap mStopPixmap;

      //true->processing live
      bool mIsStarting;
      int mBtnWidth;
      int mBtnHeight;
      bool mMousePressed;
    
};

#endif // START_BUTTON_H
