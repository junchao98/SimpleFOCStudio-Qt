#ifndef CONTROLLOOPCONFIG_H
#define CONTROLLOOPCONFIG_H

#include <QGroupBox>
#include <QComboBox>

class SimpleFOCDevice;

class ControlLoopConfig : public QGroupBox
{
    Q_OBJECT

public:
    explicit ControlLoopConfig(QWidget *parent = nullptr);

public slots:
    void onControlTypeChanged(int index);
    void onConfigurationUpdated();
    void onConnectionStateChanged(bool connected);

private:
    SimpleFOCDevice *m_device;
    QComboBox *m_combo;
    bool m_updating = false;
};

#endif
