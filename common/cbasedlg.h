#ifndef CBASEDLG_H
#define CBASEDLG_H

#include <QObject>
#include <QDialog>
#include "vhalluserinfomanager.h"

class CBaseDlg : public QDialog
{
   Q_OBJECT

public:
   CBaseDlg(QWidget *parent = NULL);
    ~CBaseDlg();

protected:
   virtual void keyPressEvent(QKeyEvent *event);

private:
    
};

class CAliveDlg : public CBaseDlg
{
	Q_OBJECT

public:
	CAliveDlg(QWidget *parent = NULL);
	~CAliveDlg();
	virtual bool  GetIsLiving() = 0;
	virtual QString GetJoinId() = 0;
	virtual QString GetJoinRole() = 0;
	virtual QString GetHostId() = 0;
	virtual QString GetStreamName() = 0;
	virtual bool IsExistRenderWnd(const QString& uid) = 0;
protected:
	QString AnalysisOnOffLine(const QString& param, VhallAudienceUserInfo* Info);
	void AnalysisLoginInfo(const QString& strInfo);

	QString mstrAvatar ;
	QString mstrAvatar_httpurl ;
	QString mstrNick_name;
	QString mstrUser_id ;
	QString mstrRole ;
};
#endif // CBASEDLG_H
