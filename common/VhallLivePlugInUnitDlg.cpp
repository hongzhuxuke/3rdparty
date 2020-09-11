#include "VhallLivePlugInUnitDlg.h"
#include "ui_VhallLivePlugInUnitDlg.h"
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QWebEngineView>
#include <QWebChannel>
#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QTimer>

#include "title_button.h"
#include "DebugTrace.h"
#include "ConfigSetting.h"
#include "pathmanager.h"

VhallLivePlugInUnitDlg::VhallLivePlugInUnitDlg(QWidget *parent) :
CBaseDlg(parent),
ui(new Ui::VhallLivePlugInUnitDlg) {
   ui->setupUi(this);
   setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
   ui->widget_title->installEventFilter(this);
   ui->left->installEventFilter(this);
   ui->left_top->installEventFilter(this);
   ui->top->installEventFilter(this);
   ui->right_top->installEventFilter(this);
   ui->right->installEventFilter(this);
   ui->right_down->installEventFilter(this);
   ui->down->installEventFilter(this);
   ui->left_down->installEventFilter(this);

   this->installEventFilter(this);
   connect(this, SIGNAL(SigJsCode(QString)), this, SLOT(SlotJsCode(QString)));
   //resize(1000, 610);
   this->setFocusPolicy(Qt::ClickFocus);
}

VhallLivePlugInUnitDlg::~VhallLivePlugInUnitDlg() {
   if (m_pWebChannel && m_pWebEngineView) {
      m_pWebEngineView->page()->setWebChannel(NULL);
      delete m_pWebChannel;
      m_pWebChannel = NULL;
   }

   if (m_pWebEngineView) {
      ui->verticalLayout_web->removeWidget(m_pWebEngineView);
      m_pWebEngineView->stop();
      delete m_pWebEngineView;
      m_pWebEngineView = NULL;
   }
   if (ui) {
      delete ui;
      ui = NULL;
   }
}

void VhallLivePlugInUnitDlg::SetWindowTitle(QString title) {
   this->setWindowTitle(title);
   ui->label_tool->setText(title);
}

bool VhallLivePlugInUnitDlg::eventFilter(QObject *o, QEvent *e) {
   if (ui->widget_title == o) {
      if (e->type() == QEvent::MouseButtonPress) {
         QMouseEvent *me = dynamic_cast<QMouseEvent *>(e);
         if (me) {
            m_PressPoint = me->globalPos();
            m_StartPoint = this->pos();
         }
      } else if (e->type() == QEvent::MouseMove) {
         QMouseEvent *me = dynamic_cast<QMouseEvent *>(e);
         if (me) {
            if (ui->widget_title == o && !this->isMaximized()) {
               this->move(m_StartPoint + me->globalPos() - m_PressPoint);
            }
         }
      } else if (e->type() == QEvent::MouseButtonDblClick) {
         this->SlotMaxClicked();
      } else if (e->type() == QEvent::Enter) {
         this->setCursor(QCursor(Qt::ArrowCursor));
         qDebug() << "VhallWebViewFree::eventFilter Enter";
      }
   } else if (this == o) {
      if (e->type() == QEvent::FocusIn) {
         qDebug() << "VhallWebViewFree::eventFilter QEvent::FocusIn";
         this->raise();
      } 
   } else {
      if (e->type() == QEvent::MouseButtonPress) {
         this->m_PressPoint = this->cursor().pos();
         this->m_startRect = this->geometry();
      } else if (e->type() == QEvent::MouseMove) {

         QRect geometry = m_startRect;
         QPoint diff = this->cursor().pos() - this->m_PressPoint;

         int left = geometry.x();
         int top = geometry.y();
         int right = geometry.x() + geometry.width();
         int bottom = geometry.y() + geometry.height();

         int new_left = left;
         int new_top = top;
         int new_right = right;
         int new_bottom = bottom;

         int x_diff = diff.x();
         int y_diff = diff.y();

         int min_width = this->minimumWidth();
         int min_height = this->minimumHeight();

         if (ui->left == o) {
            new_left = left + diff.x();
            if (right - new_left < min_width) {
               new_left = right - min_width;
            }
         } else if (ui->left_top == o) {
            new_left = left + diff.x();
            if (right - new_left < min_width) {
               new_left = right - min_width;
            }
            new_top = top + diff.y();
            if (bottom - new_top < min_height) {
               new_top = bottom - min_height;
            }
         } else if (ui->top == o) {
            new_top = top + diff.y();
            if (bottom - new_top < min_height) {
               new_top = bottom - min_height;
            }
         } else if (ui->right_top == o) {
            new_right = right + diff.x();
            if (new_right - left < min_width) {
               new_right = left + min_width;
            }
            new_top = top + diff.y();
            if (bottom - new_top < min_height) {
               new_top = bottom - min_height;
            }
         } else if (ui->right == o) {
            new_right = right + diff.x();
            if (new_right - left < min_width) {
               new_right = left + min_width;
            }
         } else if (ui->right_down == o) {
            new_right = right + diff.x();
            if (new_right - left < min_width) {
               new_right = left + min_width;
            }
            new_bottom = bottom + diff.y();
            if (new_bottom - top < min_height) {
               new_bottom = top + min_height;
            }
         } else if (ui->down == o) {
            new_bottom = bottom + diff.y();
            if (new_bottom - top < min_height) {
               new_bottom = top + min_height;
            }
         } else if (ui->left_down == o) {
            new_left = left + diff.x();
            if (right - new_left < min_width) {
               new_left = right - min_width;
            }
            new_bottom = bottom + diff.y();
            if (new_bottom - top < min_height) {
               new_bottom = top + min_height;
            }
         }

         geometry.setX(new_left);
         geometry.setY(new_top);
         geometry.setWidth(new_right - new_left);
         geometry.setHeight(new_bottom - new_top);

         if (m_pWebEngineView) {
            int width = ui->widget_10->width();
            int height = ui->widget_10->height();
            m_pWebEngineView->resize(width,height);
         }
         if (!this->isMaximized()) {
            this->setGeometry(geometry);
         }
      }
   }

   return QWidget::eventFilter(o, e);
}
bool VhallLivePlugInUnitDlg::nativeEvent(const QByteArray &eventType, void *message, long *result) {
   MSG* msg = reinterpret_cast<MSG*>(message);
   if (WM_SETCURSOR == msg->message) {
      if (ui->widget_title->rect().contains(ui->widget_title->mapFromGlobal(this->cursor().pos()))) {
         SetCursor(LoadCursor(NULL, IDC_ARROW));
      }
   }
   return QWidget::nativeEvent(eventType, message, result);
}

