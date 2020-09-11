#include "CPaintStateBtn.h"
#include <QPainter> 

CPaintStateBtn::CPaintStateBtn(QWidget * parent)
	: QPushButton(parent)
{
	mState = eHandsUpState_No;
}

CPaintStateBtn::CPaintStateBtn(const QString & str, QWidget *parent)
	: QPushButton(str, parent)
{
	mState = eHandsUpState_No;
}

CPaintStateBtn::CPaintStateBtn(const QIcon & icon, const QString & str, QWidget *parent)
	: QPushButton(icon, str, parent)
{
	mState = eHandsUpState_No;
}

CPaintStateBtn::~CPaintStateBtn()
{
	mState = eHandsUpState_No;
}

void CPaintStateBtn::SetHandsUpState(eHandsUpState eState)
{
	mState = eState;
	update();
}

void CPaintStateBtn::paintEvent(QPaintEvent * e)
{
	QPushButton::paintEvent(e);
	if (eHandsUpState_existence == mState)
	{

		QPainter painter(this);
		painter.setPen(QPen(QColor(252, 86, 89), 4, Qt::DashLine));//
		painter.setBrush(QBrush(Qt::red, Qt::SolidPattern));//
		//painter.setBrush(Qt::SolidPattern);
		int iW = this->width();
		int iH = this->height();

		painter.drawEllipse(58, 8, 2, 2);//»­Ô² 
										 //painter.drawPoint(QPointF(58, 8));
	}
}
