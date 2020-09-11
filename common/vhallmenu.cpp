#include "vhallmenu.h"
#include "ui_vhallmenu.h"
#include <QDebug>

VhallMenu::VhallMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VhallMenu) {
    ui->setupUi(this);

    setAutoFillBackground(false);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::SplashScreen | Qt::WindowStaysOnTopHint | Qt::Tool);
    this->setFocusPolicy(Qt::StrongFocus);
    this->installEventFilter(this);
    ui->widget->setStyleSheet("background-color:rgb(43,44,46);");
}

VhallMenu::~VhallMenu() {
    delete ui;
}
QAction *VhallMenu::addAction(const QString &text) {

    QAction *action = new QAction(NULL);
    action->setText(text);
    return this->addAction(action);
}
QAction *VhallMenu::addAction(QAction *action) {
    QLabel *label = new QLabel();
    label->setStyleSheet("background-color:transparent;color:#c8c8c8;");
    //label->setStyleSheet("background-color:red;color:#c8c8c8;");
    QString actionText = action->text();
    qDebug()<<"VhallMenu::addAction "<<actionText;
    label->setText(actionText);
    label->installEventFilter(this);
    //label->setMinimumHeight(22);
   
    actionMap[label] = action;
    ui->verticalLayout->addWidget(label);
    label->show();
    label->installEventFilter(this);
    
    return action;
}
bool VhallMenu::eventFilter(QObject *o,QEvent *e) {
    if(e->type()==QEvent::MouseButtonRelease) {
        QLabel *label = dynamic_cast<QLabel *>(o);
        if(label) {
            label->setStyleSheet("color:#fc363b;");
            QAction *action = actionMap[label];
            if(action) {
               emit action->toggled(true);
               qDebug()<<"VhallMenu::eventFilter toggle";
            }
        }
        
        for(auto itor = actionMap.begin();itor != actionMap.end();itor++) {
            if(o==itor.key()) {
                emit this->triggered(itor.value());
            }
        }
        this->close();
    }
    else if(e->type()==QEvent::FocusOut) {
        if(o == this) {
            this->close();
        }
    }
    else if(e->type() == QEvent::Show) {
        if(o == this) {
            this->setFocus();
        }
    }
    else if(e->type() == QEvent::Enter) {
        QLabel *label = dynamic_cast<QLabel *>(o);
        if(label) {
            label->setStyleSheet("color:#fc363b;");
        }
    }
    else if(e->type() == QEvent::Leave) {
        QLabel *label = dynamic_cast<QLabel *>(o);
        if(label) {
            label->setStyleSheet("color:#c8c8c8;");
        }
    }
    else if(e->type() == QEvent::MouseButtonPress) {
        QLabel *label = dynamic_cast<QLabel *>(o);
        if(label) {
            label->setStyleSheet("color:#c8c8c8;");
        }
    }

    return QWidget::eventFilter(o,e);
}
void VhallMenu::SetPixmap(QString name,int left,int top,int right,int bottom) {
    this->pixmap = QPixmap(name);
    if(this->pixmap.isNull()) {
         qDebug()<<"VhallMenu::SetPixmap is NULL "<<name;
        return ;
    }
    
    qDebug()<<"VhallMenu::SetPixmap not NULL "<<this->pixmap.size();
    this->setMinimumSize(this->pixmap.size());
    this->setMaximumSize(this->pixmap.size());
    this->setMinimumSize(this->pixmap.size());
    this->setMaximumSize(this->pixmap.size());
    
    this->setContentsMargins(left,top,right,bottom);
}
void VhallMenu::paintEvent(QPaintEvent *e) {
    QPainter p (this);
    p.drawPixmap(this->rect(),pixmap);
} 
void VhallMenu::clear() {
   for(auto itor = actionMap.begin();itor != actionMap.end(); itor ++) {
      delete itor.key();
      delete itor.value();
   }
   actionMap.clear();
}