bool VhallLivePlugInUnitDlg::Create() {
   m_pBtnRefresh = new TitleButton(this);
   m_pBtnRefresh->loadPixmap(":/sysButton/refresh_button");
   m_pBtnRefresh->setToolTip(QString::fromWCharArray(L"刷新"));
   connect(m_pBtnRefresh, SIGNAL(clicked()), this, SLOT(SlotRefresh()));
   ui->layout_tool->addWidget(m_pBtnRefresh);

   m_pBtnMin = new TitleButton(this);
   m_pBtnMin->loadPixmap(":/sysButton/min_button");
   ui->layout_tool->addWidget(m_pBtnMin);
   connect(m_pBtnMin, SIGNAL(clicked()), this, SLOT(SlotMinClicked()));

   m_pBtnMax = new TitleButton(this);
   m_pBtnMax->loadPixmap(":/sysButton/max_button");
   ui->layout_tool->addWidget(m_pBtnMax);
   connect(m_pBtnMax, SIGNAL(clicked()), this, SLOT(SlotMaxClicked()));

   m_pBtnClose = new TitleButton(this);
   m_pBtnClose->loadPixmap(":/sysButton/close_button");
   ui->layout_tool->addWidget(m_pBtnClose);
   connect(m_pBtnClose, SIGNAL(clicked()), this, SLOT(SlotClose()));

   bool debugjs = (0 != ConfigSetting::ReadInt(CPathManager::GetConfigPath(), GROUP_DEFAULT, DEBUGJS, 0));
   if (!debugjs) {
      //ui->webView->setContextMenuPolicy(Qt::NoContextMenu);
   }

   return true;
}
void VhallLivePlugInUnitDlg::Destory() {

}
void VhallLivePlugInUnitDlg::CenterWindow(QWidget* parent) {
   int x = 0;
   int y = 0;
   if (NULL == parent) {
      const QRect rect = QApplication::desktop()->availableGeometry();
      x = rect.left() + (rect.width() - width()) / 2;
      y = rect.top() + (rect.height() - height()) / 2;
   } else {
      QPoint point(0, 0);
      point = parent->mapToGlobal(point);
      x = point.x() + (parent->width() - width()) / 2;
      y = point.y() + (parent->height() - height()) / 2;
   }
   move(x, y);
}
void VhallLivePlugInUnitDlg::Load(QString url, QObject *obj) {
   m_obj = obj;
   m_bInit = false;
   m_pluginUrl = url;
   AddWebEngineView();
   if (m_pWebEngineView && m_pWebChannel) {
      m_pWebEngineView->page()->profile()->clearHttpCache(); // 清理缓存
      m_pWebEngineView->page()->profile()->clearAllVisitedLinks(); // 清理浏览记录
      m_pWebEngineView->load(url);
      TRACE6("%s plugin_url:%s\n",__FUNCTION__,url.toStdString().c_str());
      //m_bFirstShow = false;
   }
   mIsLoadUrlFinished = false;
}
void VhallLivePlugInUnitDlg::loadFinished(bool ok) {
   qDebug() << "VhallWebViewFree::loadFinished" << ok;
   TRACE6("VhallWebViewFree::loadFinished %d", ok ? 1 : 0);
   static int iLoadCount = 0;
   if (ok) {

	  m_bInit = true;
	  mIsLoadUrlFinished = true;
   }
   else if (iLoadCount <3)
   {
		if (m_pWebEngineView) {
			//m_pWebEngineView->page()->runJavaScript("deviceReady()");
		}
		iLoadCount++;
		QTimer::singleShot(500, this, SLOT(SlotReLoad()));
   }
}
void VhallLivePlugInUnitDlg::executeJSCode(QString method) {
   qDebug() << "executeJSCode " << method;
   emit SigJsCode(method);
}
void VhallLivePlugInUnitDlg::SlotJsCode(QString method) {
   qDebug() << "SlotJsCode " << method;
   if (m_bInit) {
      if (m_pWebEngineView) {
         m_pWebEngineView->page()->runJavaScript(method);
      }
   }
}
void VhallLivePlugInUnitDlg::SlotMaxClicked() {
   if (this->isMaximized()) {
      this->showNormal();
      this->setGeometry(m_lastGeometry);
      m_pBtnMax->loadPixmap(":/sysButton/max_button");
   } else {
      m_lastGeometry = this->geometry();
      this->showMaximized();
      m_pBtnMax->loadPixmap(":/sysButton/normal_button");
   }
   this->repaint();
}
void VhallLivePlugInUnitDlg::SlotMinClicked() {
   this->showMinimized();
}
void VhallLivePlugInUnitDlg::SlotRefresh() {
   if (m_pWebChannel && m_pWebEngineView) {
      m_pWebEngineView->page()->setWebChannel(NULL);
      delete m_pWebChannel;
      m_pWebChannel = NULL;
   }

   if (m_pWebEngineView) {
      ui->verticalLayout_web->removeWidget(m_pWebEngineView);
      m_pWebEngineView->stop();
      delete m_pWebEngineView;
      m_pWebEngineView = NULL;
   }

   if (m_obj) {
      Load(m_pluginUrl, m_obj);
      m_bInit = false;
      //m_bFirstShow = false;
   }
}
void VhallLivePlugInUnitDlg::SlotReLoad() {
   if (m_obj /*&& m_bFirstShow*/) {
      Load(m_pluginUrl, m_obj);
      //m_bFirstShow = false;
   }
}

