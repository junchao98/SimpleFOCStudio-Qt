#include "ConfigLineEdit.h"
#include <QKeyEvent>

ConfigLineEdit::ConfigLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    setMaximumWidth(100);
    setValidator(new QDoubleValidator(this));
}

void ConfigLineEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        emit updateValue();
    } else {
        QLineEdit::keyPressEvent(event);
    }
}
