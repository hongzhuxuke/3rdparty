#include "vhallexitwidget.h"
#include "ui_vhallexitwidget.h"
#include <QPainter>
#include "./baseclass/DebugTrace.h"
#include "./baseclass/PluginBase.h"
#include "DebugTrace.h"

VhallExitWidget::VhallExitWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VhallExitWidget)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint |
    Qt::WindowMinimizeButtonHint |
    Qt::WindowStaysOnTopHint|
    Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);

    this->mBackgroundPixmap.load(":/sysButton/img/sysButton/widget_exit.png");
    this->resize(this->mBackgroundPixmap.size());
    this->installEventFilter(this);
}

VhallExitWidget::~VhallExitWidget() {
    delete ui;
}
void VhallExitWidget::paintEvent(QPaintEvent *e) {
   if(!this->mBackgroundPixmap.isNull()) {
      QPainter painter(this);
      painter.drawPixmap(rect(),this->mBackgroundPixmap);
   }
}
bool VhallExitWidget::eventFilter(QObject *o, QEvent *e) {
   if(o == this) {
      switch(e->type()) {
         case QEvent::MouseButtonRelease:
            emit this->clicked();
            this->close();
         case QEvent::Enter:
            this->mBackgroundPixmap.load(":/sysButton/img/sysButton/widget_exit_high.png");
            this->repaint();
            break;
         case QEvent::MouseButtonPress:
         case QEvent::Leave:
            this->mBackgroundPixmap.load(":/sysButton/img/sysButton/widget_exit.png");
            this->repaint();
            break;
         default:
            break;
      }
   }
   
   return QWidget::eventFilter(o,e);
}

