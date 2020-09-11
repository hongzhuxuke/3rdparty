#ifndef TITLEBUTTON_H
#define TITLEBUTTON_H
#include <QPushButton>
class QPainter;
class QMouseEvent;
class TitleButton : public QPushButton
{
      Q_OBJECT
   public:
      explicit TitleButton(QWidget *parent = 0);
      ~TitleButton();
      void loadPixmap(QString pic_name);

   protected:
      // rewrite function for style of title button 
      void enterEvent(QEvent *);
      void leaveEvent(QEvent *);
      void mousePressEvent(QMouseEvent *event);
      void mouseReleaseEvent(QMouseEvent *event);
      void paintEvent(QPaintEvent *);

   private:
      //enum: status of title button
      enum ButtonStatus{NORMAL, ENTER, PRESS, NOSTATUS};
      ButtonStatus mStatus;
      QPixmap mPixmap;

      int mBtnWidth;
      int mBtnHeight;
      bool mMousePressed = false;
};

#endif // TITLEBUTTON_H

