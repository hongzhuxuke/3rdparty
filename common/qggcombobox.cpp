#include "stdafx.h"
#include "ComboBox.h"

VHComboBox::VHComboBox(QWidget *parent)
: QComboBox(parent) {
   QListWidget* pListWidget = new QListWidget(this);
   // ��������Ŀ��������������ѡ����Χ��������߿�
   //pListWidget->setItemDelegate(new NoFocusFrameDelegate(this));
   //ui.resolutionCbx->setEditable(true);
   pListWidget->setWindowOpacity(0.99);
   setModel(pListWidget->model());
   setView(pListWidget);
}

VHComboBox::~VHComboBox() {

}