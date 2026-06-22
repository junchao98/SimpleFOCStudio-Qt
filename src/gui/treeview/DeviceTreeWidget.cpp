#include "DeviceTreeWidget.h"
#include "gui/shared/GUIToolKit.h"
#include <QComboBox>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QMessageBox>

DeviceTreeWidget::DeviceTreeWidget(QWidget* parent)
    : QTreeWidget(parent), m_device(SimpleFOCDevice::instance())
{
    setHeaderLabels({"Property", "Value"});
    header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    setAlternatingRowColors(true);
    setAnimated(true);

    buildTree();

    connect(m_device,
            &SimpleFOCDevice::configurationUpdated,
            this,
            &DeviceTreeWidget::onConfigurationUpdated);
    connect(m_device,
            &SimpleFOCDevice::connectionStateChanged,
            this,
            &DeviceTreeWidget::onConnectionStateChanged);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTreeWidget::customContextMenuRequested, [this](const QPoint& pos) {
        QTreeWidgetItem* item = itemAt(pos);
        if(item && item->data(0, Qt::UserRole).toString() == "custom_command")
        {
            QMenu menu;
            QAction* addAct    = menu.addAction("Add command");
            QAction* removeAct = menu.addAction("Remove command");
            QAction* execAct   = menu.addAction("Execute command");

            QAction* chosen = menu.exec(mapToGlobal(pos));
            if(chosen == addAct)
            {
                CustomCommand cmd("New Command", "");
                m_device->customCommands.append(cmd);
                buildTree();
            }
            else if(chosen == removeAct)
            {
                int idx = item->data(1, Qt::UserRole).toInt();
                if(idx >= 0 && idx < m_device->customCommands.size())
                {
                    m_device->customCommands.removeAt(idx);
                    buildTree();
                }
            }
            else if(chosen == execAct)
            {
                int idx = item->data(1, Qt::UserRole).toInt();
                if(idx >= 0 && idx < m_device->customCommands.size() && m_device->getIsConnected())
                {
                    m_device->sendCommand(m_device->devCommandID +
                                          m_device->customCommands[idx].cmd);
                }
            }
        }
    });
}

void DeviceTreeWidget::buildTree()
{
    clear();
    QTreeWidgetItem* root = new QTreeWidgetItem(this, {"sFOC Device"});
    root->setIcon(0, GUIToolKit::getIconByName("motor"));
    root->setExpanded(true);

    addMotionConfig(root);
    addPIDGroup(root, "Velocity PID", m_device->PIDVelocity, m_device->LPFVelocity);
    addPIDGroup(root, "Angle PID", m_device->PIDAngle, m_device->LPFAngle);
    addPIDGroup(root, "Current q PID", m_device->PIDCurrentQ, m_device->LPFCurrentQ);
    addPIDGroup(root, "Current d PID", m_device->PIDCurrentD, m_device->LPFCurrentD);
    addLimits(root);
    addSensorConfig(root);
    addGeneralSettings(root);
    addCustomCommands(root);
    addStates(root);

    expandAll();
}

void DeviceTreeWidget::addMotionConfig(QTreeWidgetItem* parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, {"Motion config"});
    item->setIcon(0, GUIToolKit::getIconByName("gear"));
    item->setExpanded(true);

    createComboItem(
        item,
        "Motion Control Type",
        {"Torque", "Velocity", "Angle", "Velocity openloop", "Angle openloop", "Haptic"},
        m_device->controlType,
        "controlType");
    createComboItem(item,
                    "Torque Control Type",
                    {"Voltage", "DC Current", "FOC Current"},
                    m_device->torqueType,
                    "torqueType");
    if(m_device->controlType == SimpleFOCDevice::HAPTIC_CONTROL)
    {
        createComboItem(item,
                        "Haptic Preset",
                        {"Unbounded",
                         "Bounded 0-10",
                         "Multi-revolution",
                         "On-Off",
                         "Spring",
                         "Fine (no detent)",
                         "Fine detent",
                         "Coarse strong",
                         "Coarse weak",
                         "Magnetic",
                         "Spring with detent"},
                        m_device->hapticPreset,
                        "hapticPreset");
    }
    createEditableItem(
        item, "Motion Downsample", QString::number(m_device->motionDownsample), "motionDownsample");
}

void DeviceTreeWidget::addPIDGroup(QTreeWidgetItem* parent,
                                   const QString& name,
                                   PIDController& pid,
                                   LowPassFilter& lpf)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, {name});
    item->setIcon(0, GUIToolKit::getIconByName("pidconfig"));
    item->setExpanded(false);

    createEditableItem(item, "Proportional gain", QString::number(pid.P), "pid_P_" + pid.cmd);
    createEditableItem(item, "Integral gain", QString::number(pid.I), "pid_I_" + pid.cmd);
    createEditableItem(item, "Derivative gain", QString::number(pid.D), "pid_D_" + pid.cmd);
    createEditableItem(item, "Output Ramp", QString::number(pid.outputRamp), "pid_R_" + pid.cmd);
    createEditableItem(item, "Output Limit", QString::number(pid.outputLimit), "pid_L_" + pid.cmd);
    createEditableItem(item, "Low pass filter", QString::number(lpf.Tf), "pid_F_" + pid.cmd);
}

