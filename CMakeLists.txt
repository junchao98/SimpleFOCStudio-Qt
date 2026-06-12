cmake_minimum_required(VERSION 3.16)
project(SimpleFOCStudio LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets SerialPort Charts)

set(SOURCES
    src/main.cpp
    src/core/SimpleFOCDevice.cpp
    src/core/SerialPortHandler.cpp
    src/core/StateUpdateRunner.cpp
    src/core/CommandBuilder.cpp
    src/gui/shared/GUIToolKit.cpp
    src/gui/shared/ConfigLineEdit.cpp
    src/gui/shared/SerialPortComboBox.cpp
    src/gui/shared/CommandLineWidget.cpp
    src/gui/dialogs/ConfigureConnectionDialog.cpp
    src/gui/dialogs/GenerateCodeDialog.cpp
    src/gui/formview/DRODisplayWidget.cpp
    src/gui/formview/ControlLoopConfig.cpp
    src/gui/formview/TorqueConfig.cpp
    src/gui/formview/ConnectionControl.cpp
    src/gui/formview/GraphicWidget.cpp
    src/gui/formview/PIDConfiguration.cpp
    src/gui/formview/GeneralControls.cpp
    src/gui/formview/GeneralSettingsWidget.cpp
    src/gui/formview/DeviceJoggingControl.cpp
    src/gui/formview/DeviceConfigurationTool.cpp
    src/gui/treeview/DeviceTreeWidget.cpp
    src/gui/treeview/DevicesInspectorTree.cpp
    src/gui/treeview/DeviceInteractionFrame.cpp
    src/gui/treeview/TreeViewConfigTool.cpp
    src/gui/tools/CommandLineTool.cpp
    src/gui/tools/ConfigureConnectionWidget.cpp
    src/gui/tools/GeneratedCodeDisplay.cpp
    src/gui/tools/CodeGenerator.cpp
    src/gui/tools/JsonDeviceSerializer.cpp
    src/gui/MainWindow.cpp
    src/gui/WorkAreaTabWidget.cpp
    src/gui/ToolBar.cpp
)

set(HEADERS
    src/core/SimpleFOCDevice.h
    src/core/SerialPortHandler.h
    src/core/StateUpdateRunner.h
    src/core/CommandBuilder.h
    src/gui/shared/GUIToolKit.h
    src/gui/shared/ConfigLineEdit.h
    src/gui/shared/SerialPortComboBox.h
    src/gui/shared/CommandLineWidget.h
    src/gui/dialogs/ConfigureConnectionDialog.h
    src/gui/dialogs/GenerateCodeDialog.h
    src/gui/formview/DRODisplayWidget.h
    src/gui/formview/ControlLoopConfig.h
    src/gui/formview/TorqueConfig.h
    src/gui/formview/ConnectionControl.h
    src/gui/formview/GraphicWidget.h
    src/gui/formview/PIDConfiguration.h
    src/gui/formview/GeneralControls.h
    src/gui/formview/GeneralSettingsWidget.h
    src/gui/formview/DeviceJoggingControl.h
    src/gui/formview/DeviceConfigurationTool.h
    src/gui/treeview/DeviceTreeWidget.h
    src/gui/treeview/DevicesInspectorTree.h
    src/gui/treeview/DeviceInteractionFrame.h
    src/gui/treeview/TreeViewConfigTool.h
    src/gui/tools/CommandLineTool.h
    src/gui/tools/ConfigureConnectionWidget.h
    src/gui/tools/GeneratedCodeDisplay.h
    src/gui/tools/CodeGenerator.h
    src/gui/tools/JsonDeviceSerializer.h
    src/gui/MainWindow.h
    src/gui/WorkAreaTabWidget.h
    src/gui/ToolBar.h
)

qt_add_executable(SimpleFOCStudio
    ${SOURCES}
    ${HEADERS}
    resources/resources.qrc
)

target_include_directories(SimpleFOCStudio PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(SimpleFOCStudio PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::SerialPort
    Qt6::Charts
)
