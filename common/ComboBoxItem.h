#ifndef COMBOBOXITEM_H
#define COMBOBOXITEM_H

#include <QWidget>

class QLabel;
class QHBoxLayout;
class ComboBoxItem : public QWidget
{
	Q_OBJECT

public:
   ComboBoxItem(QWidget *parent);
   ~ComboBoxItem();

protected:
   //Êó±êÀë¿ª
   virtual void leaveEvent(QEvent * event);

private:
   QLabel* m_pImg = NULL;
   QLabel* m_pText = NULL;
   QHBoxLayout* m_pLayout;
};

#endif // COMBOBOXITEM_H