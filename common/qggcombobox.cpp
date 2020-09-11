#include "stdafx.h"
#include "ComboBox.h"

VHComboBox::VHComboBox(QWidget *parent)
: QComboBox(parent) {
   QListWidget* pListWidget = new QListWidget(this);
   // 设置子项目代理，否则下拉框选项周围会出现虚线框
   //pListWidget->setItemDelegate(new NoFocusFrameDelegate(this));
   //ui.resolutionCbx->setEditable(true);
   pListWidget->setWindowOpacity(0.99);
   setModel(pListWidget->model());
   setView(pListWidget);
}

VHComboBox::~VHComboBox() {

}