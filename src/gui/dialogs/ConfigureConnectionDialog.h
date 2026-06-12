#ifndef CONFIGURECONNECTIONDIALOG_H
#define CONFIGURECONNECTIONDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QDialogButtonBox>

class SerialPortComboBox;

class ConfigureConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigureConnectionDialog(QWidget *parent = nullptr);

    QString portName() const;
    int baudRate() const;
    QString parity() const;
    int byteSize() const;
    double stopBits() const;
    QString connectionID() const;

    void setPortName(const QString &name);
    void setBaudRate(int rate);
    void setParity(const QString &p);
    void setByteSize(int size);
    void setStopBits(double sb);
    void setConnectionID(const QString &id);

private:
    SerialPortComboBox *m_portCombo;
    QLineEdit *m_baudRateEdit;
    QComboBox *m_parityCombo;
    QComboBox *m_byteSizeCombo;
    QComboBox *m_stopBitsCombo;
    QLineEdit *m_connectionIdEdit;
    QDialogButtonBox *m_buttonBox;
};

#endif
