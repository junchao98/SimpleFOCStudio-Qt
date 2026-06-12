#include "PIDConfiguration.h"
#include "core/SimpleFOCDevice.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QThread>

PIDConfiguration::PIDConfiguration(QWidget *parent)
    : QGroupBox("PID Controller configuration", parent)
    , m_device(SimpleFOCDevice::instance())
{
    auto *layout = new QVBoxLayout(this);

    auto *selectorLayout = new QHBoxLayout();
    selectorLayout->addWidget(new QLabel("Select PID:"));
    m_pidSelector = new QComboBox(this);
    m_pidSelector->addItems({"Velocity", "Angle", "Current Q", "Current D"});
    selectorLayout->addWidget(m_pidSelector);
    selectorLayout->addStretch();
    layout->addLayout(selectorLayout);

    auto *formLayout = new QFormLayout();

    m_pEdit = new ConfigLineEdit(this);
    formLayout->addRow("Proportional gain", m_pEdit);

    m_iEdit = new ConfigLineEdit(this);
    formLayout->addRow("Integral gain", m_iEdit);

    m_dEdit = new ConfigLineEdit(this);
    formLayout->addRow("Derivative gain", m_dEdit);

    m_rampEdit = new ConfigLineEdit(this);
    formLayout->addRow("Output ramp", m_rampEdit);

    m_lpfEdit = new ConfigLineEdit(this);
    formLayout->addRow("Low pass filter", m_lpfEdit);

    layout->addLayout(formLayout);

    connect(m_pidSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PIDConfiguration::onPIDSelected);
    connectFieldSignals();

    connect(m_device, &SimpleFOCDevice::configurationUpdated,
            this, &PIDConfiguration::onConfigurationUpdated);
    connect(m_device, &SimpleFOCDevice::connectionStateChanged,
            this, &PIDConfiguration::onConnectionStateChanged);
}

void PIDConfiguration::onPIDSelected(int index)
{
    if (m_device->getIsConnected()) {
        PIDController *pid = nullptr;
        LowPassFilter *lpf = nullptr;
        switch (index) {
        case 0: pid = &m_device->PIDVelocity; lpf = &m_device->LPFVelocity; break;
        case 1: pid = &m_device->PIDAngle; lpf = &m_device->LPFAngle; break;
        case 2: pid = &m_device->PIDCurrentQ; lpf = &m_device->LPFCurrentQ; break;
        case 3: pid = &m_device->PIDCurrentD; lpf = &m_device->LPFCurrentD; break;
        }

        if (pid && m_device->getIsConnected()) {
            m_device->sendProportionalGain(*pid, 0);
            QThread::msleep(5);
            m_device->sendIntegralGain(*pid, 0);
            QThread::msleep(5);
            m_device->sendDerivativeGain(*pid, 0);
            QThread::msleep(5);
            m_device->sendOutputRamp(*pid, 0);
            QThread::msleep(5);
            m_device->sendLowPassFilter(*lpf, 0);
        }
    }
    updateFields();
}

void PIDConfiguration::updateFields()
{
    m_updating = true;
    PIDController *pid = nullptr;
    LowPassFilter *lpf = nullptr;
    switch (m_pidSelector->currentIndex()) {
    case 0: pid = &m_device->PIDVelocity; lpf = &m_device->LPFVelocity; break;
    case 1: pid = &m_device->PIDAngle; lpf = &m_device->LPFAngle; break;
    case 2: pid = &m_device->PIDCurrentQ; lpf = &m_device->LPFCurrentQ; break;
    case 3: pid = &m_device->PIDCurrentD; lpf = &m_device->LPFCurrentD; break;
    }
    if (pid) {
        m_pEdit->setText(QString::number(pid->P));
        m_iEdit->setText(QString::number(pid->I));
        m_dEdit->setText(QString::number(pid->D));
        m_rampEdit->setText(QString::number(pid->outputRamp));
        m_lpfEdit->setText(QString::number(lpf->Tf));
    }
    m_updating = false;
}

void PIDConfiguration::connectFieldSignals()
{
    connect(m_pEdit, &ConfigLineEdit::updateValue, [this]() {
        if (m_updating || !m_device->getIsConnected()) return;
        int idx = m_pidSelector->currentIndex();
        PIDController *pid = nullptr;
        switch (idx) {
        case 0: pid = &m_device->PIDVelocity; break;
        case 1: pid = &m_device->PIDAngle; break;
        case 2: pid = &m_device->PIDCurrentQ; break;
        case 3: pid = &m_device->PIDCurrentD; break;
        }
        if (pid) m_device->sendProportionalGain(*pid, m_pEdit->text().toDouble());
    });

    connect(m_iEdit, &ConfigLineEdit::updateValue, [this]() {
        if (m_updating || !m_device->getIsConnected()) return;
        int idx = m_pidSelector->currentIndex();
        PIDController *pid = nullptr;
        switch (idx) {
        case 0: pid = &m_device->PIDVelocity; break;
        case 1: pid = &m_device->PIDAngle; break;
        case 2: pid = &m_device->PIDCurrentQ; break;
        case 3: pid = &m_device->PIDCurrentD; break;
        }
        if (pid) m_device->sendIntegralGain(*pid, m_iEdit->text().toDouble());
    });

    connect(m_dEdit, &ConfigLineEdit::updateValue, [this]() {
        if (m_updating || !m_device->getIsConnected()) return;
        int idx = m_pidSelector->currentIndex();
        PIDController *pid = nullptr;
        switch (idx) {
        case 0: pid = &m_device->PIDVelocity; break;
        case 1: pid = &m_device->PIDAngle; break;
        case 2: pid = &m_device->PIDCurrentQ; break;
        case 3: pid = &m_device->PIDCurrentD; break;
        }
        if (pid) m_device->sendDerivativeGain(*pid, m_dEdit->text().toDouble());
    });

    connect(m_rampEdit, &ConfigLineEdit::updateValue, [this]() {
        if (m_updating || !m_device->getIsConnected()) return;
        int idx = m_pidSelector->currentIndex();
        PIDController *pid = nullptr;
        switch (idx) {
        case 0: pid = &m_device->PIDVelocity; break;
        case 1: pid = &m_device->PIDAngle; break;
        case 2: pid = &m_device->PIDCurrentQ; break;
        case 3: pid = &m_device->PIDCurrentD; break;
        }
        if (pid) m_device->sendOutputRamp(*pid, m_rampEdit->text().toDouble());
    });

    connect(m_lpfEdit, &ConfigLineEdit::updateValue, [this]() {
        if (m_updating || !m_device->getIsConnected()) return;
        int idx = m_pidSelector->currentIndex();
        LowPassFilter *lpf = nullptr;
        switch (idx) {
        case 0: lpf = &m_device->LPFVelocity; break;
        case 1: lpf = &m_device->LPFAngle; break;
        case 2: lpf = &m_device->LPFCurrentQ; break;
        case 3: lpf = &m_device->LPFCurrentD; break;
        }
        if (lpf) m_device->sendLowPassFilter(*lpf, m_lpfEdit->text().toDouble());
    });
}

void PIDConfiguration::onConfigurationUpdated()
{
    updateFields();
}

void PIDConfiguration::onConnectionStateChanged(bool connected)
{
    setEnabled(connected);
}
