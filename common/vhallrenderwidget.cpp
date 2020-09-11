#include "vhallrenderwidget.h"
#include "ui_vhallrenderwidget.h"

VhallRenderWidget::VhallRenderWidget(QWidget *parent) :
   QWidget(parent),
   ui(new Ui::VhallRenderWidget) {
   winId();
   ui->setupUi(this);
   mWidget = NULL;
}
VhallRenderWidget::~VhallRenderWidget() {
    delete ui;
}
void VhallRenderWidget::AddWidget(QWidget *w) {
   mWidget = w;
   ui->layout->addWidget(w);
}
QWidget *VhallRenderWidget::GetWidget() {
   return mWidget;
}

