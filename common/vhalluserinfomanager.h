#ifndef VHALLUSERINFOMANAGER_H
#define VHALLUSERINFOMANAGER_H
//#include <QMap>
#include <QSet>
#include <QList>
#include <windows.h>
#include "IInteractionClient.h"

#include <QObject>
#include <QDebug>
#define USER_HOST "host"
#define USER_GUEST "guest"
#define USER_ASSISTANT "assistant"
#define USER_USER "user"
typedef enum VhallShowType {
   VhallShowType_Allow,     //œ‘ æ
   VhallShowType_Prohibit,
   VhallShowType_Hide
}VhallShowType;

typedef struct VhallAudienceUserInfo_st {
	VhallAudienceUserInfo_st();
	VhallAudienceUserInfo_st(const UserInfo &uInfo);
	VhallAudienceUserInfo_st(const VhallAudienceUserInfo_st &uInfo);
	bool operator < (const VhallAudienceUserInfo_st other);

	bool operator == (const VhallAudienceUserInfo_st other);
   
   QString userId;
   QString userName;
   QString role;
   
   VhallShowType gagType = VhallShowType_Allow;
   VhallShowType kickType = VhallShowType_Allow;

   int miUserCount;
}VhallAudienceUserInfo;

typedef struct VhallAudienceUserInfoList_st {
	bool Has(QString qid);
	bool Has(wchar_t *id);
	bool GetUserInfo(const UserInfo &userInfo, VhallAudienceUserInfo &uInfo);   
	bool pushBack(const VhallAudienceUserInfo &info);
	bool append(const VhallAudienceUserInfo &info);
	bool Remove(const VhallAudienceUserInfo &info);

	VhallAudienceUserInfo & operator [](int index);


	void clear();
	
   int count();
   void Init(UserList &userList,QString defaultRole,VhallShowType gagType,VhallShowType kickType);
   void Debug();
   QSet<QString> mUserIdList;
   QList<VhallAudienceUserInfo> mInfoList;
   VhallAudienceUserInfo mEmpty;
}VhallAudienceUserInfoList;


#endif // VHALLUSERINFOMANAGER_H
