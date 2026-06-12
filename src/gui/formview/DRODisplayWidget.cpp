#include "DRODisplayWidget.h"
#include "core/SimpleFOCDevice.h"
#include <QVBoxLayout>
#include <QGroupBox>

DRODisplayWidget::DRODisplayWidget(QWidget *parent)
    : QGroupBox("Simple FOC Digital Read Out", parent)
    , m_device(SimpleFOCDevice::instance())
{
    auto *layout = new QHBoxLayout(this);

    auto createDisplay = [](const QString &label) -> QLCDNumber* {
        auto *l = new QLabel(label);
        l->setAlignment(Qt::AlignCenter);
        auto *lcd = new QLCDNumber(8);
        lcd->setSegmentStyle(QLCDNumber::Flat);
        lcd->setMinimumHeight(40);
        return lcd;
    };

    auto addColumn = [&](const QString &label, QLCDNumber *&lcd) {
        auto *v = new QVBoxLayout();
        auto *l = new QLabel(label);
        l->setAlignment(Qt::AlignCenter);
        lcd = createDisplay(label);
        v->addWidget(l);
        v->addWidget(lcd);
        layout->addLayout(v);
    };

    addColumn("Angle", m_angleDisplay);
    addColumn("Velocity", m_velocityDisplay);
    addColumn("Current", m_currentDisplay);
    addColumn("Target", m_targetDisplay);

    connect(m_device, &SimpleFOCDevice::stateUpdated, this, &DRODisplayWidget::onStateChanged);
}

void DRODisplayWidget::onStateChanged()
{
    m_angleDisplay->display(m_device->angleNow);
    m_velocityDisplay->display(m_device->velocityNow);
    m_targetDisplay->display(m_device->targetNow);

    if (m_device->torqueType == SimpleFOCDevice::VOLTAGE_TORQUE)
        m_currentDisplay->display(m_device->voltageQNow);
    else
        m_currentDisplay->display(m_device->currentQNow);
}
