#ifndef TOOlBUTTON_H
#define TOOlBUTTON_H
#include <QToolButton>
#include <QPoint>
class QPainter;
class QMouseEvent;

class ToolButton : public QToolButton {
   Q_OBJECT
public:

   ToolButton(QString pic_name, QWidget *parent = 0);
   ToolButton(QWidget *parent = 0);
   ~ToolButton();
   void setShowNormal(bool);
   void changeImage(QString pic_name);
   void showTips(QPoint, QString str, bool bShow = true);
   void setDisabledPixmap(QString);
signals:
   void showVolumeSlider();
   void hideVolumeSlider();
   void sigClicked();
   void sigPressed();
   void sigEnter();
   void sigLeave();
protected:
   void enterEvent(QEvent *);
   void leaveEvent(QEvent *);
   void mousePressEvent(QMouseEvent *event);
   void mouseReleaseEvent(QMouseEvent *event);
   void paintEvent(QPaintEvent *);

public:
   //enum: status of title button
   enum ButtonStatus { NORMAL, ENTER, PRESS, NOSTATUS };
   ButtonStatus mStatus;
   QPixmap mPixmap;

   int mBtnWidth;
   int mBtnHeight;
   bool mMousePressed;
   QString mToolTip;
   QPixmap mDisabledPixmap;
};

#endif //TOOlBUTTON_H
