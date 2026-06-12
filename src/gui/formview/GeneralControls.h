#ifndef GENERALCONTROLS_H
#define GENERALCONTROLS_H

#include <QGroupBox>
#include <QPushButton>

class SimpleFOCDevice;

class GeneralControls : public QGroupBox
{
    Q_OBJECT

public:
    explicit GeneralControls(QWidget *parent = nullptr);

private slots:
    void onEnableDisable();
    void onSensorZero();
    void onZeroTarget();
    void onConnectionStateChanged(bool connected);
    void onConfigurationUpdated();

private:
    SimpleFOCDevice *m_device;
    QPushButton *m_enableBtn;
    QPushButton *m_sensorZeroBtn;
    QPushButton *m_zeroTargetBtn;
};

#endif
