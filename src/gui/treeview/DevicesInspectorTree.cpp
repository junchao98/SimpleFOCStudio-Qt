#include "DevicesInspectorTree.h"
#include "gui/formview/DRODisplayWidget.h"
#include "gui/formview/GeneralControls.h"
#include "gui/formview/ConnectionControl.h"
#include "gui/formview/DeviceJoggingControl.h"
#include "DeviceTreeWidget.h"
#include "core/SimpleFOCDevice.h"

DevicesInspectorTree::DevicesInspectorTree(QWidget *parent)
    : QWidget(parent)
    , m_device(SimpleFOCDevice::instance())
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_dro = new DRODisplayWidget(this);
    layout->addWidget(m_dro);

    m_generalControls = new GeneralControls(this);
    layout->addWidget(m_generalControls);

    m_treeWidget = new DeviceTreeWidget(this);
    layout->addWidget(m_treeWidget, 1);

    m_joggingControl = new DeviceJoggingControl(this);
    layout->addWidget(m_joggingControl);

    m_connectionControl = new ConnectionControl(this);
    layout->addWidget(m_connectionControl);
}
