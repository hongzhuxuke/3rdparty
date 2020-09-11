#ifndef VHPROCESSBAR_H
#define VHPROCESSBAR_H

#include <QWidget>

namespace Ui {
class VHProcessBar;
}

class VHProcessBar : public QWidget
{
    Q_OBJECT

public:
    explicit VHProcessBar(QWidget *parent = 0);
    ~VHProcessBar();
    void SetValue(int);
signals:
    void SigHide();  
public slots:
	void show();
	void hide();
protected:
    bool eventFilter(QObject *, QEvent *);
    void hideEvent(QHideEvent *);
	 void paintEvent(QPaintEvent *);
private slots:
	 void slotTimeOut();
private:
    Ui::VHProcessBar *ui;
    QPoint pressPoint;
    QPoint startPoint;
	 QTimer* m_pTimer = NULL;
	 int m_iIndex;
	 QPixmap pixmap;
};

#endif // VHPROCESSBAR_H
