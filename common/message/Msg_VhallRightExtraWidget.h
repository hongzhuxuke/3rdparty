#ifndef _MSG_VHALLRIGHTEXTRAWIDGET_H_
#define _MSG_VHALLRIGHTEXTRAWIDGET_H_

#include "VH_ConstDeff.h"
#include <string>
#include <windows.h>
#define NULL_PAGE    0
#define CHANGE_PAGE  1
#define NEXT_PAGE    2
#define PREV_PAGE    3
using namespace std;

enum MSG_VHALLRIGHTEXTRAWIDGET_DEF {
   MSG_VHALLRIGHTEXTRAWIDGET_CREATE = DEF_RECV_VHALLRIGHETEXTRAWIDGET_BEGIN,		//创建窗口
   MSG_VHALLRIGHTEXTRAWIDGET_ACTIVE,
   //MSG_VHALLRIGHTEXTRAWIDGET_INITUSERINFO,                            //初始化用户信息
   MSG_VHALLRIGHTEXTRAWIDGET_INITCOMMONINFO,                          //初始化通用信息
   //MSG_VHALLRIGHTEXTRAWIDGET_SENDMSG,                                 //发送消息
   MSG_VHALLRIGHTEXTRAWIDGET_GETNEWPAGEONLINELIST,                     //获得新的用户列表

   MSG_VHALLRIGHTEXTRAWIDGET_DO_REFRESH,                              //执行刷新
   MSG_VHALLRIGHTEXTRAWIDGET_END_REFRESH,                             //执行完毕刷新

   //MSG_VHALLRIGHTEXTRAWIDGET_START_SHARE_DESKTOP,                    //开始桌面共享
   MSG_VHALLRIGHTEXTRAWIDGET_STOP_CREATE_RECORD_TIMER,               //停止计时消息

   MSG_VHALLRIGHTEXTRAWIDGET_RIGHT_MOUSE_BUTTON_USER,                //右键选中用户

   MSG_VHALLRIGHTEXTRAWIDGET_RIGHT_SYNC_USERLIST                     //更新用户信息

};
struct STRU_VHALLRIGHTEXTRAWIDGET_INIT_USERINFO {
   char data[4096];
public:
   STRU_VHALLRIGHTEXTRAWIDGET_INIT_USERINFO();
};
struct STRU_VHALLRIGHTEXTRAWIDGET_COMMONINFO {
   char msgUrl[256];
   char msgToken[512];
   char filterurl[512];

   unsigned short chat_port;
   wchar_t chat_srv[256];
   wchar_t chat_url[512];
   unsigned short msg_port;
   wchar_t msg_srv[256];
   wchar_t m_domain[256];

   char proxy_ip[256];
   char proxy_port[16];
   char proxy_username[256];
   char proxy_password[256];
   char domain[256];
   bool forbidchat;
   long roomId;
   bool host;
	bool bStartInteraction;	//是否启动底层消息收发。
public:
   STRU_VHALLRIGHTEXTRAWIDGET_COMMONINFO();
};
#endif //_MSG_VHALLRIGHTEXTRAWIDGET_H_
