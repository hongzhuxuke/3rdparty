#include "VhallActivityModeChoiceDlg.h"
#include "ui_vhallactivitymodechoicedlg.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

VhallActivityModeChoiceDlg::VhallActivityModeChoiceDlg(QWidget *parent) :
CBaseDlg(parent),
m_bMve(false),
ui(new Ui::VhallActivityModeChoiceDlg) {
   ui->setupUi(this);

   setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
   setAttribute(Qt::WA_TranslucentBackground);
   ui->labTitle->setAttribute(Qt::WA_TranslucentBackground, false);
   setAutoFillBackground(false);

   ui->frame_interactivity->installEventFilter(this);
   ui->frame_live->installEventFilter(this);

   pixmap = QPixmap(":/interactivity/choice");
   ui->label_live->setPixmap(QPixmap(":/interactivity/live"));
   ui->label_interactivity->setPixmap(QPixmap(":/interactivity/interactivity"));

   m_pBtnClose = new TitleButton(this);
   m_pBtnClose->loadPixmap(":/sysButton/close_button");
   m_pBtnClose->setToolTip(tr("关闭窗口"));
   connect(m_pBtnClose, SIGNAL(clicked()), this, SLOT(close()));
   ui->titlehLayout->addWidget(m_pBtnClose);
}

VhallActivityModeChoiceDlg::~VhallActivityModeChoiceDlg() {
   delete ui;
}
bool VhallActivityModeChoiceDlg::eventFilter(QObject *o, QEvent *e) {
   if (ui->frame_interactivity == o) {//互动
      if (e->type() == QEvent::Enter) {
         ui->label_interactivity->setPixmap(QPixmap(":/interactivity/interactivityHover"));
         ui->label_interactivity_tip->setStyleSheet("QFrame{border: 0px;}QLabel{color:rgb(255,255,255);}");
      } else if (e->type() == QEvent::Leave) {
         ui->label_interactivity->setPixmap(QPixmap(":/interactivity/interactivity"));
         ui->label_interactivity_tip->setStyleSheet("QFrame{border: 0px;}QLabel{color:rgb(70,70,70);}");
      } else if (e->type() == QEvent::MouseButtonRelease) {
         m_bIsInterActive = true;
         accept();
      }
   } else if (ui->frame_live == o) {//直播
      if (e->type() == QEvent::Enter) {
         ui->label_live->setPixmap(QPixmap(":/interactivity/liveHover"));
         ui->label_live_tip->setStyleSheet("QFrame{border: 0px;}QLabel{color:rgb(255,255,255);}");
      } else if (e->type() == QEvent::Leave) {
         ui->label_live->setPixmap(QPixmap(":/interactivity/live"));

         ui->label_live_tip->setStyleSheet("QFrame{border: 0px;}QLabel{color:rgb(70,70,70);}");
      } else if (e->type() == QEvent::MouseButtonRelease) {
         accept();
      }
   }

   return QWidget::eventFilter(o, e);
}
void VhallActivityModeChoiceDlg::paintEvent(QPaintEvent * e) {
   QPainter painter(this);
   painter.drawPixmap(rect(), this->pixmap);
   CBaseDlg::paintEvent(e);
}
void VhallActivityModeChoiceDlg::CenterWindow(QWidget* parent) {
   int x = 0;
   int y = 0;
   if (NULL == parent) {
      const QRect rect = QApplication::desktop()->availableGeometry();
      x = rect.left() + (rect.width() - width()) / 2;
      y = rect.top() + (rect.height() - height()) / 2;
   } else {
      QPoint point(0, 0);
      point = parent->mapToGlobal(point);
      x = point.x() + (parent->width() - width()) / 2;
      y = point.y() + (parent->height() - height()) / 2;
   }
   move(x, y);
}

void VhallActivityModeChoiceDlg::mousePressEvent(QMouseEvent * event) {
   if (this->cursor().pos().ry() - this->y() < 51) {
      m_bMve = true;
      this->pressPoint = this->cursor().pos();
      this->startPoint = this->pos();
   }
   QDialog::mousePressEvent(event);
}

void VhallActivityModeChoiceDlg::mouseMoveEvent(QMouseEvent * event) {
   if (m_bMve) {
      int dx = this->cursor().pos().x() - this->pressPoint.x();
      int dy = this->cursor().pos().y() - this->pressPoint.y();
      this->move(this->startPoint.x() + dx, this->startPoint.y() + dy);
   }
   QDialog::mouseMoveEvent(event);
}

void VhallActivityModeChoiceDlg::mouseReleaseEvent(QMouseEvent * event) {
   m_bMve = false;
   QDialog::mouseReleaseEvent(event);
}

