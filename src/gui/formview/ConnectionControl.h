#ifndef CONNECTIONCONTROL_H
#define CONNECTIONCONTROL_H

#include <QGroupBox>
#include <QPushButton>
#include <QLineEdit>

class SimpleFOCDevice;

class ConnectionControl : public QGroupBox
{
    Q_OBJECT

public:
    explicit ConnectionControl(QWidget *parent = nullptr);

signals:
    void configureClicked();

private slots:
    void onConnect();
    void onDisconnect();
    void onPullParams();
    void onConnectionStateChanged(bool connected);
    void onConfigureClicked();

private:
    SimpleFOCDevice *m_device;
    QLineEdit *m_commandIdEdit;
    QPushButton *m_pullBtn;
    QPushButton *m_connectBtn;
    QPushButton *m_disconnectBtn;
    QPushButton *m_configureBtn;
};

#endif
