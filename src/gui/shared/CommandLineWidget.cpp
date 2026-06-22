#include "CommandLineWidget.h"
#include "gui/shared/GUIToolKit.h"
#include "core/SimpleFOCDevice.h"
#include <QTextCursor>
#include <QTextDocument>

CommandLineWidget::CommandLineWidget(QWidget* parent, bool filterMonitoringData)
    : QGroupBox("Command Line interface", parent), m_device(SimpleFOCDevice::instance()), m_filterMonitoringData(filterMonitoringData)
{
    auto* mainLayout = new QVBoxLayout(this);

    m_textDisplay = new QTextEdit();
    m_textDisplay->setReadOnly(true);
    mainLayout->addWidget(m_textDisplay, 1);

    auto* inputLayout = new QHBoxLayout();
    m_commandInput    = new QLineEdit();
    m_commandInput->setPlaceholderText("Command:");
    inputLayout->addWidget(m_commandInput);

    m_sendBtn = new QPushButton();
    m_sendBtn->setIcon(GUIToolKit::getIconByName("send"));
    m_sendBtn->setToolTip("Send");
    connect(m_sendBtn, &QPushButton::clicked, this, &CommandLineWidget::onSend);
    inputLayout->addWidget(m_sendBtn);

    m_clearBtn = new QPushButton();
    m_clearBtn->setIcon(GUIToolKit::getIconByName("delete"));
    m_clearBtn->setToolTip("Clear");
    connect(m_clearBtn, &QPushButton::clicked, this, &CommandLineWidget::onClear);
    inputLayout->addWidget(m_clearBtn);

    m_listDevicesBtn = new QPushButton();
    m_listDevicesBtn->setIcon(GUIToolKit::getIconByName("list"));
    m_listDevicesBtn->setToolTip("List Devices");
    connect(m_listDevicesBtn, &QPushButton::clicked, this, &CommandLineWidget::onListDevices);
    inputLayout->addWidget(m_listDevicesBtn);

    mainLayout->addLayout(inputLayout);

    connect(m_commandInput, &QLineEdit::returnPressed, this, &CommandLineWidget::onSend);

    // Connect to appropriate signal based on filter mode
    if(m_filterMonitoringData) {
        // In Device tab: only show command responses, not monitoring data
        connect(m_device, &SimpleFOCDevice::commandDataReceived,
                this, &CommandLineWidget::onRawDataReceived);
    } else {
        // In Command Line tab: show all data
        connect(m_device, &SimpleFOCDevice::rawDataReceived,
                this, &CommandLineWidget::onRawDataReceived);
    }
}

void CommandLineWidget::appendText(const QString& text) { m_textDisplay->append(text); }

void CommandLineWidget::clearText() { m_textDisplay->clear(); }

void CommandLineWidget::onSend()
{
    QString cmd = m_commandInput->text().trimmed();
    if(!cmd.isEmpty())
    {
        m_device->sendCommand(cmd);
        m_commandInput->clear();
    }
}

void CommandLineWidget::onClear() { clearText(); }

void CommandLineWidget::onListDevices() { m_device->sendListDevices(); }

void CommandLineWidget::onRawDataReceived(const QString& data)
{
    // Limit text display to prevent memory issues
    QTextDocument* doc = m_textDisplay->document();
    if(doc->lineCount() > MAX_LINES) {
        // Remove old lines to limit memory usage
        QTextCursor cursor(doc);
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        cursor.deleteChar(); // Remove newline
    }
    appendText(data);
}
