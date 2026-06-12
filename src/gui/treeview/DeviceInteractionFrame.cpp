#include "DeviceInteractionFrame.h"
#include "gui/formview/GraphicWidget.h"
#include "gui/shared/CommandLineWidget.h"
#include "core/SimpleFOCDevice.h"

DeviceInteractionFrame::DeviceInteractionFrame(QWidget *parent)
    : QWidget(parent)
    , m_device(SimpleFOCDevice::instance())
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_graphicWidget = new GraphicWidget(this);
    layout->addWidget(m_graphicWidget, 1);

    m_commandLine = new CommandLineWidget(this);
    layout->addWidget(m_commandLine);
}
