#ifndef VHALLACTIVITYMODECHOICE_H
#define VHALLACTIVITYMODECHOICE_H

#include <QDialog>
#include <QPixmap>
#include <QPainter>
#include "cbasedlg.h"
#include "title_button.h"
namespace Ui {
	class VhallActivityModeChoiceDlg;
}

class VhallActivityModeChoiceDlg : public CBaseDlg
{
    Q_OBJECT

public:
    explicit VhallActivityModeChoiceDlg(QWidget *parent = 0);
    ~VhallActivityModeChoiceDlg();
    bool GetIsInterActive() {return m_bIsInterActive;};
    void CenterWindow(QWidget* parent);
protected:    
    virtual bool eventFilter(QObject *o, QEvent *e);
    virtual void paintEvent(QPaintEvent *) ;

	 void mousePressEvent(QMouseEvent * event);
	 void mouseMoveEvent(QMouseEvent * event);
	 void mouseReleaseEvent(QMouseEvent * event);

private:
	Ui::VhallActivityModeChoiceDlg *ui;
    bool m_bIsInterActive = false;
    QPoint pressPoint;
    QPoint startPoint;
    QPixmap pixmap;
    TitleButton *m_pBtnClose = NULL;
	 bool m_bMve;
};

#endif // VHALLACTIVITYMODECHOICE_H
