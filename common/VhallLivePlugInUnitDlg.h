#ifndef VHALLWEBVIEWFREE_H
#define VHALLWEBVIEWFREE_H

#include <QDialog>
#include <QPoint>
#include "cbasedlg.h"

class TitleButton;
class QWebEngineView;
class QWebChannel;

namespace Ui {
	class VhallLivePlugInUnitDlg;
}

class VhallLivePlugInUnitDlg : public CBaseDlg
{
    Q_OBJECT

public:
    explicit VhallLivePlugInUnitDlg(QWidget *parent = 0);
    ~VhallLivePlugInUnitDlg();
    void SetWindowTitle(QString);
    void InitPluginUrl(QString url,QObject *obj);
    bool Create();
    void Destory();
    void CenterWindow(QWidget* parent);
    void Load(QString url,QObject *obj);
    void executeJSCode(QString);
    void ReloadPluginUrl();
    bool IsLoadUrlFinished();

signals:
    void SigJsCode(QString);
    void SigClose();
protected:
    bool eventFilter(QObject *, QEvent *);    
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);

private:
   void AddWebEngineView();
   

public slots:
    void loadFinished(bool);    
    void SlotJsCode(QString);
    void SlotMaxClicked();
    void SlotMinClicked();
    void SlotRefresh();
    void SlotReLoad();
    void JsCallQtDebug(QString msg);
private slots:
   void SlotClose();
private:
	 Ui::VhallLivePlugInUnitDlg *ui;
    QPoint m_PressPoint;
    QPoint m_StartPoint;
    TitleButton *m_pBtnRefresh = NULL;
    TitleButton *m_pBtnMin = NULL;
    TitleButton *m_pBtnMax = NULL;  
    TitleButton *m_pBtnClose = NULL;

    QObject *m_obj= NULL;
    bool m_bInit = false;
    //bool m_bFirstShow = true;
    QRect m_lastGeometry;
    bool mIsLoadUrlFinished = true;
    QRect m_startRect;
    QPoint m_startPoint;

    QWebEngineView *m_pWebEngineView = NULL;
    QWebChannel* m_pWebChannel = NULL;
    QString m_pluginUrl;   
    QString mStreamID;
    bool mFirstLoad = true;
};

#endif // VHALLWEBVIEWFREE_H
