#include "vhnetworkproxy.h"
#include "ui_vhnetworkproxy.h"
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
#include "vhproxytest.h"
#include "titleWidget.h"
#include "ConfigSetting.h"
#include <string>
#include <windows.h>
#include "pathManage.h"
#include "pathManager.h"
#include "HttpProxyGet.h"
#include "vhnetworkproxy.h"
#include "ConfigSetting.h"

//std::wstring GetAppPath();
VHNetworkProxy::VHNetworkProxy(QWidget *parent) :
CBaseDlg(parent),
   ui(new Ui::VHNetworkProxy) {
   
   ui->setupUi(this);
   setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
   setAttribute(Qt::WA_TranslucentBackground);
   this->setAutoFillBackground(true);
   
   this->setWindowTitle(QString::fromWCharArray(L"网络代理设置"));
   
   TitleWidget *title = new TitleWidget(QString::fromWCharArray(L"网络代理"),this);
   ui->layout_title->addWidget(title);
   connect(title,SIGNAL(closeWidget()),this,SLOT(close()));

   //std::wstring confPath = GetAppPath() + CONFIGPATH;
	QString qsConfPath = CPathManager::GetConfigPath();
   bool bProxyOpen = ConfigSetting::ReadInt(qsConfPath, GROUP_DEFAULT, PROXY_OPEN, 0);
   int proxyType = ConfigSetting::ReadInt(qsConfPath, GROUP_DEFAULT, PROXY_TYPE, ProxyConfig_Http);
   if(bProxyOpen) {
      ui->comboBox_proxyEnable->setCurrentIndex(proxyType);
      UIProxyEnable();
   }
   else {
      ui->comboBox_proxyEnable->setCurrentIndex(0);
      UIProxyDisable();
   }

   QString host = ConfigSetting::ReadString(qsConfPath, GROUP_DEFAULT, PROXY_HOST, "");
   int port = ConfigSetting::ReadInt(qsConfPath, GROUP_DEFAULT, PROXY_PORT, 80);
   QString usr = ConfigSetting::ReadString(qsConfPath, GROUP_DEFAULT, PROXY_USERNAME, "");
   QString pwd = ConfigSetting::ReadString(qsConfPath, GROUP_DEFAULT, PROXY_PASSWORD, "");

   ui->ip->setText(host);
   ui->port->setText(QString::number(port));
   ui->name->setText(usr);
   ui->pwd->setText(pwd); 
}

VHNetworkProxy::~VHNetworkProxy()
{
    delete ui;
}
void VHNetworkProxy::paintEvent(QPaintEvent *){
   QPainter painter(this);
   painter.setRenderHint(QPainter::Antialiasing, true);
   painter.setPen(QPen(QColor(54, 54, 54), 1));
   painter.setBrush(QColor(38, 38, 38));
   painter.drawRoundedRect(rect(), 4.0, 4.0);
}
void VHNetworkProxy::on_btn_testing_clicked()
{
   ui->label_tip->clear();
   #define VersionHttpAPI QString("http://e.vhall.com/api/client/v1/util/current-version")
   //std::wstring confPath = GetAppPath() + CONFIGPATH;
	QString qsConfPath = CPathManager::GetToolConfigPath();

   QString ip = ui->ip->text();
   unsigned short port = ui->port->text().toUShort();
   QString userName = ui->name->text();
   QString password = ui->pwd->text();
   QString url = ConfigSetting::ReadString(qsConfPath, GROUP_DEFAULT, KEY_CHK_VER_URL, VersionHttpAPI);
   HttpRequest req;
   if (req.VHProxyTest(ip, port, userName, password, url)) {
      qDebug()<<"VHProxyTest Successed"<<ip<<port<<userName<<password<<url;
      ui->label_tip->setText(QString::fromWCharArray(L"该代理可使用"));
   }
   else{
      qDebug()<<"VHProxyTest Failed"<<ip<<port<<userName<<password<<url;
      ui->label_tip->setText(QString::fromWCharArray(L"无法连接到代理服务器"));
   }
}

void VHNetworkProxy::UIProxyEnable() {
   ui->btn_testing->setEnabled(true);
   ui->ip->setEnabled(true);
   ui->port->setEnabled(true);
   ui->name->setEnabled(true);
   ui->pwd->setEnabled(true);
   
   ui->label_proxy_port->setStyleSheet("color:#999999;");
   ui->label_proxy_ip->setStyleSheet("color:#999999;");
   ui->label_proxy_name->setStyleSheet("color:#999999;");
   ui->label_proxy_password->setStyleSheet("color:#999999;");
}

void VHNetworkProxy::UIProxyDisable() {
   ui->btn_testing->setEnabled(false);
   ui->ip->setEnabled(false);
   ui->port->setEnabled(false);
   ui->name->setEnabled(false);
   ui->pwd->setEnabled(false);
   
   ui->label_proxy_port->setStyleSheet("color:rgb(76,76,76);");
   ui->label_proxy_ip->setStyleSheet("color:rgb(76,76,76);");
   ui->label_proxy_name->setStyleSheet("color:rgb(76,76,76);");
   ui->label_proxy_password->setStyleSheet("color:rgb(76,76,76);");
}

void VHNetworkProxy::on_comboBox_proxyEnable_currentIndexChanged(int index){
   if (index == NoProxyConfig) {
      UIProxyDisable();
   }
   else {
      UIProxyEnable();
      ui->name->clear();
      ui->ip->clear();
      ui->pwd->clear();
      ui->port->setText("80");
      if (index == ProxyConfig_Browser) {       
         QString ip,userName,userPwd;
         int port;
         HttpProxyInfo::GetProxyInfoFromBrowser(ip,port,userName,userPwd);
         if (!ip.isEmpty()) {
            ui->ip->setText(ip);
            ui->port->setText(QString::number(port));
            ui->name->setText(userName);
            ui->pwd->setText(userPwd);
         }
      }
   }
}

void VHNetworkProxy::on_btn_sure_clicked(){
   bRestart = true;
	QString qsConfPath = CPathManager::GetConfigPath();
   int index = ui->comboBox_proxyEnable->currentIndex();
   ConfigSetting::writeValue(qsConfPath, GROUP_DEFAULT, PROXY_OPEN, index == 0 ? 0 : 1);

   QString ip = ui->ip->text();
   unsigned short port = ui->port->text().toUShort();
   QString userName = ui->name->text();
   QString password = ui->pwd->text();
   int proxyType = ui->comboBox_proxyEnable->currentIndex();

   ConfigSetting::writeValue(qsConfPath, GROUP_DEFAULT, PROXY_HOST, ip);
   ConfigSetting::writeValue(qsConfPath, GROUP_DEFAULT, PROXY_PORT, port);
   ConfigSetting::writeValue(qsConfPath, GROUP_DEFAULT, PROXY_USERNAME, userName);
   ConfigSetting::writeValue(qsConfPath, GROUP_DEFAULT, PROXY_PASSWORD, password);
   ConfigSetting::writeValue(qsConfPath, GROUP_DEFAULT, PROXY_TYPE, proxyType);
   close();
}

void VHNetworkProxy::on_btn_return_clicked() {
   close();
}


