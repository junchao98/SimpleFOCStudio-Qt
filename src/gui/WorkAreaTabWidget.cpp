#include "WorkAreaTabWidget.h"
#include "gui/formview/DeviceConfigurationTool.h"
#include "gui/treeview/TreeViewConfigTool.h"
#include "gui/tools/CommandLineTool.h"
#include "gui/tools/GeneratedCodeDisplay.h"
#include "gui/tools/JsonDeviceSerializer.h"
#include "gui/dialogs/GenerateCodeDialog.h"
#include "gui/tools/CodeGenerator.h"
#include "gui/shared/GUIToolKit.h"
#include "core/SimpleFOCDevice.h"
#include <QFileDialog>
#include <QMessageBox>

WorkAreaTabWidget::WorkAreaTabWidget(QWidget *parent)
    : QTabWidget(parent)
    , m_device(SimpleFOCDevice::instance())
{
    setTabsClosable(true);
    setMovable(true);
    setStyleSheet("QTabBar::tab { height: 30px; width: 150px; }");

    connect(this, &QTabWidget::tabCloseRequested, this, &WorkAreaTabWidget::removeTabHandler);
}

void WorkAreaTabWidget::addDeviceForm()
{
    if (m_configDeviceTool) return;
    auto *tool = new DeviceConfigurationTool(this);
    m_configDeviceTool = tool;
    m_activeToolsList.append(tool);
    addTab(tool, tool->getTabIcon(), tool->getTabName());
    setCurrentIndex(count() - 1);
}

void WorkAreaTabWidget::addDeviceTree()
{
    if (m_configDeviceTool) return;
    auto *tool = new TreeViewConfigTool(this);
    m_configDeviceTool = tool;
    m_activeToolsList.append(tool);
    addTab(tool, tool->getTabIcon(), tool->getTabName());
    setCurrentIndex(count() - 1);
}

void WorkAreaTabWidget::openDevice()
{
    if (m_configDeviceTool) return;
    QString fileName = QFileDialog::getOpenFileName(this, "Open Device Configuration",
                                                     "", "JSON Files (*.json)");
    if (fileName.isEmpty()) return;

    if (JsonDeviceSerializer::loadFromFile(m_device, fileName)) {
        auto *tool = new TreeViewConfigTool(this);
        m_configDeviceTool = tool;
        m_activeToolsList.append(tool);
        addTab(tool, tool->getTabIcon(), tool->getTabName());
        setCurrentIndex(count() - 1);
    } else {
        QMessageBox::warning(this, "SimpleFOC ConfigTool",
                             "Error while opening selected file");
    }
}

void WorkAreaTabWidget::saveDevice()
{
    if (m_activeToolsList.isEmpty()) return;
    int idx = currentIndex();
    if (idx < 0) return;

    if (m_device->openedFile.isEmpty()) {
        QString fileName = QFileDialog::getSaveFileName(this, "Save device configuration",
                                                         "", "JSON configuration file (*.json)");
        if (fileName.isEmpty()) return;
        saveToFile(m_device, fileName);
    } else {
        saveToFile(m_device, m_device->openedFile);
    }
}

void WorkAreaTabWidget::generateCode()
{
    if (m_activeToolsList.isEmpty()) return;

    GenerateCodeDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        auto params = dlg.selectedParameters();
        QString code = CodeGenerator::generateSetupPrefix();
        code += m_device->toArduinoCode(params);
        code += CodeGenerator::generateSetupSuffix();

        m_generatedCodeTab = new GeneratedCodeDisplay(this);
        m_generatedCodeTab->setCode(code);
        m_activeToolsList.append(m_generatedCodeTab);
        addTab(m_generatedCodeTab, m_generatedCodeTab->getTabIcon(), m_generatedCodeTab->getTabName());
        setCurrentIndex(count() - 1);
    }
}

void WorkAreaTabWidget::openConsoleTool()
{
    if (m_cmdLineTool) return;
    m_cmdLineTool = new CommandLineTool(this);
    m_activeToolsList.append(m_cmdLineTool);
    addTab(m_cmdLineTool, m_cmdLineTool->getTabIcon(), m_cmdLineTool->getTabName());
    setCurrentIndex(count() - 1);
}

void WorkAreaTabWidget::removeTabHandler(int index)
{
    QWidget *w = widget(index);
    if (qobject_cast<CommandLineTool*>(w)) {
        m_cmdLineTool = nullptr;
    }
    if (qobject_cast<DeviceConfigurationTool*>(w) || qobject_cast<TreeViewConfigTool*>(w)) {
        m_configDeviceTool = nullptr;
        if (m_device->getIsConnected())
            m_device->disconnectDevice();
    }
    if (qobject_cast<GeneratedCodeDisplay*>(w)) {
        m_generatedCodeTab = nullptr;
    }

    m_activeToolsList.removeAt(index);
    removeTab(index);
}

void WorkAreaTabWidget::saveToFile(SimpleFOCDevice *device, const QString &file)
{
    if (!JsonDeviceSerializer::saveToFile(device, file)) {
        QMessageBox::warning(this, "SimpleFOC ConfigTool", "Error saving file");
    }
}
