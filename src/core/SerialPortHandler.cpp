#include "SerialPortHandler.h"
#include <QDebug>

SerialPortHandler::SerialPortHandler(QObject *parent)
    : QThread(parent)
{
}

SerialPortHandler::~SerialPortHandler()
{
    stop();
    wait();
}

void SerialPortHandler::open(const QString &portName, int baudRate, int byteSize,
                              const QString &parity, double stopBits)
{
    m_serialPort = new QSerialPort();
    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(baudRate);
    m_serialPort->setDataBits(toDataBits(byteSize));
    m_serialPort->setParity(toParity(parity));
    m_serialPort->setStopBits(toStopBits(stopBits));
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (!m_serialPort->open(QIODevice::ReadWrite)) {
        delete m_serialPort;
        m_serialPort = nullptr;
        throw std::runtime_error("Cannot open serial port");
    }

    m_running = true;
    start();
}

void SerialPortHandler::close()
{
    m_running = false;
    wait();
    if (m_serialPort) {
        if (m_serialPort->isOpen())
            m_serialPort->close();
        delete m_serialPort;
        m_serialPort = nullptr;
    }
}

void SerialPortHandler::sendData(const QString &data)
{
    QMutexLocker locker(&m_mutex);
    if (m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->write(data.toUtf8());
        m_serialPort->flush();
    }
}

void SerialPortHandler::stop()
{
    m_running = false;
}

void SerialPortHandler::run()
{
    while (m_running && m_serialPort && m_serialPort->isOpen()) {
        if (m_serialPort->waitForReadyRead(50)) {
            while (m_serialPort->canReadLine()) {
                QByteArray data = m_serialPort->readLine();
                QString line = QString::fromUtf8(data).trimmed();
                if (!line.isEmpty()) {
                    handleReceivedData(line);
                }
            }
        }
    }
}

bool SerialPortHandler::isMonitoringData(const QString &data)
{
    if (data.isEmpty()) return false;
    QChar first = data.at(0);
    return first.isDigit() || first == '-';
}

bool SerialPortHandler::isStateData(const QString &data)
{
    return data.contains("Monitor");
}

void SerialPortHandler::handleReceivedData(const QString &data)
{
    if (data.isEmpty()) return;

    if (isMonitoringData(data)) {
        QStringList parts = data.split('\t');
        QList<double> values;
        for (const QString &p : parts) {
            bool ok;
            double v = p.trimmed().toDouble(&ok);
            if (ok) values.append(v);
        }
        emit monitoringDataReceived(values);
    } else if (isStateData(data)) {
        emit stateMonitorReceived(data);
    } else {
        emit commandDataReceived(data);
    }
    emit rawDataReceived(data);
}

QSerialPort::DataBits SerialPortHandler::toDataBits(int size)
{
    switch (size) {
    case 5: return QSerialPort::Data5;
    case 6: return QSerialPort::Data6;
    case 7: return QSerialPort::Data7;
    default: return QSerialPort::Data8;
    }
}

QSerialPort::Parity SerialPortHandler::toParity(const QString &p)
{
    if (p == "E" || p == "Even") return QSerialPort::EvenParity;
    if (p == "O" || p == "Odd") return QSerialPort::OddParity;
    return QSerialPort::NoParity;
}

QSerialPort::StopBits SerialPortHandler::toStopBits(double sb)
{
    if (sb == 1.5) return QSerialPort::OneAndHalfStop;
    if (sb == 2) return QSerialPort::TwoStop;
    return QSerialPort::OneStop;
}
