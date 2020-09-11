#ifndef VHALLRENDERWIDGET_H
#define VHALLRENDERWIDGET_H

#include <QWidget>

namespace Ui {
class VhallRenderWidget;
}

class VhallRenderWidget : public QWidget
{
    Q_OBJECT

public:
   explicit VhallRenderWidget(QWidget *parent = 0);
   ~VhallRenderWidget();
   void AddWidget(QWidget *w);
   QWidget *GetWidget();

private:
   Ui::VhallRenderWidget *ui;
   QWidget *mWidget;
};

#endif // VHALLRENDERWIDGET_H