void DeviceTreeWidget::addLimits(QTreeWidgetItem* parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, {"Limits"});
    item->setIcon(0, GUIToolKit::getIconByName("statistics"));
    item->setExpanded(false);

    createEditableItem(
        item, "Velocity Limit", QString::number(m_device->velocityLimit), "velocityLimit");
    createEditableItem(
        item, "Voltage Limit", QString::number(m_device->voltageLimit), "voltageLimit");
    createEditableItem(
        item, "Current Limit", QString::number(m_device->currentLimit), "currentLimit");
}

void DeviceTreeWidget::addSensorConfig(QTreeWidgetItem* parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, {"Sensor config"});
    item->setIcon(0, GUIToolKit::getIconByName("sensor"));
    item->setExpanded(false);

    createEditableItem(item,
                       "Electrical Zero",
                       QString::number(m_device->sensorElectricalZero),
                       "sensorElectricalZero");
    createEditableItem(
        item, "Zero Offset", QString::number(m_device->sensorZeroOffset), "sensorZeroOffset");
}

void DeviceTreeWidget::addGeneralSettings(QTreeWidgetItem* parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, {"General settings"});
    item->setIcon(0, GUIToolKit::getIconByName("generalsettings"));
    item->setExpanded(false);

    createEditableItem(
        item, "Phase Resistance", QString::number(m_device->phaseResistance), "phaseResistance");
}

void DeviceTreeWidget::addCustomCommands(QTreeWidgetItem* parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, {"Custom commands"});
    item->setIcon(0, GUIToolKit::getIconByName("customcommands"));
    item->setData(0, Qt::UserRole, "custom_commands");
    item->setExpanded(false);

    for(int i = 0; i < m_device->customCommands.size(); ++i)
    {
        QTreeWidgetItem* cmdItem = new QTreeWidgetItem(
            item, {m_device->customCommands[i].cmdName, m_device->customCommands[i].cmd});
        cmdItem->setIcon(0, GUIToolKit::getIconByName("gear"));
        cmdItem->setData(0, Qt::UserRole, "custom_command");
        cmdItem->setData(1, Qt::UserRole, i);
        cmdItem->setFlags(cmdItem->flags() | Qt::ItemIsEditable);
    }
}

void DeviceTreeWidget::addStates(QTreeWidgetItem* parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, {"States"});
    item->setIcon(0, GUIToolKit::getIconByName("statistics"));
    item->setExpanded(false);

    new QTreeWidgetItem(item, {"Target", QString::number(m_device->targetNow)});
    new QTreeWidgetItem(item, {"Vq", QString::number(m_device->voltageQNow)});
    new QTreeWidgetItem(item, {"Vd", QString::number(m_device->voltageDNow)});
    new QTreeWidgetItem(item, {"Cq", QString::number(m_device->currentQNow)});
    new QTreeWidgetItem(item, {"Cd", QString::number(m_device->currentDNow)});
    new QTreeWidgetItem(item, {"Velocity", QString::number(m_device->velocityNow)});
    new QTreeWidgetItem(item, {"Angle", QString::number(m_device->angleNow)});
}

QTreeWidgetItem* DeviceTreeWidget::createEditableItem(QTreeWidgetItem* parent,
                                                      const QString& label,
                                                      const QString& value,
                                                      const QString& propertyKey)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, {label, value});
    item->setData(0, Qt::UserRole, propertyKey);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    return item;
}

QTreeWidgetItem* DeviceTreeWidget::createComboItem(QTreeWidgetItem* parent,
                                                   const QString& label,
                                                   const QStringList& options,
                                                   int currentIndex,
                                                   const QString& propertyKey)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, {label, options.value(currentIndex, "")});
    item->setData(0, Qt::UserRole, propertyKey);

    auto* combo = new QComboBox(this);
    combo->addItems(options);
    combo->setCurrentIndex(currentIndex);
    connect(
        combo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, propertyKey](int idx) {
            if(m_updating || !m_device->getIsConnected())
                return;
            if(propertyKey == "controlType")
                m_device->sendControlType(idx);
            else if(propertyKey == "torqueType")
                m_device->sendTorqueType(idx);
            else if(propertyKey == "hapticPreset")
                m_device->sendHapticPreset(idx);
        });
    setItemWidget(item, 1, combo);
    return item;
}

void DeviceTreeWidget::onConfigurationUpdated()
{
    m_updating = true;
    buildTree();
    m_updating = false;
}

void DeviceTreeWidget::onConnectionStateChanged(bool connected) { Q_UNUSED(connected); }
