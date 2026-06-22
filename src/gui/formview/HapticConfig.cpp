#include "HapticConfig.h"
#include "core/SimpleFOCDevice.h"
#include <QVBoxLayout>

HapticConfig::HapticConfig(QWidget* parent)
    : QGroupBox("Haptic Mode", parent), m_device(SimpleFOCDevice::instance())
{
    auto* layout = new QVBoxLayout(this);
    m_combo      = new QComboBox(this);
    m_combo->addItems({"Unbounded",
                       "Bounded 0-10",
                       "Multi-revolution",
                       "On-Off",
                       "Spring",
                       "Fine (no detent)",
                       "Fine detent",
                       "Coarse strong",
                       "Coarse weak",
                       "Magnetic",
                       "Spring with detent"});
    layout->addWidget(m_combo);

    connect(m_combo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &HapticConfig::onHapticPresetChanged);
    connect(m_device,
            &SimpleFOCDevice::configurationUpdated,
            this,
            &HapticConfig::onConfigurationUpdated);
    connect(m_device,
            &SimpleFOCDevice::connectionStateChanged,
            this,
            &HapticConfig::onConnectionStateChanged);
}

void HapticConfig::onHapticPresetChanged(int index)
{
    if(!m_updating && m_device->getIsConnected())
        m_device->sendHapticPreset(index);
}

void HapticConfig::onConfigurationUpdated()
{
    m_updating = true;
    m_combo->setCurrentIndex(m_device->hapticPreset);
    m_updating = false;
}

void HapticConfig::onConnectionStateChanged(bool connected) { m_combo->setEnabled(connected); }
