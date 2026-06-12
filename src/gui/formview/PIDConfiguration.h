#ifndef PIDCONFIGURATION_H
#define PIDCONFIGURATION_H

#include <QGroupBox>
#include <QComboBox>
#include "gui/shared/ConfigLineEdit.h"
#include "core/SimpleFOCDevice.h"

class PIDConfiguration : public QGroupBox
{
    Q_OBJECT

public:
    explicit PIDConfiguration(QWidget *parent = nullptr);

public slots:
    void onPIDSelected(int index);
    void onConfigurationUpdated();
    void onConnectionStateChanged(bool connected);

private:
    void updateFields();
    void connectFieldSignals();

    SimpleFOCDevice *m_device;
    QComboBox *m_pidSelector;
    ConfigLineEdit *m_pEdit;
    ConfigLineEdit *m_iEdit;
    ConfigLineEdit *m_dEdit;
    ConfigLineEdit *m_rampEdit;
    ConfigLineEdit *m_lpfEdit;
    bool m_updating = false;
};

#endif
