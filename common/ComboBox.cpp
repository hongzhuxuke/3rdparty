#include "stdafx.h"
#include "ComboBox.h"
#include <QListWidget>

ComboBox::ComboBox(QWidget *parent)
: QComboBox(parent) {
	mpListWidget = new QListWidget(this);
	mpListWidget->setWindowOpacity(0.99);
   setModel(mpListWidget->model());
   setView(mpListWidget);
}

ComboBox::~ComboBox() {
	if (NULL != mpListWidget)
	{
		mpListWidget->deleteLater();
		mpListWidget = NULL;
	}
}