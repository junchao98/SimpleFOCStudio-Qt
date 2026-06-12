#ifndef GENERALSETTINGSWIDGET_H
#define GENERALSETTINGSWIDGET_H

#include <QGroupBox>
#include "gui/shared/ConfigLineEdit.h"

class SimpleFOCDevice;

class GeneralSettingsWidget : public QGroupBox
{
    Q_OBJECT

public:
    explicit GeneralSettingsWidget(QWidget *parent = nullptr);

public slots:
    void onConnectionStateChanged(bool connected);
    void onConfigurationUpdated();

private:
    SimpleFOCDevice *m_device;
    ConfigLineEdit *m_motionDownsample;
    ConfigLineEdit *m_currentLimit;
    ConfigLineEdit *m_velocityLimit;
    ConfigLineEdit *m_voltageLimit;
    bool m_updating = false;
};

#endif
