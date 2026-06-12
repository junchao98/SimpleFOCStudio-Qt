#ifndef DEVICETREEWIDGET_H
#define DEVICETREEWIDGET_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "core/SimpleFOCDevice.h"
#include "gui/shared/ConfigLineEdit.h"

class SimpleFOCDevice;

class DeviceTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit DeviceTreeWidget(QWidget *parent = nullptr);
    void buildTree();

public slots:
    void onConfigurationUpdated();
    void onConnectionStateChanged(bool connected);

private:
    void addMotionConfig(QTreeWidgetItem *parent);
    void addPIDGroup(QTreeWidgetItem *parent, const QString &name,
                     PIDController &pid, LowPassFilter &lpf);
    void addLimits(QTreeWidgetItem *parent);
    void addSensorConfig(QTreeWidgetItem *parent);
    void addGeneralSettings(QTreeWidgetItem *parent);
    void addCustomCommands(QTreeWidgetItem *parent);
    void addStates(QTreeWidgetItem *parent);

    QTreeWidgetItem *createEditableItem(QTreeWidgetItem *parent, const QString &label,
                                         const QString &value, const QString &propertyKey);
    QTreeWidgetItem *createComboItem(QTreeWidgetItem *parent, const QString &label,
                                      const QStringList &options, int currentIndex,
                                      const QString &propertyKey);

    SimpleFOCDevice *m_device;
    bool m_updating = false;
};

#endif
