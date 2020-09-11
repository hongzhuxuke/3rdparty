#include "vhalltechingtip.h"
#include "ui_vhalltechingtip.h"
VhallTechingTip::VhallTechingTip(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VhallTechingTip)
{
    ui->setupUi(this);
    this->setAutoFillBackground(false);
}

VhallTechingTip::~VhallTechingTip()
{
    delete ui;
}
void VhallTechingTip::SetTipMode(TipMode mode){
    switch(mode) {
        case Tip_Top:
        ui->label->setContentsMargins(0,15,0,0);
        break;
        case Tip_Down:
        ui->label->setContentsMargins(0,0,0,15);
        break;
    }

}

void VhallTechingTip::SetText(QString str){
    ui->label->setText(str);
}
void VhallTechingTip::paintEvent(QPaintEvent *e) {
    QPainter p(this);
    p.drawPixmap(this->rect(),bgPixmap);
}

void VhallTechingTip::LoadPixmap(QString file){
    this->bgPixmap.load(file);
    this->setMinimumSize(this->bgPixmap.size());
    this->setMaximumSize(this->bgPixmap.size());
    this->setMinimumSize(this->bgPixmap.size());
    this->setMaximumSize(this->bgPixmap.size());
    this->resize(this->bgPixmap.size());
    this->repaint();
}

