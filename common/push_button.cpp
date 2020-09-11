#include <QPainter>
#include <QMouseEvent>
#include "push_button.h"
PushButton::PushButton(QWidget *parent)
: QPushButton(parent) {
   mStatus = NORMAL;
   mMousePressed = false;
   this->setFocusPolicy(Qt::StrongFocus);
}

PushButton::~PushButton() {

}

void PushButton::loadPixmap(QString pic_name) {
   mPixmap.load(pic_name);
   mBtnWidth = mPixmap.width() / 4;
   mBtnHeight = mPixmap.height();
   setFixedSize(mBtnWidth, mBtnHeight);
}
void PushButton::SetEnabled(bool ok) {
   if(ok) {
      mStatus = NORMAL;
   }
   else {
      mStatus = NOSTATUS;
   }
   update();
   this->repaint();
}

void PushButton::enterEvent(QEvent *) {
   if(mStatus==NOSTATUS) {
      return ;
   }
   mStatus = ENTER;
   update();
   this->repaint();
   
   emit sigEnterIn();
}

void PushButton::leaveEvent(QEvent *) {
   if(mStatus==NOSTATUS) {
      return ;
   }
   mStatus = NORMAL;
   update();
   
   this->repaint();
}

void PushButton::mousePressEvent(QMouseEvent *event) {
   if(mStatus==NOSTATUS) {
      return ;
   }
   if (event->button() == Qt::LeftButton) {
      mMousePressed = true;
      mStatus = PRESS;
      update();
   }
}

void PushButton::mouseReleaseEvent(QMouseEvent *) {
   if (mMousePressed) {
      mMousePressed = false;
      mStatus = ENTER;
      update();
      emit clicked();
   }
   emit sigClicked();
}

void PushButton::paintEvent(QPaintEvent *) {
   QPainter painter(this);
   painter.drawPixmap(rect(), mPixmap.copy(mBtnWidth * mStatus, 0, mBtnWidth, mBtnHeight));
}
