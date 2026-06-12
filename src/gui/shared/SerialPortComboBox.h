#ifndef SERIALPORTCOMBOBOX_H
#define SERIALPORTCOMBOBOX_H

#include <QComboBox>
#include <QStringList>

class SerialPortComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit SerialPortComboBox(QWidget *parent = nullptr);
    QStringList getAvailableSerialPortNames();

public slots:
    void showPopup() override;
};

#endif
