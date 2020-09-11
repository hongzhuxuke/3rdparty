#include "vhallwaiting.h"
#include "ui_vhallwaiting.h"
#include <QDebug>
VhallWaiting::VhallWaiting(QWidget *parent) :
CBaseDlg(parent),
    ui(new Ui::VhallWaiting),
	 m_iPrentEffectiveWidth(0){
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    this->setAutoFillBackground(true);

    index = 0;
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(timeout()));
}

VhallWaiting::~VhallWaiting()
{
   m_timer.stop();
   delete ui;
}
void VhallWaiting::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.drawPixmap(this->rect(),m_pixmap);
    if(index<paddingPixmaps.count()) {
        painter.drawPixmap(m_circle_x,m_circle_y,36,36,paddingPixmaps[index]);
    }
}
void VhallWaiting::SetPixmap(QString name) {
    m_pixmap = QPixmap(name);
    qDebug()<<"VhallWaiting::SetPixmap:"<<m_pixmap.size();
    this->setMinimumSize(m_pixmap.size());
    this->setMaximumSize(m_pixmap.size());
    this->resize(m_pixmap.size());
}
void VhallWaiting::timeout() {
    index ++;
    if(index>=paddingPixmaps.count()) {
        index = 0;
    }

    this->repaint();
}
void VhallWaiting::Repos() {
   int x = 0;
   int y = 0;
   QPoint point(0, 0);
   point = parentWidget()->mapToGlobal(point);
	if (0==m_iPrentEffectiveWidth)
	{
		x = point.x() + (parentWidget()->width() - width()) / 2;
	}
	else
	{
		x = point.x() + (m_iPrentEffectiveWidth - width()) / 2;
	}
   y = point.y() + (parentWidget()->height() - height()) / 2;
   move(x, y);
}

void VhallWaiting::Show() {
	this->m_timer.stop();
   this->m_timer.start(100);
   this->resize(m_pixmap.size());
   Repos();
   this->setVisible(true);
   this->raise();
   this->show();
   emit SigShowWaiting();
}
void VhallWaiting::Close(bool sendSig) {
   this->m_timer.stop();
   this->close();
   if (sendSig) {
      emit SigCloseWating();
   }
}

void VhallWaiting::Append(QString name) {
    paddingPixmaps.append(QPixmap(name));
}

void VhallWaiting::SetParentEffectiveWidth(const int& iWidth)
{
	m_iPrentEffectiveWidth = iWidth;
}
