#ifndef VHALLWAITING_H
#define VHALLWAITING_H

#include <QWidget>
#include <QMovie>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QDialog>
#include <QList>
#include <QTimer>
#include "cbasedlg.h"
namespace Ui {
class VhallWaiting;
}

class VhallWaiting : public CBaseDlg
{
    Q_OBJECT

public:
    explicit VhallWaiting(QWidget *parent = 0);
    ~VhallWaiting();
    void Append(QString);
    void SetPixmap(QString);
    void Show();
    void Close(bool sendSig = true);
    void Repos();
    void SetCerclePos(int x,int y) {m_circle_x = x;m_circle_y = y;}
	 void SetParentEffectiveWidth(const int& iWidth);
public slots:
    void timeout();
protected:
    void paintEvent(QPaintEvent *);
signals:
    void SigShowWaiting();
    void SigCloseWating();

private:
    Ui::VhallWaiting *ui;
    QPixmap m_pixmap;
    QList<QPixmap> paddingPixmaps;
    int index;
    QTimer m_timer;
    int m_circle_x = 45;
    int m_circle_y = 38;
	 int m_iPrentEffectiveWidth;
};

#endif // VHALLWAITING_H
