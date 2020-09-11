#include "vhallhorlistwidget.h"
#include "vhallrenderwidget.h"

#include "ui_vhallhorlistwidget.h"
#include "VideoRenderWdg.h"
#include "CRPluginDef.h"
#include "DebugTrace.h"
#include "MainUIIns.h"
#include "Msg_VhallRightExtraWidget.h"
#include "IVhallRightExtraWidgetLogic.h"
#include <QMouseEvent>

VhallHorListWidget::VhallHorListWidget(QWidget *parent) :
   QWidget(parent),
   ui(new Ui::VhallHorListWidget) {
   ui->setupUi(this);
}

VhallHorListWidget::~VhallHorListWidget() {
   delete ui;
}
void VhallHorListWidget::Clear() {
   for(auto itor = m_pWidgets.begin();itor!=m_pWidgets.end();itor++) {
      QWidget *w = *itor;
      w->removeEventFilter(this);
      ui->horizontalLayout->removeWidget(w);
      delete w;
   }
   
   m_pWidgets.clear();
}

void VhallHorListWidget::Append(VhallHorListWidgetKey &key,QWidget *w) {
   if (w) {
      m_pWidgets[key] = w;
      ui->horizontalLayout->addWidget(w);
      w->installEventFilter(this);
   }
}

void VhallHorListWidget::Remove(VhallHorListWidgetKey &key) {
   auto itor = m_pWidgets.find(key);
   if(itor != m_pWidgets.end()) {
      QWidget *w = *itor;
      w->removeEventFilter(this);
      m_pWidgets.erase(itor);
      ui->horizontalLayout->removeWidget(w);
      delete w;
   }
}
bool VhallHorListWidget::eventFilter(QObject *o, QEvent *e) {
   if(e->type()==QEvent::MouseButtonRelease) {
      QMouseEvent *me = dynamic_cast<QMouseEvent *>(e);
      if(me) {
         if(me->button() == Qt::LeftButton) {
            for(auto itor = m_pWidgets.begin() ; itor != m_pWidgets.end() ; itor ++) {
               if(itor.value() == o) {
                  emit this->SigClicked((QWidget *)itor.value());
                  break;
               }
            }  
         }
      }
   }
   
   return QWidget::eventFilter(o,e);
}
QWidget *VhallHorListWidget::GetRenderWidget(VhallHorListWidgetKey &identifier){
   auto itor = m_pWidgets.find(identifier);
   if(itor != m_pWidgets.end()) {
      return *itor;
   }                
   return NULL;
}

QList<QString> VhallHorListWidget::GetRenderMembers() {
   QList<QString> renders;
   auto itor = m_pWidgets.begin();
   while (itor != m_pWidgets.end()) {
      renders.push_back(itor.key());
      itor++;
   }
   return renders;
}

void VhallHorListWidget::RefreshOnlineList() {
   qDebug() << "#######VhallSpeakerList::RefreshOnlineList()#######";
   VH::CComPtr<IVhallRightExtraWidgetLogic> pVhallRightExtraWidget = NULL;
   DEF_GET_INTERFACE_PTR(SingletonMainUIIns, PID_IVhallRightExtraWidget, IID_IVhallRightExtraWidgetLogic, pVhallRightExtraWidget);
   if (!pVhallRightExtraWidget) {
      return;
   }

   for (auto itor = m_pWidgets.begin(); itor != m_pWidgets.end(); itor++) {
      QWidget *w = *itor;
      VhallRenderWidget *wrender = dynamic_cast<VhallRenderWidget *>(w);
      if (wrender) {
         CVideoRenderWdg *render = dynamic_cast<CVideoRenderWdg *>(wrender->GetWidget());
         if (render) {
            QString id = render->GetID();
            wchar_t name[256] = { 0 };
            //if (pVhallRightExtraWidget->GetUserName(id.toUtf8().data(), name)) 
			{
               render->SetUserName(QString::fromWCharArray(name));
               qDebug() << "VhallHorListWidget::RefreshOnlineList() " << id << QString::fromWCharArray(name);
            }
         }
      }
   }
}

bool VhallHorListWidget::CheckRendExist(QWidget* render) {
   for (int i = 0; i < ui->horizontalLayout->count(); i++) {
      QWidget* w = ui->horizontalLayout->itemAt(i)->widget();
      if (w == render) {
         return true;
      }
   }
   return false;
}

void VhallHorListWidget::ChangeCenterRender(QString showUser, QString hideUser) {
   if (!hideUser.isEmpty()) {
      auto itor = m_pWidgets.find(hideUser);
      if (itor != m_pWidgets.end()) {
         itor.value()->hide();
      }
   }

   if (!showUser.isEmpty()) {
      auto itor = m_pWidgets.find(showUser);
      if (itor != m_pWidgets.end() && itor.value() != NULL) {
         itor.value()->show();
         VhallRenderWidget *wrender = dynamic_cast<VhallRenderWidget *>(itor.value());
         if (wrender) {
            CVideoRenderWdg *render = dynamic_cast<CVideoRenderWdg *>(wrender->GetWidget());
            if (render && !render->IsFrameRender()) {
               render->Clear(true);
            }
         }
      }
   }
}

int VhallHorListWidget::GetRenderCount() {
   return m_pWidgets.count();
}

const QMap<QString, QWidget*> VhallHorListWidget::GetRenderWidgetsMap() const {
   return m_pWidgets;
}

