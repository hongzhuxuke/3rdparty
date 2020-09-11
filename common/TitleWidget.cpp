#include <QLabel>
#include <QBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include "push_button.h"
#include "TitleWidget.h"

#if _MSC_VER >= 1600  
#pragma execution_character_set("utf-8")  
#endif 

TitleWidget::TitleWidget(QString titleName, QWidget *parent)
: QWidget(parent) {
   setFocusPolicy(Qt::NoFocus);

   mVersionTitle = new QLabel(this);
   mBtnClose = new PushButton(this);

   mVersionTitle->setStyleSheet("font: 16px; color: #969696; ");
   mVersionTitle->setText(titleName);

   mBtnClose->loadPixmap(":/sysButton/close_button");
   connect(mBtnClose, SIGNAL(clicked()), this, SIGNAL(closeWidget()));

   QHBoxLayout *title_layout = new QHBoxLayout();
   title_layout->addWidget(mVersionTitle, 0, Qt::AlignVCenter);
   title_layout->addStretch();
   title_layout->addWidget(mBtnClose, 0, Qt::AlignVCenter);
   title_layout->setContentsMargins(3, 0, 3, 1);

   QVBoxLayout *main_layout = new QVBoxLayout();
   main_layout->addLayout(title_layout);
   setLayout(main_layout);
   setFixedHeight(45);

   mIsMoved = false;
}

TitleWidget::~TitleWidget() {

}

void TitleWidget::mousePressEvent(QMouseEvent *e) {
   mPressPoint = e->pos();
   mIsMoved = true;
}

void TitleWidget::mouseMoveEvent(QMouseEvent *e) {
   if ((e->buttons() == Qt::LeftButton) && mIsMoved&&m_bIsEnableMove) {
      //WTF!! parentWidget() is not a good idea. see: http://blog.csdn.net/dbzhang800/article/details/7006270
      QWidget* parent_widget = this->nativeParentWidget();
      QString objectName = parent_widget->objectName();
      QPoint parent_point = parent_widget->pos();
      parent_point.setX(parent_point.x() + e->x() - mPressPoint.x());
      parent_point.setY(parent_point.y() + e->y() - mPressPoint.y());
      parent_widget->move(parent_point);
   }
}

void TitleWidget::mouseReleaseEvent(QMouseEvent *) {
   if (mIsMoved) {
      mIsMoved = false;
   }
}

void TitleWidget::hideCloseBtn(){
   mBtnClose->hide();
}
void TitleWidget::SetMoveEnable(bool bEnable){
   m_bIsEnableMove = bEnable;
}

void TitleWidget::SetTitle(QString titleName)
{
   mVersionTitle->setText(titleName);
}
void TitleWidget::paintEvent(QPaintEvent *) {
   QPainter painter(this);
   painter.setRenderHint(QPainter::Antialiasing, true);
   painter.setPen(QPen(QColor(30, 30, 30), 1));
   painter.setBrush(QColor(43, 44, 46));
   painter.drawRoundedRect(rect(), 4.0, 4.0);
}
