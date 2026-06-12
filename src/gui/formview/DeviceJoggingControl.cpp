#include "DeviceJoggingControl.h"
#include "core/SimpleFOCDevice.h"
#include "gui/shared/GUIToolKit.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

DeviceJoggingControl::DeviceJoggingControl(QWidget *parent)
    : QGroupBox("Jogging control", parent)
    , m_device(SimpleFOCDevice::instance())
{
    auto *layout = new QVBoxLayout(this);

    auto *btnLayout = new QHBoxLayout();

    auto *fastBackBtn = new QPushButton();
    fastBackBtn->setIcon(GUIToolKit::getIconByName("fastbackward"));
    fastBackBtn->setToolTip("Fast Backward");
    connect(fastBackBtn, &QPushButton::clicked, this, &DeviceJoggingControl::onFastBackward);
    btnLayout->addWidget(fastBackBtn);

    auto *backBtn = new QPushButton();
    backBtn->setIcon(GUIToolKit::getIconByName("backward"));
    backBtn->setToolTip("Backward");
    connect(backBtn, &QPushButton::clicked, this, &DeviceJoggingControl::onBackward);
    btnLayout->addWidget(backBtn);

    auto *stopBtn = new QPushButton();
    stopBtn->setIcon(GUIToolKit::getIconByName("stopjogging"));
    stopBtn->setToolTip("Stop");
    connect(stopBtn, &QPushButton::clicked, this, &DeviceJoggingControl::onStop);
    btnLayout->addWidget(stopBtn);

    auto *fwdBtn = new QPushButton();
    fwdBtn->setIcon(GUIToolKit::getIconByName("fordward"));
    fwdBtn->setToolTip("Forward");
    connect(fwdBtn, &QPushButton::clicked, this, &DeviceJoggingControl::onForward);
    btnLayout->addWidget(fwdBtn);

    auto *fastFwdBtn = new QPushButton();
    fastFwdBtn->setIcon(GUIToolKit::getIconByName("fastfordward"));
    fastFwdBtn->setToolTip("Fast Forward");
    connect(fastFwdBtn, &QPushButton::clicked, this, &DeviceJoggingControl::onFastForward);
    btnLayout->addWidget(fastFwdBtn);

    layout->addLayout(btnLayout);

    auto *incLayout = new QHBoxLayout();
    incLayout->addWidget(new QLabel("Increment:"));
    m_incrementEdit = new ConfigLineEdit(this);
    m_incrementEdit->setText("1.0");
    incLayout->addWidget(m_incrementEdit);
    layout->addLayout(incLayout);

    connect(m_device, &SimpleFOCDevice::connectionStateChanged,
            this, &DeviceJoggingControl::onConnectionStateChanged);
}

double DeviceJoggingControl::increment() const
{
    return m_incrementEdit->text().toDouble();
}

void DeviceJoggingControl::onFastBackward()
{
    if (m_device->getIsConnected())
        m_device->sendTargetValue(m_device->target - 2 * increment());
}

void DeviceJoggingControl::onBackward()
{
    if (m_device->getIsConnected())
        m_device->sendTargetValue(m_device->target - increment());
}

void DeviceJoggingControl::onStop()
{
    if (!m_device->getIsConnected()) return;
    if (m_device->controlType == SimpleFOCDevice::ANGLE_CONTROL)
        m_device->sendTargetValue(m_device->angleNow);
    else
        m_device->sendTargetValue(0);
}

void DeviceJoggingControl::onForward()
{
    if (m_device->getIsConnected())
        m_device->sendTargetValue(m_device->target + increment());
}

void DeviceJoggingControl::onFastForward()
{
    if (m_device->getIsConnected())
        m_device->sendTargetValue(m_device->target + 2 * increment());
}

void DeviceJoggingControl::onConnectionStateChanged(bool connected)
{
    setEnabled(connected);
}
