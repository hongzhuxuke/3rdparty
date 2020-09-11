#ifndef _H_PUBSTRUCT_H_
#define _H_PUBSTRUCT_H_

#include "VH_ConstDeff.h"
#include <QString>
#include <QDateTime>
#include <QMap>

struct STRU_MAINUI_LOG{
	WCHAR m_wzRequestUrl[DEF_MAX_HTTP_URL_LEN + 1];            //«Î«ÛURLµÿ÷∑   
	char m_wzRequestJson[DEF_MAX_HTTP_URL_LEN + 1]; 
public:
	STRU_MAINUI_LOG();


};

typedef struct VhallUserInfo_st {
	QString id;
	QString name;
	QString role;
}VhallUserInfo;

#endif //_H_PUBCONST_H_