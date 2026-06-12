#ifndef STATEUPDATERUNNER_H
#define STATEUPDATERUNNER_H

#include <QThread>
#include <QAtomicInt>

class SimpleFOCDevice;

class StateUpdateRunner : public QThread
{
    Q_OBJECT

public:
    StateUpdateRunner(SimpleFOCDevice *device, QObject *parent = nullptr);
    void stop();
    bool isStopped() const;

protected:
    void run() override;

private:
    SimpleFOCDevice *m_device;
    QAtomicInt m_stopFlag{0};
};

#endif
