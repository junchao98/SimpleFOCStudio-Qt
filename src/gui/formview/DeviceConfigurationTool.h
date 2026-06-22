#ifndef DEVICECONFIGURATIONTOOL_H
#define DEVICECONFIGURATIONTOOL_H

#include <QWidget>
#include <QIcon>
#include "core/SimpleFOCDevice.h"

class DRODisplayWidget;
class ControlLoopConfig;
class TorqueConfig;
class HapticConfig;
class ConnectionControl;
class GraphicWidget;
class PIDConfiguration;
class GeneralControls;
class GeneralSettingsWidget;
class CommandLineWidget;
class DeviceJoggingControl;

class DeviceConfigurationTool : public QWidget
{
    Q_OBJECT

    public:
    explicit DeviceConfigurationTool(QWidget* parent = nullptr);
    QIcon getTabIcon() const;
    QString getTabName() const { return "Device"; }

    private slots:
    void onConfigurationUpdated();

    private:
    SimpleFOCDevice* m_device;
    DRODisplayWidget* m_dro;
    ControlLoopConfig* m_controlLoop;
    TorqueConfig* m_torqueConfig;
    HapticConfig* m_hapticConfig;
    ConnectionControl* m_connectionControl;
    GraphicWidget* m_graphicWidget;
    PIDConfiguration* m_pidConfig;
    GeneralControls* m_generalControls;
    GeneralSettingsWidget* m_generalSettings;
    CommandLineWidget* m_commandLine;
    DeviceJoggingControl* m_joggingControl;
};

#endif
