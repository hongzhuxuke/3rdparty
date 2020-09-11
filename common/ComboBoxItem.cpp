#include "stdafx.h"
#include "ComboBoxItem.h"
#include <QLabel>
#include <QHBoxLayout>
ComboBoxItem::ComboBoxItem(QWidget *parent) 
: QWidget(parent) {
   m_pImg = new QLabel(this);
   QPixmap pic(QStringLiteral(":/misc/preference"));
   m_pImg->setPixmap(pic);
   m_pImg->setFixedSize(pic.size());
   m_pText = new QLabel(this);

   m_pLayout = new QHBoxLayout(this);
   m_pLayout->addWidget(m_pText);
   m_pLayout->addStretch();
   m_pLayout->addWidget(m_pImg);

   m_pLayout->setSpacing(5);
   m_pLayout->setContentsMargins(5, 5, 5, 5);

   setLayout(m_pLayout);
}

ComboBoxItem::~ComboBoxItem() {

}

void ComboBoxItem::leaveEvent(QEvent * event) {
     update();
}
