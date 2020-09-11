#include "vhnetworktips.h"
#include "ui_vhnetworktips.h"
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QBoxLayout>
#include <QCheckBox>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QDesktopWidget>
#include "titleWidget.h"

VHNetworkTips::VHNetworkTips(QWidget *parent) :
CBaseDlg(parent),
   ui(new Ui::VHNetworkTips) {
   ui->setupUi(this);
   setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
   setAttribute(Qt::WA_TranslucentBackground);
   this->setAutoFillBackground(true);

   TitleWidget *title = new TitleWidget(QString::fromWCharArray(L"ÍøÂç´úÀí"),this);
   ui->layout_title->addWidget(title);
   connect(title,SIGNAL(closeWidget()),this,SLOT(close()));
   ui->label_network->setStyleSheet("color: rgb(153, 153, 153);font: 16px 'Î¢ÈíÑÅºÚ';"); 
   connect(ui->btn_sure,SIGNAL(clicked()),this,SLOT(close()));
   this->setWindowTitle(QString::fromWCharArray(L"ÍøÂç´úÀí"));
}

VHNetworkTips::~VHNetworkTips() {
    delete ui;
}

bool VHNetworkTips::ProxyConfigure(){
   return this->bProsyConfigure;
}
void VHNetworkTips::paintEvent(QPaintEvent *){
   QPainter painter(this);
   painter.setRenderHint(QPainter::Antialiasing, true);
   painter.setPen(QPen(QColor(54, 54, 54), 1));
   painter.setBrush(QColor(38, 38, 38));
   painter.drawRoundedRect(rect(), 4.0, 4.0);
}
void VHNetworkTips::on_btn_config_clicked(){
   this->bProsyConfigure = true;
   close();
}

