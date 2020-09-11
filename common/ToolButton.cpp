#include <QObject>
#include <QPainter>
#include <QMouseEvent>
#include "ToolButton.h"
#include <QFile>
#include <QToolTip>
#include <QDebug>

ToolButton::ToolButton(QString pic_name, QWidget *parent)
:QToolButton(parent) {
   //设置文本颜色
   QPalette text_palette = palette();
   text_palette.setColor(QPalette::ButtonText, QColor(230, 230, 230));
   setPalette(text_palette);

   //设置文本粗体
   QFont &text_font = const_cast<QFont &>(font());
   text_font.setWeight(QFont::Bold);
   setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

   //设置图标
   if (mPixmap.load(pic_name))
   {
	   mBtnWidth = mPixmap.width() / 3;
	   mBtnHeight = mPixmap.height();
   }

   setIcon(mPixmap);
   setIconSize(mPixmap.size());
   setFixedSize(mBtnWidth, mBtnHeight);
   setAutoRaise(true);
   setStyleSheet("background:transparent;");
   setAutoFillBackground(false);
   setAttribute(Qt::WA_TranslucentBackground, true);
   mStatus = NORMAL;
   mMousePressed = false;
   
   this->setFocusPolicy(Qt::NoFocus);
}

ToolButton::ToolButton(QWidget *parent)
:QToolButton(parent) {
   //设置文本颜色
   QPalette text_palette = palette();
   text_palette.setColor(QPalette::ButtonText, QColor(230, 230, 230));
   setPalette(text_palette);

   //设置文本粗体
   QFont &text_font = const_cast<QFont &>(font());
   text_font.setWeight(QFont::Bold);
   setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

   setAutoRaise(true);
   setStyleSheet("background:transparent;");

   mStatus = NORMAL;
   mMousePressed = false;
   this->setFocusPolicy(Qt::NoFocus);
}

ToolButton::~ToolButton() {

}

void ToolButton::enterEvent(QEvent *) {
   mStatus = ENTER;
   emit sigEnter();
   update();
}

void ToolButton::leaveEvent(QEvent *) {
   mStatus = NORMAL;
   emit sigLeave();
   update();
}
void ToolButton::setShowNormal(bool ok) {
   if (ok) {
      mStatus = NORMAL;
   } else {
      mStatus = ENTER;
   }
   update();
   this->repaint();

}
void ToolButton::mousePressEvent(QMouseEvent *event) {
   if(!this->isEnabled()) {
      return ;
   }
   if (event->button() == Qt::LeftButton) {      
      emit sigPressed();
      mMousePressed = true;
      mStatus = PRESS;
      update();
   } else {
   }
}

void ToolButton::mouseReleaseEvent(QMouseEvent *) {
   if(!this->isEnabled()) {
      return ;
   }
   
   if (mMousePressed) {
      mMousePressed = false;
      mStatus = ENTER;
      update();
      this->repaint();
      emit sigClicked();
   }
}

void ToolButton::paintEvent(QPaintEvent *) {
   QPainter painter(this);
   if(this->isEnabled()) {
      painter.drawPixmap(rect(), mPixmap.copy(mBtnWidth * mStatus, 0, mBtnWidth, mBtnHeight));
   }
   else {
	   QSize size = mDisabledPixmap.size();
	   if (size.rwidth()==0 || size.rheight()==0 || size.rwidth()>500 || size.rheight()>500)
	   {
		   painter.drawPixmap(rect(), mPixmap.copy(mBtnWidth * PRESS, 0, mBtnWidth, mBtnHeight));
	   }
	   else
	   {
		   painter.drawPixmap(rect(), mDisabledPixmap);
	   }
   }
}

void ToolButton::changeImage(QString pic_name) {
   if (mPixmap.load(pic_name)) {
      mBtnWidth = mPixmap.width() / 3;
      mBtnHeight = mPixmap.height();
      setFixedSize(mBtnWidth, mBtnHeight);
      update();
   }
}
void ToolButton::showTips(QPoint p,QString str,bool bShow) {
   this->mToolTip=str;
   if(bShow) {
      QToolTip::showText(p,str,this,this->rect());
   }
}
void ToolButton::setDisabledPixmap(QString name) {
   mDisabledPixmap = QPixmap(name);
}

