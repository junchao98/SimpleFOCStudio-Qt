#ifndef CONFIGLINEEDIT_H
#define CONFIGLINEEDIT_H

#include <QLineEdit>
#include <QDoubleValidator>

class ConfigLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit ConfigLineEdit(QWidget *parent = nullptr);

signals:
    void updateValue();

protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif
