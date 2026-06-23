#include "CommandLineTool.h"
#include "gui/shared/CommandLineWidget.h"
#include "gui/shared/GUIToolKit.h"
#include "core/SimpleFOCDevice.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "gui/shared/SerialPortComboBox.h"
#include <QLineEdit>
#include <QComboBox>

CommandLineTool::CommandLineTool(QWidget* parent)
    : QWidget(parent), m_device(SimpleFOCDevice::instance())
{
    auto* layout = new QVBoxLayout(this);

    auto* configGroup  = new QGroupBox("Configure serial connection", this);
    auto* configLayout = new QHBoxLayout(configGroup);

    configLayout->addWidget(new QLabel("Port Name:"));
    auto* portCombo = new SerialPortComboBox(this);
    configLayout->addWidget(portCombo);

    configLayout->addWidget(new QLabel("Bit rate:"));
    auto* baudEdit = new QLineEdit("115200", this);
    configLayout->addWidget(baudEdit);

    configLayout->addWidget(new QLabel("Parity:"));
    auto* parityCombo = new QComboBox(this);
    parityCombo->addItems({"None", "Even", "Odd"});
    configLayout->addWidget(parityCombo);

    configLayout->addWidget(new QLabel("Byte size:"));
    auto* byteCombo = new QComboBox(this);
    byteCombo->addItems({"5", "6", "7", "8"});
    byteCombo->setCurrentText("8");
    configLayout->addWidget(byteCombo);

    configLayout->addWidget(new QLabel("Stop bits:"));
    auto* stopCombo = new QComboBox(this);
    stopCombo->addItems({"1", "1.5", "2"});
    configLayout->addWidget(stopCombo);

    auto* connectBtn = new QPushButton("Connect", this);
    configLayout->addWidget(connectBtn);

    connect(connectBtn,
            &QPushButton::clicked,
            [this, portCombo, baudEdit, parityCombo, byteCombo, stopCombo]() {
                QVariantMap config;
                config["serialPortName"] = portCombo->currentText();
                config["serialRate"]     = baudEdit->text().toInt();
                config["serialParity"]   = parityCombo->currentText().startsWith('E')   ? "E"
                                           : parityCombo->currentText().startsWith('O') ? "O"
                                                                                        : "N";
                config["serialByteSize"] = byteCombo->currentText().toInt();
                config["stopBits"]       = stopCombo->currentText().toDouble();
                m_device->configureConnection(config);
                m_device->connectDevice(SimpleFOCDevice::ONLY_CONNECT);
            });

    layout->addWidget(configGroup);

    m_commandLine = new CommandLineWidget(this);
    layout->addWidget(m_commandLine, 1);
}

QIcon CommandLineTool::getTabIcon() const { return GUIToolKit::getIconByName("consoletool"); }
