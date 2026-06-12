#ifndef SERIALPORTHANDLER_H
#define SERIALPORTHANDLER_H

#include <QThread>
#include <QSerialPort>
#include <QMutex>
#include <QList>

class SerialPortHandler : public QThread
{
    Q_OBJECT

public:
    explicit SerialPortHandler(QObject *parent = nullptr);
    ~SerialPortHandler();

    void open(const QString &portName, int baudRate, int byteSize,
              const QString &parity, double stopBits);
    void close();
    void sendData(const QString &data);
    void stop();

signals:
    void commandDataReceived(const QString &data);
    void stateMonitorReceived(const QString &data);
    void monitoringDataReceived(const QList<double> &data);
    void rawDataReceived(const QString &data);

protected:
    void run() override;

private:
    QSerialPort *m_serialPort = nullptr;
    QMutex m_mutex;
    bool m_running = false;

    bool isMonitoringData(const QString &data);
    bool isStateData(const QString &data);
    void handleReceivedData(const QString &data);

    QSerialPort::DataBits toDataBits(int size);
    QSerialPort::Parity toParity(const QString &p);
    QSerialPort::StopBits toStopBits(double sb);
};

#endif
