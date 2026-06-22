#ifndef HAPTICCONFIG_H
#define HAPTICCONFIG_H

#include <QGroupBox>
#include <QComboBox>

class SimpleFOCDevice;

class HapticConfig : public QGroupBox
{
    Q_OBJECT

    public:
    explicit HapticConfig(QWidget* parent = nullptr);

    public slots:
    void onHapticPresetChanged(int index);
    void onConfigurationUpdated();
    void onConnectionStateChanged(bool connected);

    private:
    SimpleFOCDevice* m_device;
    QComboBox* m_combo;
    bool m_updating = false;
};

#endif
