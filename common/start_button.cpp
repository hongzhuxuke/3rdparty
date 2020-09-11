#include <QPainter>
#include <QMouseEvent>
#include "start_button.h"

#if _MSC_VER >= 1600  
#pragma execution_character_set("utf-8")  
#endif  

StartButton::StartButton(QWidget *parent)
: QPushButton(parent) {
   mStatus = NORMAL;
   mMousePressed = false;
   mIsStarting = false;
}

StartButton::~StartButton() {

}

void StartButton::loadPixmap(QString startPicPath, QString stopPicPath) {
   mStartPixmap.load(startPicPath);
   mStopPixmap.load(stopPicPath);
   mBtnWidth = mStartPixmap.width() / 3;
   mBtnHeight = mStartPixmap.height();
   setFixedSize(mBtnWidth, mBtnHeight);
}

void StartButton::updateLiveStatus(bool liveStatus) {
   mIsStarting = liveStatus;
   update();
}

bool StartButton::GetLiveStatus() {
   return mIsStarting;
}

void StartButton::enterEvent(QEvent *) {
   mStatus = ENTER;
   update();
}

void StartButton::leaveEvent(QEvent *) {
   mStatus = NORMAL;
   update();
}

void StartButton::mousePressEvent(QMouseEvent *event) {
   if (event->button() == Qt::LeftButton) {
      mMousePressed = true;
      mStatus = PRESS;
      update();
   }
}

void StartButton::mouseReleaseEvent(QMouseEvent *) {
   if (mMousePressed) {
      mMousePressed = false;
      mStatus = ENTER;
      update();
      emit clicked();
   }
}

void StartButton::paintEvent(QPaintEvent *) {
   if (!mIsStarting) {
      this->setToolTip(tr("开始直播"));
      QPainter painter(this);
      painter.drawPixmap(rect(), mStartPixmap.copy(mBtnWidth * mStatus, 0, mBtnWidth, mBtnHeight));
   } else {
      this->setToolTip(tr("暂停直播"));
      QPainter painter(this);
      painter.drawPixmap(rect(), mStopPixmap.copy(mBtnWidth * mStatus, 0, mBtnWidth, mBtnHeight));
   }
}


