#ifndef TREEVIEWCONFIGTOOL_H
#define TREEVIEWCONFIGTOOL_H

#include <QWidget>
#include <QIcon>
#include "core/SimpleFOCDevice.h"

class DevicesInspectorTree;
class DeviceInteractionFrame;

class TreeViewConfigTool : public QWidget
{
    Q_OBJECT

public:
    explicit TreeViewConfigTool(QWidget *parent = nullptr);
    QIcon getTabIcon() const;
    QString getTabName() const { return "Device"; }

private:
    SimpleFOCDevice *m_device;
    DevicesInspectorTree *m_inspector;
    DeviceInteractionFrame *m_interactionFrame;
};

#endif
