#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QBoxLayout>
#include <QCheckBox>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QStandardPaths>

//#include "../Unility/Unility.h"

#include "AlertDlg.h"
#include "TitleWidget.h"
#include "DebugTrace.h"
#include "ConfigSetting.h"

//extern ToolManager *globalToolManager;


#if _MSC_VER >= 1600  
#pragma execution_character_set("utf-8")  
#endif  

/*
message: message for exit 
isEnableCancel: if true, users can close the dialog; if false, uses can only click the "YES" button
*/

QString GetAppDataPath() {
   QString strAppDataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
   strAppDataPath.append("\\VHallHelper\\");
   return strAppDataPath;
}

AlertDlg::AlertDlg(QString message, bool isEnableCancel, QWidget *parent)
   : CBaseDlg(parent)
{
   this->setFixedSize(350, 212);
   QString qsConfPath = GetAppDataPath() + QString::fromStdWString(VHALL_TOOL_CONFIG);
   QString vhallLive = ConfigSetting::ReadString(qsConfPath, GROUP_DEFAULT, KEY_VHALL_LIVE, VHALL_LIVE_TEXT);
   this->setWindowTitle(vhallLive);
   setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
   setAttribute(Qt::WA_TranslucentBackground);
   this->setAutoFillBackground(true);

   mTitleBar = new TitleWidget("提示",this);
   mTitleBar->setStyleSheet("background-color:transparent;");
   if (!isEnableCancel)
      mTitleBar->hideCloseBtn();
   connect(mTitleBar, SIGNAL(closeWidget()), this, SLOT(close()));
   mMessageLbl = new QLabel(message,this);
   mMessageLbl->setObjectName("exitMsgLbl");
   mMessageLbl->setWordWrap(true);
   mMessageLbl->setAlignment(Qt::AlignHCenter);
   mMessageLbl->setStyleSheet("font: 16px '微软雅黑';background-color:transparent; ");
   mYesBtn = new QPushButton(QString::fromStdWString(L"确定"),this);
   mYesBtn->setObjectName("okButton");
   mYesBtn->setFixedSize(65, 24);
   mYesBtn->setStyleSheet(QString::fromUtf8("QPushButton#okButton{\n"
      "border-image: url(:/sysButton/img/sysButton/okButton.png);\n"
      "  color: rgb(200, 200, 200);\n"
      "background-color:transparent;"
      "}\n"
      "\n"
      "QPushButton#okButton:hover{\n"
      "background-color:transparent;"
      "	border-image: url(:/sysButton/img/sysButton/okButtonOn.png);\n"
      "  color: rgb(255, 255, 255);\n"
      "}"));

   mNoBtn = new QPushButton(QString::fromStdWString(L"取消"),this);
   mNoBtn->setObjectName("cancelBtn");
   mNoBtn->setFixedSize(65, 24);
   mNoBtn->setStyleSheet(QString("QPushButton#cancelBtn{\n"
      "background-color:transparent;"
      "	border-image:url(:/sysButton/img/sysButton/cancelButton.png);\n"
      "  color: rgb(200, 200, 200);\n"
      "background-color:transparent;"
      "}\n"
      "\n"
      "QPushButton#cancelBtn:hover{\n"
      "background-color:transparent;"
      "	border-image:url(:/sysButton/img/sysButton/cancelButtonOn.png);\n"
      "  color: rgb(255, 255, 255);\n"
      "}"));


   mYesBtn->setAttribute(Qt::WA_TranslucentBackground);
   mYesBtn->setAutoFillBackground(true);
   mNoBtn->setAttribute(Qt::WA_TranslucentBackground);
   mNoBtn->setAutoFillBackground(true);

   QVBoxLayout *main_layout = new QVBoxLayout();
   main_layout->addWidget(mTitleBar, 0, Qt::AlignTop);
   main_layout->setContentsMargins(0, 0, 0, 0);

   QVBoxLayout *center_layout = new QVBoxLayout();
   center_layout->addSpacing(38);
   center_layout->addWidget(mMessageLbl, Qt::AlignCenter); 
   center_layout->addSpacing(24);

   QHBoxLayout *btn_layout = new QHBoxLayout();
   btn_layout->addWidget(mYesBtn);
   if (isEnableCancel){
      btn_layout->addSpacing(-70);
      btn_layout->addWidget(mNoBtn);
   }
   else {
      mNoBtn->hide();
   }
      
   connect(mYesBtn, SIGNAL(clicked()), this, SLOT(Slot_Accept()));
   connect(mNoBtn, SIGNAL(clicked()), this, SLOT(Slot_Reject()));
   
   center_layout->addLayout(btn_layout);
   center_layout->setContentsMargins(0, 0, 0, 30);

   main_layout->addLayout(center_layout);
   main_layout->addStretch(16);
   setLayout(main_layout);
   //installEventFilter(this);
  // globalToolManager->GetDataManager()->WriteLog("%s message:%s\n",__FUNCTION__,message.toUtf8().data());
}

