#include "StateUpdateRunner.h"
#include "SimpleFOCDevice.h"
#include <QThread>

StateUpdateRunner::StateUpdateRunner(SimpleFOCDevice *device, QObject *parent)
    : QThread(parent), m_device(device)
{
}

void StateUpdateRunner::stop()
{
    m_stopFlag.storeRelaxed(1);
}

bool StateUpdateRunner::isStopped() const
{
    return m_stopFlag.loadRelaxed() != 0;
}

void StateUpdateRunner::run()
{
    while (!m_stopFlag.loadRelaxed()) {
        if (m_device && m_device->getIsConnected()) {
            m_device->getCommand("MG0");
            QThread::msleep(100);
            m_device->getCommand("MG1");
            QThread::msleep(100);
            m_device->getCommand("MG2");
            QThread::msleep(100);
            m_device->getCommand("MG3");
            QThread::msleep(100);
            m_device->getCommand("MG4");
            QThread::msleep(100);
            m_device->getCommand("MG5");
            QThread::msleep(100);
            m_device->getCommand("MG6");
            QThread::msleep(100);
        }
        QThread::msleep(300);
    }
}
