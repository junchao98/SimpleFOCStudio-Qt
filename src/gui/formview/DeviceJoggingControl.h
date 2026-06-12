#ifndef DEVICEJOGGINGCONTROL_H
#define DEVICEJOGGINGCONTROL_H

#include <QGroupBox>
#include <QPushButton>
#include "gui/shared/ConfigLineEdit.h"

class SimpleFOCDevice;

class DeviceJoggingControl : public QGroupBox
{
    Q_OBJECT

public:
    explicit DeviceJoggingControl(QWidget *parent = nullptr);

private slots:
    void onFastBackward();
    void onBackward();
    void onStop();
    void onForward();
    void onFastForward();
    void onConnectionStateChanged(bool connected);

private:
    SimpleFOCDevice *m_device;
    ConfigLineEdit *m_incrementEdit;
    double increment() const;
};

#endif
