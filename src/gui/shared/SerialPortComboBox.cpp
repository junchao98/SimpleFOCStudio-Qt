#include "SerialPortComboBox.h"
#include <QSerialPortInfo>

SerialPortComboBox::SerialPortComboBox(QWidget *parent)
    : QComboBox(parent)
{
    addItems(getAvailableSerialPortNames());
}

QStringList SerialPortComboBox::getAvailableSerialPortNames()
{
    QStringList names;
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port : ports) {
        if (!port.description().isEmpty())
            names.append(port.portName());
    }
    return names;
}

void SerialPortComboBox::showPopup()
{
    QString selected = currentText();
    clear();
    QStringList ports = getAvailableSerialPortNames();
    addItems(ports);
    int idx = ports.indexOf(selected);
    if (idx >= 0) setCurrentIndex(idx);
    QComboBox::showPopup();
}
