# AGENTS.md

## 构建

```bash
# Ubuntu (apt Qt6)
sudo apt install -y build-essential cmake qt6-base-dev libqt6serialport6-dev libqt6charts6-dev
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel $(nproc)

# 运行
./SimpleFOCStudio
```

Linux 输出 `build/SimpleFOCStudio`，Windows 输出 `build/Release/SimpleFOCStudio.exe`。

## 代码格式

根目录有 `.clang-format`。提交前对改动文件运行 `clang-format -i <file>`。关键设置：4 空格缩进，行宽 100，Allman 大括号风格，指针靠左，`SortIncludes: false`。

## 架构

- **入口**: `src/main.cpp` → `MainWindow`
- **核心** (`src/core/`): `SimpleFOCDevice`（单例数据模型）、`SerialPortHandler`（QThread 串口读取）、`StateUpdateRunner`（1 Hz 轮询线程）、`CommandBuilder`（协议命令构建）
- **GUI** (`src/gui/`): `MainWindow` 持有 `WorkAreaTabWidget` + `ToolBar`，两种主视图：
  - `formview/` — 表单视图（`DeviceConfigurationTool` 为主标签页容器）
  - `treeview/` — 树形视图（`TreeViewConfigTool`）
- **工具** (`src/gui/tools/`): 串口终端、JSON 设备序列化
- **资源**: `resources/resources.qrc` 嵌入 `resources/icons/` 中的 PNG 图标

GUI 类均为 Qt Designer 风格的 `.h/.cpp` 配对，CMake 启用了 `AUTOMOC`/`AUTORCC`/`AUTOUIC`。新增 `.cpp`/`.h` 文件必须同时加入 `CMakeLists.txt` 的 `SOURCES` 和 `HEADERS`。

## CI / 发布

单一 workflow `.github/workflows/build-windows.yml`（虽然名字叫 build-windows，实际构建所有平台）：
- **Windows**: Qt 6.8.3 MSVC 2022 + `windeployqt` → zip 产物
- **Ubuntu 22.04/24.04**: Qt 6.8.3 + `dpkg-deb` → `.deb` 产物

版本号硬编码在两处：`tools/deb/DEBIAN/control` 和 CI workflow 的 `Build deb package` 步骤中。升版本时需同时修改。

## 本地 deb 打包

```bash
./tools/deb/build.sh
```

在项目根目录生成 `simplefocstudio_<version>_amd64.deb`。依赖系统 Qt6（apt 安装），与 CI 中 `install-qt-action` 安装的 Qt 不兼容。

## 约定

- 无测试套件，通过编译和运行验证
- Qt6 必需组件：`Core`、`Gui`、`Widgets`、`SerialPort`、`Charts`
- C++17 标准
- 图标通过 `GUIToolKit::loadIcon()` 从 qrc 路径 `:/icons/<name>.png` 加载
- 设备通信使用 SimpleFOC Commander 协议（单字符命令前缀，如 `C`、`T`、`V`、`A`、`Q`、`D`、`L`、`MG`、`MS`）
