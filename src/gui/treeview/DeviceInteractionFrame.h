#ifndef DEVICEINTERACTIONFRAME_H
#define DEVICEINTERACTIONFRAME_H

#include <QWidget>
#include <QVBoxLayout>

class GraphicWidget;
class CommandLineWidget;
class SimpleFOCDevice;

class DeviceInteractionFrame : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceInteractionFrame(QWidget *parent = nullptr);

private:
    SimpleFOCDevice *m_device;
    GraphicWidget *m_graphicWidget;
    CommandLineWidget *m_commandLine;
};

#endif
