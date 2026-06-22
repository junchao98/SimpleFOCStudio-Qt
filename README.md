# SimpleFOCStudio (Qt C++ Edition)

SimpleFOC Configuration Tool 的 Qt C++ 重新实现版本。用于配置和调试基于 [SimpleFOC](https://github.com/simplefoc) 库的无刷电机驱动器。

原版 Python/PyQt5 项目：[SimpleFOCStudio](https://github.com/JorgeMaker/SimpleFOCStudio)

## 功能特性

- **双视图模式**：Form View（表单视图）和 Tree View（树形视图）
- **实时监控**：7 通道实时数据绘图（Target、Vq、Vd、Cq、Cd、Velocity、Angle）
- **PID 在线调参**：Velocity / Angle / Current Q / Current D 四组 PID 参数实时调整
- **串口通信**：支持自定义波特率、校验位、数据位、停止位
- **点动控制**：正向/反向快速点动，可配置步进增量
- **设备配置文件**：JSON 格式的设备参数保存/加载
- **自定义命令**：支持用户自定义串口命令（树形视图）
- **命令行工具**：独立的串口终端工具

## 快速安装（Ubuntu 22.04）

从 [Releases](https://github.com/junchao98/SimpleFOCStudio-Qt/releases) 下载最新 deb 包：

```bash
sudo apt install -y libqt6core6 libqt6gui6 libqt6widgets6 libqt6serialport6 libqt6charts6
sudo dpkg -i simplefocstudio_1.0.0_amd64.deb
```

## 从源码编译

### 1. 安装依赖

```bash
sudo apt install -y build-essential cmake git \
    qt6-base-dev libqt6serialport6-dev libqt6charts6-dev
```

### 2. 编译

```bash
git clone https://github.com/junchao98/SimpleFOCStudio-Qt.git
cd SimpleFOCStudio-Qt
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel $(nproc)
```

### 3. 运行

```bash
./SimpleFOCStudio
```

### 4. 构建 deb 包

```bash
./tools/deb/build.sh
```

详见 [tools/deb/](tools/deb/) 目录。

## 项目结构

```
SimpleFOCStudio-Qt/
├── CMakeLists.txt              # CMake 构建配置
├── README.md
├── resources/
│   ├── resources.qrc           # Qt 资源文件
│   └── icons/                  # 49 个 PNG 图标
├── tools/
│   └── deb/                    # deb 打包脚本
│       ├── build.sh            # 一键打包
│       ├── DEBIAN/control      # 包元数据
│       └── simplefocstudio.desktop
└── src/
    ├── main.cpp                # 程序入口
    ├── core/                   # 核心逻辑
    │   ├── SimpleFOCDevice     # 设备单例模型（参数、串口协议、响应解析）
    │   ├── SerialPortHandler   # QThread 串口读取线程
    │   ├── StateUpdateRunner   # 1Hz 状态轮询线程
    │   └── CommandBuilder      # 命令字符串构建工具
    ├── gui/
    │   ├── MainWindow          # 主窗口
    │   ├── WorkAreaTabWidget   # 可关闭/可移动的标签页容器
    │   ├── ToolBar             # 顶部工具栏
    │   ├── shared/             # 共享组件
    │   │   ├── GUIToolKit      # 图标加载工具
    │   │   ├── ConfigLineEdit  # 浮点数验证输入框
    │   │   ├── SerialPortComboBox # 自动检测串口
    │   │   └── CommandLineWidget  # 串口命令行终端
    │   ├── dialogs/            # 对话框
    │   │   └── ConfigureConnectionDialog # 串口配置
    │   ├── formview/           # 表单视图
    │   │   ├── DeviceConfigurationTool   # 主表单标签页
    │   │   ├── DRODisplayWidget          # 4 位数字 LCD 显示
    │   │   ├── ControlLoopConfig         # 控制环路选择
    │   │   ├── TorqueConfig              # 力矩模式选择
    │   │   ├── ConnectionControl         # 连接/断开/拉取参数
    │   │   ├── GraphicWidget             # QtCharts 实时绘图
    │   │   ├── PIDConfiguration          # PID 参数表单
    │   │   ├── GeneralControls           # 启用/归零控制
    │   │   ├── GeneralSettingsWidget     # 限幅/下采样设置
    │   │   └── DeviceJoggingControl      # 点动控制
    │   ├── treeview/           # 树形视图
    │   │   ├── TreeViewConfigTool        # 主树形标签页
    │   │   ├── DeviceTreeWidget          # 参数树（带内联编辑）
    │   │   ├── DevicesInspectorTree      # 左侧面板
    │   │   └── DeviceInteractionFrame    # 右侧面板
    │   └── tools/              # 工具
    │       ├── CommandLineTool           # 独立串口终端
    │       └── JsonDeviceSerializer      # JSON 读写
```

## 串口通信协议

本工具兼容 SimpleFOC Commander 协议，支持以下命令：

| 命令 | 格式 | 说明 |
|------|------|------|
| `C` | `{ID}C{0-4}` | 运动控制模式（力矩/速度/角度/开环） |
| `T` | `{ID}T{0-2}` | 力矩模式（电压/直流电流/FOC电流） |
| `VP/VI/VD` | `{ID}V{P\|I\|D}{值}` | Velocity PID 参数 |
| `AP/AI/AD` | `{ID}A{P\|I\|D}{值}` | Angle PID 参数 |
| `QP/QI/QD` | `{ID}Q{P\|I\|D}{值}` | Current Q PID 参数 |
| `DP/DI/DD` | `{ID}D{P\|I\|D}{值}` | Current D PID 参数 |
| `LV/LU/LC` | `{ID}L{V\|U\|C}{值}` | 速度/电压/电流限幅 |
| `MG0-MG6` | `{ID}MG{0-6}` | 读取状态变量 |
| `MS` | `{ID}MS{7位掩码}` | 设置监控变量 |

其中 `{ID}` 为设备命令 ID，默认为 `M`。

## 许可证

MIT License
