#include "GeneralSettingsWidget.h"
#include "core/SimpleFOCDevice.h"
#include <QFormLayout>

GeneralSettingsWidget::GeneralSettingsWidget(QWidget *parent)
    : QGroupBox("General Settings", parent)
    , m_device(SimpleFOCDevice::instance())
{
    auto *layout = new QFormLayout(this);

    m_motionDownsample = new ConfigLineEdit(this);
    layout->addRow("Motion Downsample", m_motionDownsample);

    m_currentLimit = new ConfigLineEdit(this);
    layout->addRow("Current Limit", m_currentLimit);

    m_velocityLimit = new ConfigLineEdit(this);
    layout->addRow("Velocity Limit", m_velocityLimit);

    m_voltageLimit = new ConfigLineEdit(this);
    layout->addRow("Voltage Limit", m_voltageLimit);

    connect(m_motionDownsample, &ConfigLineEdit::updateValue, [this]() {
        if (!m_updating && m_device->getIsConnected())
            m_device->sendMotionDownsample(m_motionDownsample->text().toDouble());
    });
    connect(m_currentLimit, &ConfigLineEdit::updateValue, [this]() {
        if (!m_updating && m_device->getIsConnected())
            m_device->sendCurrentLimit(m_currentLimit->text().toDouble());
    });
    connect(m_velocityLimit, &ConfigLineEdit::updateValue, [this]() {
        if (!m_updating && m_device->getIsConnected())
            m_device->sendVelocityLimit(m_velocityLimit->text().toDouble());
    });
    connect(m_voltageLimit, &ConfigLineEdit::updateValue, [this]() {
        if (!m_updating && m_device->getIsConnected())
            m_device->sendVoltageLimit(m_voltageLimit->text().toDouble());
    });

    connect(m_device, &SimpleFOCDevice::connectionStateChanged,
            this, &GeneralSettingsWidget::onConnectionStateChanged);
    connect(m_device, &SimpleFOCDevice::configurationUpdated,
            this, &GeneralSettingsWidget::onConfigurationUpdated);
}

void GeneralSettingsWidget::onConnectionStateChanged(bool connected)
{
    setEnabled(connected);
}

void GeneralSettingsWidget::onConfigurationUpdated()
{
    m_updating = true;
    m_motionDownsample->setText(QString::number(m_device->motionDownsample));
    m_currentLimit->setText(QString::number(m_device->currentLimit));
    m_velocityLimit->setText(QString::number(m_device->velocityLimit));
    m_voltageLimit->setText(QString::number(m_device->voltageLimit));
    m_updating = false;
}
