#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <QWidget>
class QLabel;
class PushButton;

class TitleWidget : public QWidget {
   Q_OBJECT
public:
   explicit TitleWidget(QString titleName, QWidget *parent = 0);
   ~TitleWidget();
   void SetMoveEnable(bool bEnable);
   void SetTitle(QString titleName);

signals:
   void closeWidget();

protected:
   void mousePressEvent(QMouseEvent *);
   void mouseMoveEvent(QMouseEvent *);
   void mouseReleaseEvent(QMouseEvent *);
   void paintEvent(QPaintEvent *);

private:
   QPoint mPressPoint;
   bool mIsMoved;

   QLabel *mVersionTitle = NULL;
   PushButton *mBtnClose = NULL;
   bool m_bIsEnableMove = true;
public:
   void hideCloseBtn();
};

#endif // TITLEWIDGET_H
