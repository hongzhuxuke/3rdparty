#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>
class QListWidget;
class ComboBox : public QComboBox
{
	Q_OBJECT

public:
   ComboBox(QWidget *parent);
   ~ComboBox();

private:
	QListWidget * mpListWidget = NULL;
};

#endif // COMBOBOX_H