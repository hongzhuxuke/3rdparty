#ifndef VHNETWORKPROXY_H
#define VHNETWORKPROXY_H

#include <QDialog>
#include "cbasedlg.h"

namespace Ui {
class VHNetworkProxy;
}
class QPaintEvent;
class VHNetworkProxy : public CBaseDlg
{
    Q_OBJECT

public:
   explicit VHNetworkProxy(QWidget *parent = 0);
   ~VHNetworkProxy();
   bool IsRestart() {return bRestart;}
protected:
   void paintEvent(QPaintEvent *);
private slots:
   void on_btn_testing_clicked();
   void on_comboBox_proxyEnable_currentIndexChanged(int);
   void on_btn_sure_clicked();
   void on_btn_return_clicked();
private:
   void UIProxyEnable();
   void UIProxyDisable();

private:
   Ui::VHNetworkProxy *ui;
   bool bRestart = false;
};

#endif // VHNETWORKPROXY_H
