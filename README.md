# SimpleFOCStudio (Qt C++ Edition)

SimpleFOC Configuration Tool 的 Qt C++ 重新实现版本。用于配置和调试基于 [SimpleFOC](https://github.com/simplefoc) 库的无刷电机驱动器。

原版 Python/PyQt5 项目：[SimpleFOCStudio](https://github.com/JorgeMaker/SimpleFOCStudio)

## 功能特性

- **双视图模式**：Form View（表单视图）和 Tree View（树形视图）
- **实时监控**：7 通道实时数据绘图（Target、Vq、Vd、Cq、Cd、Velocity、Angle）
- **PID 在线调参**：Velocity / Angle / Current Q / Current D 四组 PID 参数实时调整
- **串口通信**：支持自定义波特率、校验位、数据位、停止位
- **点动控制**：正向/反向快速点动，可配置步进增量
- **Arduino 代码生成**：根据当前参数自动生成 SimpleFOC 初始化代码
- **设备配置文件**：JSON 格式的设备参数保存/加载
- **自定义命令**：支持用户自定义串口命令（树形视图）
- **命令行工具**：独立的串口终端工具

## 依赖

- **C++17** 编译器（GCC 10+ / Clang 14+）
- **CMake** 3.16+
- **Qt 6.x**（需要以下模块）：
  - Qt6::Core
  - Qt6::Gui / Qt6::Widgets
  - Qt6::SerialPort
  - Qt6::Charts

## Ubuntu 22.04 安装与编译

### 1. 安装系统依赖

```bash
sudo apt update
sudo apt install -y build-essential cmake git libgl1-mesa-dev libegl1-mesa-dev \
    libxkbcommon-x11-0 libxcb-xinerama0 libxcb-cursor0 libdbus-1-3 \
    libfontconfig1 libfreetype6 libglib2.0-0 libxrender1 libxcb-icccm4 \
    libxcb-image0 libxcb-keysyms1 libxcb-randr0 libxcb-render-util0 \
    libxcb-shape0 libxcb-shm0 libxcb-sync1 libxcb-xfixes0
```

### 2. 安装 Qt6

**方式 A：从源码编译（推荐，可自定义模块）**

```bash
# 下载 Qt 6.9.2 源码
wget https://download.qt.io/archive/qt/6.9/6.9.2/single/qt-everywhere-src-6.9.2.tar.xz
tar xf qt-everywhere-src-6.9.2.tar.xz
cd qt-everywhere-src-6.9.2

# 配置（只编译需要的模块，大幅节省时间）
./configure \
    -release \
    -opensource -confirm-license \
    -skip qt3d -skip qt5compat -skip qtwebengine -skip qtvirtualkeyboard \
    -skip qtquick3d -skip qtquicktimeline -skip qtshadertools \
    -nomake examples -nomake tests \
    -prefix /opt/Qt/6.9.2

# 编译安装（根据 CPU 核心数调整 -j 参数）
cmake --build . --parallel $(nproc)
sudo cmake --install .

cd ..
```

**方式 B：通过 APT 安装（Ubuntu 22.04 自带 Qt 6.2）**

```bash
sudo apt install -y qt6-base-dev qt6-serialport-dev qt6-charts-dev
```

> 注意：方式 B 安装的 Qt 版本较旧（6.2），但功能上兼容本项目。如果使用方式 B，编译时需要移除 CMakeLists.txt 中的 `set(Qt6_DIR ...)` 行，让 CMake 自动查找系统 Qt。

### 3. 获取源码

```bash
git clone <repository-url> SimpleFOCStudio-Qt
cd SimpleFOCStudio-Qt
```

### 4. 编译

```bash
# 创建构建目录
mkdir build && cd build

# 配置（方式 A：源码编译的 Qt）
cmake .. -DQt6_DIR=/opt/Qt/6.9.2/lib/cmake/Qt6

# 或配置（方式 B：APT 安装的 Qt）
cmake ..

# 编译
cmake --build . --parallel $(nproc)
```

### 5. 运行

```bash
# 方式 A 需要设置库路径
LD_LIBRARY_PATH=/opt/Qt/6.9.2/lib:$LD_LIBRARY_PATH ./SimpleFOCStudio

# 方式 B 直接运行
./SimpleFOCStudio
```

## 项目结构

```
SimpleFOCStudio-Qt/
├── CMakeLists.txt              # CMake 构建配置
├── README.md
├── resources/
│   ├── resources.qrc           # Qt 资源文件
│   └── icons/                  # 49 个 PNG 图标
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
    │   │   ├── ConfigureConnectionDialog # 串口配置
    │   │   └── GenerateCodeDialog        # 代码生成选项
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
    │       ├── GeneratedCodeDisplay      # 代码展示
    │       ├── CodeGenerator             # Arduino 代码生成
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