void VhallLivePlugInUnitDlg::SlotClose() {
   emit SigClose();
}

void VhallLivePlugInUnitDlg::AddWebEngineView() {
   if (m_pWebEngineView == NULL && m_pWebChannel == NULL) {
      m_pWebEngineView = new QWebEngineView(this);
      m_pWebChannel = new QWebChannel(this);
      if (m_pWebEngineView && m_pWebChannel) {
         m_pWebEngineView->page()->profile()->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
         m_pWebEngineView->page()->profile()->clearHttpCache(); // 清理缓存
         m_pWebEngineView->page()->profile()->clearAllVisitedLinks(); // 清理浏览记录

         connect(m_pWebEngineView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
         QWebEngineSettings* pWebSettings = m_pWebEngineView->settings();
         if (pWebSettings) {
            pWebSettings->setAttribute(QWebEngineSettings::AutoLoadImages, true);
            pWebSettings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
            pWebSettings->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, false);
            pWebSettings->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
            pWebSettings->setAttribute(QWebEngineSettings::PluginsEnabled, true);
            pWebSettings->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, true);
         }
         //如果不设置这就话，加载URL时会先显示以下白色背景。
         m_pWebEngineView->page()->setBackgroundColor(Qt::transparent);
         ui->verticalLayout_web->addWidget(m_pWebEngineView);
         m_pWebEngineView->setContextMenuPolicy(Qt::NoContextMenu);

         m_pWebChannel->registerObject("bridge", m_obj);
         m_pWebEngineView->page()->setWebChannel(m_pWebChannel);
      }
   }
}

void VhallLivePlugInUnitDlg::JsCallQtDebug(QString msg) {
   qDebug() << msg;
}

void VhallLivePlugInUnitDlg::ReloadPluginUrl() {
   if (m_obj && mFirstLoad) {
      Load(m_pluginUrl, m_obj);
      mFirstLoad = false;
   }
}

void VhallLivePlugInUnitDlg::InitPluginUrl(QString url, QObject *obj) {
   m_obj = obj;
   m_bInit = false;
   m_pluginUrl = url;
   mFirstLoad = true;
}

bool VhallLivePlugInUnitDlg::IsLoadUrlFinished() {
   return mIsLoadUrlFinished;
}

