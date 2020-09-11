#ifndef VHALLMENU_H
#define VHALLMENU_H

#include <QWidget>
#include <QMenu>
#include <QMap>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>

namespace Ui {
class VhallMenu;
}

class VhallMenu : public QWidget
{
    Q_OBJECT

public:
    explicit VhallMenu(QWidget *parent = 0);
    ~VhallMenu();
    void SetPixmap(QString,int,int,int,int);
    QAction *addAction(const QString &text);
    QAction *addAction(QAction *action);
    void clear();
signals:
    void triggered(QAction *);
protected:
    bool eventFilter(QObject *o,QEvent *e);
    void paintEvent(QPaintEvent *);
private:
    Ui::VhallMenu *ui;
    QMap<QWidget *,QAction *> actionMap;
    QPixmap pixmap;
};

#endif // VHALLMENU_H
