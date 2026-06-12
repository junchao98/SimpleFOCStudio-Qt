#include "DeviceConfigurationTool.h"
#include "DRODisplayWidget.h"
#include "ControlLoopConfig.h"
#include "TorqueConfig.h"
#include "ConnectionControl.h"
#include "GraphicWidget.h"
#include "PIDConfiguration.h"
#include "GeneralControls.h"
#include "GeneralSettingsWidget.h"
#include "gui/shared/CommandLineWidget.h"
#include "DeviceJoggingControl.h"
#include "gui/shared/GUIToolKit.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>

DeviceConfigurationTool::DeviceConfigurationTool(QWidget *parent)
    : QWidget(parent)
    , m_device(SimpleFOCDevice::instance())
{
    auto *mainLayout = new QVBoxLayout(this);

    auto *topRow = new QHBoxLayout();

    m_dro = new DRODisplayWidget(this);
    topRow->addWidget(m_dro);

    m_controlLoop = new ControlLoopConfig(this);
    topRow->addWidget(m_controlLoop);

    m_torqueConfig = new TorqueConfig(this);
    topRow->addWidget(m_torqueConfig);

    m_connectionControl = new ConnectionControl(this);
    topRow->addWidget(m_connectionControl);

    mainLayout->addLayout(topRow);

    m_graphicWidget = new GraphicWidget(this);
    mainLayout->addWidget(m_graphicWidget, 1);

    auto *bottomRow = new QHBoxLayout();

    m_pidConfig = new PIDConfiguration(this);
    bottomRow->addWidget(m_pidConfig, 2);

    auto *middleCol = new QVBoxLayout();
    m_generalControls = new GeneralControls(this);
    middleCol->addWidget(m_generalControls);
    m_generalSettings = new GeneralSettingsWidget(this);
    middleCol->addWidget(m_generalSettings);
    bottomRow->addLayout(middleCol, 2);

    auto *rightCol = new QVBoxLayout();
    m_commandLine = new CommandLineWidget(this);
    rightCol->addWidget(m_commandLine);
    m_joggingControl = new DeviceJoggingControl(this);
    rightCol->addWidget(m_joggingControl);
    bottomRow->addLayout(rightCol, 3);

    mainLayout->addLayout(bottomRow);
}

QIcon DeviceConfigurationTool::getTabIcon() const
{
    return GUIToolKit::getIconByName("motor");
}
