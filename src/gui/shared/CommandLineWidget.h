#ifndef COMMANDLINEWIDGET_H
#define COMMANDLINEWIDGET_H

#include <QGroupBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

class SimpleFOCDevice;

class CommandLineWidget : public QGroupBox
{
    Q_OBJECT

public:
    explicit CommandLineWidget(QWidget *parent = nullptr);
    void appendText(const QString &text);
    void clearText();

private slots:
    void onSend();
    void onClear();
    void onListDevices();
    void onRawDataReceived(const QString &data);

private:
    SimpleFOCDevice *m_device;
    QTextEdit *m_textDisplay;
    QLineEdit *m_commandInput;
    QPushButton *m_sendBtn;
    QPushButton *m_clearBtn;
    QPushButton *m_listDevicesBtn;
};

#endif
