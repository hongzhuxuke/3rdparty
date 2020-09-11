#ifndef VHALLEXITWIDGET_H
#define VHALLEXITWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QPaintEvent>

namespace Ui {
class VhallExitWidget;
}

class VhallExitWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VhallExitWidget(QWidget *parent = 0);
    ~VhallExitWidget();
signals:
    void clicked();
protected:
    void paintEvent(QPaintEvent *e);
    bool eventFilter(QObject *obj, QEvent *ev);
private:
    Ui::VhallExitWidget *ui;
    QPixmap mBackgroundPixmap;
};

#endif // VHALLEXITWIDGET_H
