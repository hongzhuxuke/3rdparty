#include "CLineEditInput.h"

CLineEditInput::CLineEditInput(QWidget *parent)
    : QLineEdit(parent)
{
}

CLineEditInput::~CLineEditInput()
{
}

void CLineEditInput::focusInEvent(QFocusEvent *e) {
    emit sig_focusInEvent();
    QLineEdit::focusInEvent(e);
}
