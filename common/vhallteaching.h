#ifndef VHALLTEACHING_H
#define VHALLTEACHING_H

#include <QDialog>
#include <QList>
#include <QPaintEvent>
#include <QPainter>
#include <QRect>
#include <QImage>
#include <QMultiMap>
#include <QPoint>
#include "cbasedlg.h"

#include "vhalltechingtip.h"

namespace Ui {
class VhallTeaching;
}
struct VHTeachingRes{
   VHTeachingRes(QPoint pos,QString res){
      this->m_res=res;
      m_pos=pos;
   }
   
   QPoint m_pos;
   QString m_res;
};
struct VHTeachingStep{
    VHTeachingStep(  QRect areaRect,
                        bool bCycle,
                        QRect tipRect,
                        QString tipContent,
                        VhallTechingTip::TipMode tipMode,
                        QString tipPixmap,
                        QRect btnRect,
                        QString btnTip
                       ){
        this->areaRect = areaRect;
        this->tipRect = tipRect;
        this->tipContent = tipContent;
        this->tipMode = tipMode;
        this->tipPixmap = tipPixmap;
        this->btnRect = btnRect;
        this->btnTip = btnTip;
        this->bCycle = bCycle;
    }
    
    QRect areaRect;
    QRect tipRect;
    QString tipContent;

    VhallTechingTip::TipMode tipMode;
    QString tipPixmap;
    
    QRect btnRect;
    QString btnTip;

    bool bCycle;
    
};

class VhallTeaching : public CBaseDlg
{
    Q_OBJECT

public:
    explicit VhallTeaching(QWidget *parent,QRect rect);
    ~VhallTeaching();
    void AddStep(QRect areaRect,bool bCycle,
                     QRect tipRect,QString tipContent,
                      VhallTechingTip::TipMode tipMode,
                        QString tipPixmap,
                     QRect btnRect,QString btnTip);
    void AddResource(int step,QPoint p,QString res);
    void Teaching(bool m_bBlock = true);
    void SetCloseBtn(int step,QPoint p);
signals:
	void signal_Accept();
public slots:
   void SlotClose();
private:
    void Step();
protected:
    bool eventFilter(QObject *, QEvent *);
    void paintEvent(QPaintEvent *);
private:
    Ui::VhallTeaching *ui;
    QList<VHTeachingStep> mStepList;
    int stepIndex = 0;
    QImage mPixmap;
    VhallTechingTip *mTip;
    QMultiMap <int,VHTeachingRes> mResMap;
    QMap <int,QPoint> mCloseBtn;
};

#endif // VHALLTEACHING_H
