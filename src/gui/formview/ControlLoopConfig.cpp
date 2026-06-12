#include "ControlLoopConfig.h"
#include "core/SimpleFOCDevice.h"
#include <QVBoxLayout>
#include <QLabel>

ControlLoopConfig::ControlLoopConfig(QWidget *parent)
    : QGroupBox("Control Loop Mode", parent)
    , m_device(SimpleFOCDevice::instance())
{
    auto *layout = new QVBoxLayout(this);

    m_combo = new QComboBox(this);
    m_combo->addItems({"Torque", "Velocity", "Angle", "Velocity openloop", "Angle openloop"});
    layout->addWidget(m_combo);

    connect(m_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ControlLoopConfig::onControlTypeChanged);
    connect(m_device, &SimpleFOCDevice::configurationUpdated,
            this, &ControlLoopConfig::onConfigurationUpdated);
    connect(m_device, &SimpleFOCDevice::connectionStateChanged,
            this, &ControlLoopConfig::onConnectionStateChanged);
}

void ControlLoopConfig::onControlTypeChanged(int index)
{
    if (!m_updating && m_device->getIsConnected())
        m_device->sendControlType(index);
}

void ControlLoopConfig::onConfigurationUpdated()
{
    m_updating = true;
    m_combo->setCurrentIndex(m_device->controlType);
    m_updating = false;
}

void ControlLoopConfig::onConnectionStateChanged(bool connected)
{
    m_combo->setEnabled(connected);
}
