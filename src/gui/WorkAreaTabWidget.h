#ifndef WORKAREATABWIDGET_H
#define WORKAREATABWIDGET_H

#include <QTabWidget>
#include <QList>

class CommandLineTool;
class DeviceConfigurationTool;
class TreeViewConfigTool;
class SimpleFOCDevice;

class WorkAreaTabWidget : public QTabWidget
{
    Q_OBJECT

    public:
    explicit WorkAreaTabWidget(QWidget* parent = nullptr);

    public slots:
    void addDeviceForm();
    void addDeviceTree();
    void openDevice();
    void saveDevice();
    void openConsoleTool();
    void removeTabHandler(int index);

    private:
    SimpleFOCDevice* m_device;
    CommandLineTool* m_cmdLineTool = nullptr;
    QWidget* m_configDeviceTool    = nullptr;
    QList<QWidget*> m_activeToolsList;

    void saveToFile(SimpleFOCDevice* device, const QString& file);
};

#endif
