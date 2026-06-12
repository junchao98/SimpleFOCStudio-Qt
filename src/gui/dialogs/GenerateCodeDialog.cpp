#include "GenerateCodeDialog.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>

GenerateCodeDialog::GenerateCodeDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Generate Arduino Code");
    setMinimumWidth(350);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Select parameters to generate:"));

    QStringList labels = {
        "Torque / Motion control",
        "PID velocity",
        "PID angle",
        "PID current q",
        "PID current d",
        "Limits",
        "Phase Resistance",
        "PWM Modulation",
        "Sensor Offset",
        "Sensor Electrical Offset"
    };

    for (int i = 0; i < labels.size(); ++i) {
        auto *cb = new QCheckBox(labels[i], this);
        if (i == 9) {
            cb->setChecked(false);
            cb->setToolTip("Warning: Only set if you know the electrical zero offset. Setting wrong values can damage the motor.");
        } else {
            cb->setChecked(true);
        }
        m_checkboxes.append(cb);
        layout->addWidget(cb);
    }

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

QList<bool> GenerateCodeDialog::selectedParameters() const
{
    QList<bool> result;
    for (const auto *cb : m_checkboxes)
        result.append(cb->isChecked());
    return result;
}
