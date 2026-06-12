#include "TreeViewConfigTool.h"
#include "DevicesInspectorTree.h"
#include "DeviceInteractionFrame.h"
#include "gui/shared/GUIToolKit.h"
#include <QSplitter>
#include <QHBoxLayout>

TreeViewConfigTool::TreeViewConfigTool(QWidget *parent)
    : QWidget(parent)
    , m_device(SimpleFOCDevice::instance())
{
    auto *layout = new QHBoxLayout(this);

    auto *splitter = new QSplitter(Qt::Horizontal, this);

    m_inspector = new DevicesInspectorTree(this);
    splitter->addWidget(m_inspector);

    m_interactionFrame = new DeviceInteractionFrame(this);
    splitter->addWidget(m_interactionFrame);

    splitter->setSizes({400, 800});
    layout->addWidget(splitter);
}

QIcon TreeViewConfigTool::getTabIcon() const
{
    return GUIToolKit::getIconByName("motor");
}
