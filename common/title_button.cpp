
#include <QPainter>
#include <QMouseEvent>
#include "title_button.h"

TitleButton::TitleButton(QWidget *parent)
   : QPushButton(parent)
{
   mStatus = NORMAL;
   mMousePressed = false;
}

TitleButton::~TitleButton()
{

}

void TitleButton::loadPixmap(QString pic_name)
{
   mPixmap.load(pic_name);
   mBtnWidth = mPixmap.width() / 4;
   mBtnHeight = mPixmap.height();
   setFixedSize(mBtnWidth, mBtnHeight);
}

void TitleButton::enterEvent(QEvent *)
{
   mStatus = ENTER;
   update();
}

void TitleButton::mousePressEvent(QMouseEvent *event)
{
   if(event->button() == Qt::LeftButton)
   {
      mMousePressed = true;
      mStatus = PRESS;
      update();
   }
}

void TitleButton::mouseReleaseEvent(QMouseEvent *)
{
   if (mMousePressed)
   {
      mMousePressed = false;
      mStatus = ENTER;
      update();
      emit clicked();
   }
}

void TitleButton::leaveEvent(QEvent *)
{
   mStatus = NORMAL;
   update();
}

void TitleButton::paintEvent(QPaintEvent *)
{
   QPainter painter(this);
   painter.drawPixmap(rect(), mPixmap.copy(mBtnWidth * mStatus, 0, mBtnWidth, mBtnHeight));
}
