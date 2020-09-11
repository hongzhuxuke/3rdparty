#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <QPushButton>
class QPainter;
class QMouseEvent;

class PushButton : public QPushButton {
   Q_OBJECT
public:
   explicit PushButton(QWidget *parent = 0);
   ~PushButton();
   void loadPixmap(QString pic_name);
   void SetEnabled(bool);
signals:
   void sigClicked();
   void sigEnterIn();
protected:
   void enterEvent(QEvent *);
   void leaveEvent(QEvent *);
   void mousePressEvent(QMouseEvent *event);
   void mouseReleaseEvent(QMouseEvent *event);
   void paintEvent(QPaintEvent *);
private:
   enum ButtonStatus { NORMAL, ENTER, PRESS, NOSTATUS };
   ButtonStatus mStatus;
   QPixmap mPixmap;

   int mBtnWidth;
   int mBtnHeight;
   bool mMousePressed = false;
};

#endif // PUSHBUTTON_H
