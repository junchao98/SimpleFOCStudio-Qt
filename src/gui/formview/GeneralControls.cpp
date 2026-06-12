#include "GeneralControls.h"
#include "core/SimpleFOCDevice.h"
#include "gui/shared/GUIToolKit.h"
#include <QHBoxLayout>

GeneralControls::GeneralControls(QWidget *parent)
    : QGroupBox("General Controls", parent)
    , m_device(SimpleFOCDevice::instance())
{
    auto *layout = new QHBoxLayout(this);

    m_enableBtn = new QPushButton("Disable Device");
    m_enableBtn->setIcon(GUIToolKit::getIconByName("stop"));
    connect(m_enableBtn, &QPushButton::clicked, this, &GeneralControls::onEnableDisable);
    layout->addWidget(m_enableBtn);

    m_sensorZeroBtn = new QPushButton("Sensor Zero");
    m_sensorZeroBtn->setIcon(GUIToolKit::getIconByName("home"));
    connect(m_sensorZeroBtn, &QPushButton::clicked, this, &GeneralControls::onSensorZero);
    layout->addWidget(m_sensorZeroBtn);

    m_zeroTargetBtn = new QPushButton("Zero Target");
    m_zeroTargetBtn->setIcon(GUIToolKit::getIconByName("stop"));
    connect(m_zeroTargetBtn, &QPushButton::clicked, this, &GeneralControls::onZeroTarget);
    layout->addWidget(m_zeroTargetBtn);

    connect(m_device, &SimpleFOCDevice::connectionStateChanged,
            this, &GeneralControls::onConnectionStateChanged);
    connect(m_device, &SimpleFOCDevice::configurationUpdated,
            this, &GeneralControls::onConfigurationUpdated);
}

void GeneralControls::onEnableDisable()
{
    if (!m_device->getIsConnected()) return;
    if (m_device->deviceStatus == 1) {
        m_device->sendDeviceStatus(0);
    } else {
        m_device->sendDeviceStatus(1);
    }
}

void GeneralControls::onSensorZero()
{
    if (m_device->getIsConnected())
        m_device->sendSensorZeroOffset(0);
}

void GeneralControls::onZeroTarget()
{
    if (m_device->getIsConnected())
        m_device->sendTargetValue(0);
}

void GeneralControls::onConnectionStateChanged(bool connected)
{
    setEnabled(connected);
}

void GeneralControls::onConfigurationUpdated()
{
    if (m_device->deviceStatus == 1) {
        m_enableBtn->setText("Disable Device");
    } else {
        m_enableBtn->setText("Enable Device");
    }
}
