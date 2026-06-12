#ifndef TORQUECONFIG_H
#define TORQUECONFIG_H

#include <QGroupBox>
#include <QComboBox>

class SimpleFOCDevice;

class TorqueConfig : public QGroupBox
{
    Q_OBJECT

public:
    explicit TorqueConfig(QWidget *parent = nullptr);

public slots:
    void onTorqueTypeChanged(int index);
    void onConfigurationUpdated();
    void onConnectionStateChanged(bool connected);

private:
    SimpleFOCDevice *m_device;
    QComboBox *m_combo;
    bool m_updating = false;
};

#endif
