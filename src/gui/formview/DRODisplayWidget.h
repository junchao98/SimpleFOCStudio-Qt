#ifndef DRODISPLAYWIDGET_H
#define DRODISPLAYWIDGET_H

#include <QGroupBox>
#include <QLCDNumber>
#include <QLabel>
#include <QHBoxLayout>

class SimpleFOCDevice;

class DRODisplayWidget : public QGroupBox
{
    Q_OBJECT

public:
    explicit DRODisplayWidget(QWidget *parent = nullptr);

public slots:
    void onStateChanged();

private:
    SimpleFOCDevice *m_device;
    QLCDNumber *m_angleDisplay;
    QLCDNumber *m_velocityDisplay;
    QLCDNumber *m_currentDisplay;
    QLCDNumber *m_targetDisplay;
};

#endif
