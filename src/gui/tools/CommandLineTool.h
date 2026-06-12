#ifndef COMMANDLINETOOL_H
#define COMMANDLINETOOL_H

#include <QWidget>
#include <QIcon>

class CommandLineWidget;
class SimpleFOCDevice;

class CommandLineTool : public QWidget
{
    Q_OBJECT

public:
    explicit CommandLineTool(QWidget *parent = nullptr);
    QIcon getTabIcon() const;
    QString getTabName() const { return "Cmd Line"; }

private:
    SimpleFOCDevice *m_device;
    CommandLineWidget *m_commandLine;
};

#endif
