#include "ConfigureConnectionDialog.h"
#include "gui/shared/SerialPortComboBox.h"
#include <QFormLayout>
#include <QIntValidator>

ConfigureConnectionDialog::ConfigureConnectionDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Configure serial connection");
    setMinimumWidth(350);

    auto *layout = new QFormLayout(this);

    m_portCombo = new SerialPortComboBox(this);
    layout->addRow("Port Name:", m_portCombo);

    m_baudRateEdit = new QLineEdit("115200", this);
    m_baudRateEdit->setValidator(new QIntValidator(this));
    layout->addRow("Bit rate:", m_baudRateEdit);

    m_parityCombo = new QComboBox(this);
    m_parityCombo->addItems({"None", "Even", "Odd"});
    layout->addRow("Parity:", m_parityCombo);

    m_byteSizeCombo = new QComboBox(this);
    m_byteSizeCombo->addItems({"5", "6", "7", "8"});
    m_byteSizeCombo->setCurrentText("8");
    layout->addRow("Byte size:", m_byteSizeCombo);

    m_stopBitsCombo = new QComboBox(this);
    m_stopBitsCombo->addItems({"1", "1.5", "2"});
    layout->addRow("Stop bits:", m_stopBitsCombo);

    m_connectionIdEdit = new QLineEdit(this);
    layout->addRow("Connection ID:", m_connectionIdEdit);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addRow(m_buttonBox);
}

QString ConfigureConnectionDialog::portName() const { return m_portCombo->currentText(); }
int ConfigureConnectionDialog::baudRate() const { return m_baudRateEdit->text().toInt(); }
QString ConfigureConnectionDialog::parity() const
{
    QString p = m_parityCombo->currentText();
    if (p == "Even") return "E";
    if (p == "Odd") return "O";
    return "N";
}
int ConfigureConnectionDialog::byteSize() const { return m_byteSizeCombo->currentText().toInt(); }
double ConfigureConnectionDialog::stopBits() const { return m_stopBitsCombo->currentText().toDouble(); }
QString ConfigureConnectionDialog::connectionID() const { return m_connectionIdEdit->text(); }

void ConfigureConnectionDialog::setPortName(const QString &n) { m_portCombo->setCurrentText(n); }
void ConfigureConnectionDialog::setBaudRate(int r) { m_baudRateEdit->setText(QString::number(r)); }
void ConfigureConnectionDialog::setParity(const QString &p)
{
    if (p == "E") m_parityCombo->setCurrentText("Even");
    else if (p == "O") m_parityCombo->setCurrentText("Odd");
    else m_parityCombo->setCurrentText("None");
}
void ConfigureConnectionDialog::setByteSize(int s) { m_byteSizeCombo->setCurrentText(QString::number(s)); }
void ConfigureConnectionDialog::setStopBits(double s) { m_stopBitsCombo->setCurrentText(QString::number(s)); }
void ConfigureConnectionDialog::setConnectionID(const QString &id) { m_connectionIdEdit->setText(id); }
