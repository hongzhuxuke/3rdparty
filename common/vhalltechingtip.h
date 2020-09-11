#ifndef VHALLTECHINGTIP_H
#define VHALLTECHINGTIP_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>

namespace Ui {
class VhallTechingTip;
}

class VhallTechingTip : public QWidget
{
    Q_OBJECT
public:
    typedef enum {
        Tip_Top,
        Tip_Down
    }TipMode;
public:
    explicit VhallTechingTip(QWidget *parent = 0);
    ~VhallTechingTip();
    void SetTipMode(TipMode mode);
    void SetText(QString);
    void LoadPixmap(QString);
protected:
    void paintEvent(QPaintEvent *);
private:
    Ui::VhallTechingTip *ui;
    QPixmap bgPixmap;
};

#endif // VHALLTECHINGTIP_H
