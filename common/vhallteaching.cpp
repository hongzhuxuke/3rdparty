#include "vhallteaching.h"
#include "ui_vhallteaching.h"
#include <QDebug>
VhallTeaching::VhallTeaching(QWidget *parent,QRect rect) 
:CBaseDlg(parent)
,ui(new Ui::VhallTeaching)
{
    ui->setupUi(this);
    ui->btn->installEventFilter(this);

    this->setWindowFlags(Qt::FramelessWindowHint|Qt::Window);
    
    setAttribute(Qt::WA_TranslucentBackground);
    qDebug()<<rect;

    this->setGeometry(rect);
    this->mPixmap=QImage(rect.width(),rect.height(),QImage::Format_ARGB32);
    this->mPixmap.fill(Qt::transparent);

    mTip = new VhallTechingTip(this);

    QPixmap btnPixmap = QPixmap(":/teaching/BTN_BG");
    ui->btn->resize(btnPixmap.size());
    //ui->btn->setPixmap(btnPixmap);

    ui->close_btn->setIcon(QIcon(QPixmap(":/teaching/BTN_CLOSE")));
    connect(ui->close_btn,SIGNAL(clicked()),this,SLOT(close()));
}

VhallTeaching::~VhallTeaching()
{
    delete ui;
}
void VhallTeaching::AddStep(QRect areaRect,
    bool bCycle,
    QRect tipRect,
    QString tipContent,
    VhallTechingTip::TipMode tipMode,
    QString tipPixmap,
    QRect btnRect,
    QString btnTip){
    mStepList.append(VHTeachingStep(areaRect,bCycle,tipRect,tipContent,tipMode,tipPixmap,btnRect,btnTip));
}
void VhallTeaching::Teaching(bool m_bBlock) {
    this->move(this->parentWidget()->mapToGlobal(QPoint(0,0)));
    ui->btn->setStyleSheet(QString::fromWCharArray(L"background:url(:/teaching/BTN_BG);color:#ca1c29;font: 12pt \"微软雅黑\";"));
    Step();
    if(m_bBlock) {
       this->exec();
    }
    else {
      this->show();
      this->raise();
    }
}
bool VhallTeaching::eventFilter(QObject *o, QEvent *e){
    if(o==ui->btn) {
        if(e->type()==QEvent::Enter) {
            ui->btn->setStyleSheet(QString::fromWCharArray(L"background:url(:/teaching/BTN_BG);color:#ca1c29;font: 12pt \"微软雅黑\";"));
        }
        else if(e->type()==QEvent::Leave||e->type()==QEvent::MouseButtonPress) {
            ui->btn->setStyleSheet(QString::fromWCharArray(L"background:url(:/teaching/BTN_BG);color:#ff3333;font: 12pt \"微软雅黑\";"));
        }
        else if(e->type()==QEvent::MouseButtonRelease) {
            qDebug()<<"VhallTeaching MouseButtonRelease";
            ui->btn->setStyleSheet(QString::fromWCharArray(L"background:url(:/teaching/BTN_BG);color:#ca1c29;font: 12pt \"微软雅黑\";"));
            this->Step();
        }
    }
    return QWidget::eventFilter(o,e);
}
void VhallTeaching::Step(){
    if(this->stepIndex>=this->mStepList.count()) {
		signal_Accept();
        this->accept();
    }
    else {
       this->mPixmap.fill(QColor(0,0,0,200));
       //this->mPixmap.fill(Qt::red);
        
        VHTeachingStep step = mStepList[stepIndex];

        QPainter p(&mPixmap);
        
        if(!step.bCycle) {
           for(int x=0;x<step.areaRect.width();x++) {
             for(int y=0;y<step.areaRect.height();y++) {
               int dx=x+step.areaRect.x();
               int dy=y+step.areaRect.y();
               if(dx>=0&&dx<mPixmap.width()&&dy>=0&&dy<=mPixmap.height()) {
                  mPixmap.setPixel(dx,dy,0);
               }
             }
           }
        }
        else {

        }


        QList<VHTeachingRes> resList = mResMap.values(this->stepIndex);
        for(int i=0;i<resList.count();i++) {
           VHTeachingRes res = resList[i];
           QPixmap pixmap;
           pixmap.load(res.m_res);

           QRect rect = pixmap.rect();
           rect.setX(res.m_pos.x());
           rect.setY(res.m_pos.y());
           rect.setWidth(pixmap.size().width());
           rect.setHeight(pixmap.size().height());
           
           p.drawPixmap(rect,pixmap);
        }        

        mTip->move(step.tipRect.x(),step.tipRect.y());
        mTip->SetText(step.tipContent);
        mTip->SetTipMode(step.tipMode);
        mTip->LoadPixmap(step.tipPixmap);

        ui->btn->move(step.btnRect.x(),step.btnRect.y());
        ui->btn->setText(step.btnTip);

        QMap <int,QPoint>::iterator itor = mCloseBtn.find(stepIndex);
        
if(itor==mCloseBtn.end()) {
            ui->close_btn->hide();
        }
        else {
            ui->close_btn->move(*itor);
            ui->close_btn->show();
            ui->close_btn->raise();
        }
         
        this->repaint();
    }
    this->stepIndex++;
}
void VhallTeaching::paintEvent(QPaintEvent *e) {
    if(this->mPixmap.isNull()) {
        qDebug()<<"VhallTeaching::paintEvent this->mPixmap.isNull";
        return ;
    }
    QPainter p(this);
    p.drawImage(this->rect(),this->mPixmap);
}
void VhallTeaching::SlotClose() {
   qDebug()<<"SlotClose";
   this->close();
}

void VhallTeaching::AddResource(int step,QPoint p,QString res) {
   mResMap.insert(step,VHTeachingRes(p,res));
}
void VhallTeaching::SetCloseBtn(int step,QPoint p){
   mCloseBtn[step]=p;
}

