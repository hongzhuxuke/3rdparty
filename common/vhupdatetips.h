#ifndef VHUPDATETIPS_H
#define VHUPDATETIPS_H

#include <QDialog>
#include "cbasedlg.h"

namespace Ui {
class VHUpdateTips;
}
//class TitleButton;
class VHUpdateTips : public CBaseDlg 
{
    Q_OBJECT

public:
    explicit VHUpdateTips(QWidget *parent = 0, bool bForceUpdata = false);
    ~VHUpdateTips();
    void SetVersion(QString version);
    void SetTip(QString str);
protected:
    //void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *, QEvent *);
	virtual void contextMenuEvent(QContextMenuEvent *);
	void paintEvent(QPaintEvent *);
private:
    Ui::VHUpdateTips *ui;
	 bool m_bForceUpdate = false;
    //QPixmap pixmap;
    QPoint pressPoint;
    QPoint startPoint;
	 //QPushButton *m_pCloseButton = NULL;
	 QPixmap pixmap;
};

#endif // VHUPDATETIPS_H
