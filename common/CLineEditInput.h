#pragma once

#include <QObject>
#include <QLineEdit>

class CLineEditInput : public QLineEdit
{
    Q_OBJECT

public:
    CLineEditInput(QWidget *parent);
    ~CLineEditInput();

protected:
    void focusInEvent(QFocusEvent *) Q_DECL_OVERRIDE;

signals:
    void sig_focusInEvent();
};
