#ifndef ALERTDLG_H
#define ALERTDLG_H

#include <QDialog>
#include "cbasedlg.h"

class TitleWidget;
class QLabel;
class QCheckBox;
class AlertDlg : public CBaseDlg
{
   Q_OBJECT

public:
   AlertDlg(QString message, bool isEnableCancel, QWidget *parent = 0);
   AlertDlg(QString message, QString optionCbxText, bool isEnableCancel, QWidget *parent = 0);
   ~AlertDlg();
   void SetTitle(QString title);
   void CenterWindow(QWidget* parent);
   void SetYesBtnText(QString);
   void SetNoBtnText(QString txt);
   void SetTipsText(const QString& strmsg);
private:
   TitleWidget *mTitleBar = NULL;
   QPushButton *mYesBtn = NULL;
   QPushButton *mNoBtn = NULL;
   QLabel *mMessageLbl = NULL;
   QCheckBox *mOptionCbx = nullptr;

private slots:
   void Slot_Accept();
   void Slot_Reject();
public:
   void setOptCbxState(bool checked);
   bool getOptCbxState();
protected:
   void paintEvent(QPaintEvent *);
   //bool eventFilter(QObject *obj, QEvent *ev);
};

#endif // ALERTDLG_H
