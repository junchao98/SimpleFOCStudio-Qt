#ifndef DEVICESINSPECTORTREE_H
#define DEVICESINSPECTORTREE_H

#include <QWidget>
#include <QVBoxLayout>

class DRODisplayWidget;
class GeneralControls;
class DeviceTreeWidget;
class DeviceJoggingControl;
class ConnectionControl;
class SimpleFOCDevice;

class DevicesInspectorTree : public QWidget
{
    Q_OBJECT

public:
    explicit DevicesInspectorTree(QWidget *parent = nullptr);

private:
    SimpleFOCDevice *m_device;
    DRODisplayWidget *m_dro;
    GeneralControls *m_generalControls;
    DeviceTreeWidget *m_treeWidget;
    DeviceJoggingControl *m_joggingControl;
    ConnectionControl *m_connectionControl;
};

#endif
