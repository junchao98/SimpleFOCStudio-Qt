#include "TorqueConfig.h"
#include "core/SimpleFOCDevice.h"
#include <QVBoxLayout>

TorqueConfig::TorqueConfig(QWidget *parent)
    : QGroupBox("Torque Mode", parent)
    , m_device(SimpleFOCDevice::instance())
{
    auto *layout = new QVBoxLayout(this);
    m_combo = new QComboBox(this);
    m_combo->addItems({"Voltage", "DC Current", "FOC Current"});
    layout->addWidget(m_combo);

    connect(m_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TorqueConfig::onTorqueTypeChanged);
    connect(m_device, &SimpleFOCDevice::configurationUpdated,
            this, &TorqueConfig::onConfigurationUpdated);
    connect(m_device, &SimpleFOCDevice::connectionStateChanged,
            this, &TorqueConfig::onConnectionStateChanged);
}

void TorqueConfig::onTorqueTypeChanged(int index)
{
    if (!m_updating && m_device->getIsConnected())
        m_device->sendTorqueType(index);
}

void TorqueConfig::onConfigurationUpdated()
{
    m_updating = true;
    m_combo->setCurrentIndex(m_device->torqueType);
    m_updating = false;
}

void TorqueConfig::onConnectionStateChanged(bool connected)
{
    m_combo->setEnabled(connected);
}