AlertDlg::AlertDlg(QString message, QString optionCbxText, bool isEnableCancel, QWidget *parent)
: CBaseDlg(parent)
{
   this->setFixedSize(340, 190);
   setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
   setAttribute(Qt::WA_TranslucentBackground);
   this->setAutoFillBackground(true);

   mTitleBar = new TitleWidget("提示", this);
   mTitleBar->setStyleSheet("background-color:transparent;");
   if (!isEnableCancel)
      mTitleBar->hideCloseBtn();
   connect(mTitleBar, SIGNAL(closeWidget()), this, SLOT(close()));
   mMessageLbl = new QLabel(message, this);
   mMessageLbl->setObjectName("exitMsgLbl");
   mMessageLbl->setWordWrap(true);
   mMessageLbl->setAlignment(Qt::AlignHCenter);
   mMessageLbl->setStyleSheet("font: 16px '微软雅黑';");

   mOptionCbx = new QCheckBox(optionCbxText, this);
   mOptionCbx->setObjectName("exitOptCbx");
   mOptionCbx->setStyleSheet("color: rgb(153, 153, 153);font: 12px '微软雅黑';");

   QVBoxLayout *main_layout = new QVBoxLayout();
   main_layout->addWidget(mTitleBar, 0, Qt::AlignTop);
   main_layout->setContentsMargins(0, 0, 0, 0);

   QVBoxLayout *center_layout = new QVBoxLayout();
   center_layout->addSpacing(20);
   center_layout->addWidget(mMessageLbl, Qt::AlignCenter);
   center_layout->addSpacing(14);
   QHBoxLayout *opt_layout = new QHBoxLayout();
   opt_layout->addStretch();
   opt_layout->addWidget(mOptionCbx);
   opt_layout->addStretch();
   center_layout->addLayout(opt_layout);
   center_layout->addSpacing(22);
   

   QHBoxLayout *btn_layout = new QHBoxLayout();
   
   mYesBtn = new QPushButton(QString::fromStdWString(L"确定"),this);
   mYesBtn->setObjectName("okButton");
   mYesBtn->setFixedSize(65, 24);

   mYesBtn->setStyleSheet(QString::fromUtf8(
      "QPushButton#okButton{\n"
      "background-color:transparent;"
      "border-image: url(:/sysButton/img/sysButton/okButton.png);\n"
      "  color: rgb(200, 200, 200);\n"
      "}\n"
      "\n"
      "QPushButton#okButton:hover{\n"
      "	border-image: url(:/sysButton/img/sysButton/okButtonOn.png);\n"
      "  color: rgb(255, 255, 255);\n"
      "}"));


   mNoBtn = new QPushButton(QString::fromStdWString(L"取消"), this);
   mNoBtn->setObjectName("cancelBtn");
   mNoBtn->setFixedSize(65, 24);
   mNoBtn->setStyleSheet(QLatin1String(
      "QPushButton#cancelBtn{\n"
      "background-color:transparent;"
      "	border-image:url(:/sysButton/img/sysButton/cancelButton.png);\n"
      "  color: rgb(203, 203, 203);\n"
      "}\n"
      "\n"
      "QPushButton#cancelBtn:hover{\n"
      "	border-image:url(:/sysButton/img/sysButton/cancelButtonOn.png);\n"
      "  color: rgb(255, 255, 255);\n"
      "}"));

   mYesBtn->setAttribute(Qt::WA_TranslucentBackground);
   mYesBtn->setAutoFillBackground(true);

   mNoBtn->setAttribute(Qt::WA_TranslucentBackground);
   mNoBtn->setAutoFillBackground(true);

   btn_layout->addWidget(mYesBtn);
   if (isEnableCancel){
      btn_layout->addSpacing(-70);
      btn_layout->addWidget(mNoBtn);
   }
   else {
      mNoBtn->hide();
   }

   connect(mYesBtn, SIGNAL(clicked()), this, SLOT(accept()));
   connect(mNoBtn, SIGNAL(clicked()), this, SLOT(reject()));

   center_layout->addLayout(btn_layout);
   center_layout->setContentsMargins(0, 0, 0, 30);

   main_layout->addLayout(center_layout);
   main_layout->addStretch(16);
   setLayout(main_layout);
   //installEventFilter(this);
}

AlertDlg::~AlertDlg() {
}
void AlertDlg::SetYesBtnText(QString txt) {
   if(mYesBtn) {
      mYesBtn->setText(txt);
   }
}

void AlertDlg::SetNoBtnText(QString txt) {
   if(mNoBtn) {
      mNoBtn->setText(txt);
   }
}

void AlertDlg::SetTipsText(const QString& strmsg)
{
	if (NULL != mMessageLbl)
	{
		mMessageLbl->setText(strmsg);
	}
	
}

void AlertDlg::paintEvent(QPaintEvent *){
   QPainter painter(this);
   painter.setRenderHint(QPainter::Antialiasing, true);
   painter.setPen(QPen(QColor(30, 30, 30), 1));
   painter.setBrush(QColor(43, 44, 46));
   painter.drawRoundedRect(rect(), 3.0, 3.0);
}

void AlertDlg::CenterWindow(QWidget * parent){
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

void AlertDlg::setOptCbxState(bool checked){
   if (mOptionCbx) {
      mOptionCbx->setChecked(checked);
   }
   
}
bool AlertDlg::getOptCbxState(){
   if (mOptionCbx) {
      return mOptionCbx->isChecked();
   }
   return false;
}
void AlertDlg::SetTitle(QString title){
   setWindowTitle(title);
   mTitleBar->SetTitle(title);
}

void AlertDlg::Slot_Accept() {
   accept();
}

void AlertDlg::Slot_Reject() {
   reject();
}

