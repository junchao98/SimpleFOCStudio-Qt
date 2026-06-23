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
    explicit ConfigureConnectionDialog(QWidget* parent = nullptr);

    QString portName() const;
    int baudRate() const;
    QString parity() const;
    int byteSize() const;
    double stopBits() const;

    void setPortName(const QString& name);
    void setBaudRate(int rate);
    void setParity(const QString& p);
    void setByteSize(int size);
    void setStopBits(double sb);

    private:
    SerialPortComboBox* m_portCombo;
    QLineEdit* m_baudRateEdit;
    QComboBox* m_parityCombo;
    QComboBox* m_byteSizeCombo;
    QComboBox* m_stopBitsCombo;
    QDialogButtonBox* m_buttonBox;
};

#endif
