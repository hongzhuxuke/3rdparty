#ifndef VHNETWORKTIPS_H
#define VHNETWORKTIPS_H

#include <QDialog>
#include "cbasedlg.h"

namespace Ui {
class VHNetworkTips;
}
class QPaintEvent;
class VHNetworkTips : public CBaseDlg
{
    Q_OBJECT

public:
    explicit VHNetworkTips(QWidget *parent = 0);
    ~VHNetworkTips();
    bool ProxyConfigure();
protected:
   void paintEvent(QPaintEvent *);
private slots:
   void on_btn_config_clicked();
private:
    Ui::VHNetworkTips *ui;
    bool bProsyConfigure = false;
};

#endif // VHNETWORKTIPS_H
