#ifndef VHALLHORLISTWIDGET_H
#define VHALLHORLISTWIDGET_H

#include <QWidget>
#include <QMap>
#include <string>
namespace Ui {
class VhallHorListWidget;
}

#define VhallHorListWidgetKey QString

class VhallHorListWidget : public QWidget
{
    Q_OBJECT

public:
   explicit VhallHorListWidget(QWidget *parent = 0);
   ~VhallHorListWidget();
   void Append(VhallHorListWidgetKey &,QWidget *);
   bool CheckRendExist(QWidget* w);
   void Remove(VhallHorListWidgetKey &);
   QWidget *GetRenderWidget(VhallHorListWidgetKey &key);
   QList<QString> GetRenderMembers();
   void Clear();
   void RefreshOnlineList();
   void ChangeCenterRender(QString showUser, QString hideUser);
   int GetRenderCount();
   const QMap<QString, QWidget*> GetRenderWidgetsMap() const;
signals:
   void SigClicked(QWidget *);
protected:
   bool eventFilter(QObject *o, QEvent *e);
private:
   Ui::VhallHorListWidget *ui;
   QMap<VhallHorListWidgetKey,QWidget *> m_pWidgets;
   
};

#endif // VHALLHORLISTWIDGET_H
