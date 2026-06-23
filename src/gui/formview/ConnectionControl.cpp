#include "ConnectionControl.h"
#include "core/SimpleFOCDevice.h"
#include "gui/shared/GUIToolKit.h"
#include "gui/dialogs/ConfigureConnectionDialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

ConnectionControl::ConnectionControl(QWidget* parent)
    : QGroupBox("Connection control", parent), m_device(SimpleFOCDevice::instance())
{
    auto* layout = new QVBoxLayout(this);

    auto* cmdLayout = new QHBoxLayout();
    cmdLayout->addWidget(new QLabel("Command:"));
    m_commandIdEdit = new QLineEdit("M");
    m_commandIdEdit->setMaximumWidth(50);
    connect(m_commandIdEdit, &QLineEdit::textChanged, [this](const QString& t) {
        m_device->devCommandID = t;
    });
    cmdLayout->addWidget(m_commandIdEdit);
    cmdLayout->addStretch();
    layout->addLayout(cmdLayout);

    auto* btnLayout = new QHBoxLayout();

    m_pullBtn = new QPushButton("Pull Params");
    m_pullBtn->setIcon(GUIToolKit::getIconByName("pull"));
    connect(m_pullBtn, &QPushButton::clicked, this, &ConnectionControl::onPullParams);
    btnLayout->addWidget(m_pullBtn);

    m_connectBtn = new QPushButton("Connect");
    m_connectBtn->setIcon(GUIToolKit::getIconByName("connect"));
    connect(m_connectBtn, &QPushButton::clicked, this, &ConnectionControl::onConnect);
    btnLayout->addWidget(m_connectBtn);

    m_disconnectBtn = new QPushButton("Disconnect");
    m_disconnectBtn->setIcon(GUIToolKit::getIconByName("disconnect"));
    m_disconnectBtn->setEnabled(false);
    connect(m_disconnectBtn, &QPushButton::clicked, this, &ConnectionControl::onDisconnect);
    btnLayout->addWidget(m_disconnectBtn);

    m_configureBtn = new QPushButton("Configure");
    m_configureBtn->setIcon(GUIToolKit::getIconByName("configure"));
    connect(m_configureBtn, &QPushButton::clicked, this, &ConnectionControl::onConfigureClicked);
    btnLayout->addWidget(m_configureBtn);

    layout->addLayout(btnLayout);

    connect(m_device,
            &SimpleFOCDevice::connectionStateChanged,
            this,
            &ConnectionControl::onConnectionStateChanged);
}

void ConnectionControl::onConnect() { m_device->connectDevice(SimpleFOCDevice::PULL_CONFIG); }

void ConnectionControl::onDisconnect() { m_device->disconnectDevice(); }

void ConnectionControl::onPullParams()
{
    if(m_device->getIsConnected())
        m_device->pullConfiguration();
}

void ConnectionControl::onConnectionStateChanged(bool connected)
{
    m_connectBtn->setEnabled(!connected);
    m_disconnectBtn->setEnabled(connected);
    m_pullBtn->setEnabled(connected);
}

void ConnectionControl::onConfigureClicked()
{
    emit configureClicked();

    ConfigureConnectionDialog dlg(this);
    dlg.setPortName(m_device->serialPortName);
    dlg.setBaudRate(m_device->serialRate);
    dlg.setParity(m_device->serialParity);
    dlg.setByteSize(m_device->serialByteSize);
    dlg.setStopBits(m_device->stopBits);

    if(dlg.exec() == QDialog::Accepted)
    {
        QVariantMap config;
        config["serialPortName"] = dlg.portName();
        config["serialRate"]     = dlg.baudRate();
        config["serialByteSize"] = dlg.byteSize();
        config["serialParity"]   = dlg.parity();
        config["stopBits"]       = dlg.stopBits();
        m_device->configureConnection(config);
    }
}
