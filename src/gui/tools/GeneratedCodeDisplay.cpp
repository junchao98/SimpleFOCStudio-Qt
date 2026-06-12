#include "GeneratedCodeDisplay.h"
#include "gui/shared/GUIToolKit.h"
#include "core/SimpleFOCDevice.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFont>

GeneratedCodeDisplay::GeneratedCodeDisplay(QWidget *parent)
    : QWidget(parent)
    , m_device(SimpleFOCDevice::instance())
{
    auto *layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("<h2>Generated Arduino Code</h2>"));
    layout->addWidget(new QLabel("This generated code you can just copy/paste into your setup() function, "
                                  "it is important that you place it before calling the motor.init()"));

    m_codeDisplay = new QTextEdit(this);
    m_codeDisplay->setReadOnly(true);
    m_codeDisplay->setFont(QFont("Courier", 10));
    layout->addWidget(m_codeDisplay, 1);

    layout->addWidget(new QLabel("motor.init();\nmotor.initFOC();"));
}

QIcon GeneratedCodeDisplay::getTabIcon() const
{
    return GUIToolKit::getIconByName("gen");
}

void GeneratedCodeDisplay::setCode(const QString &code)
{
    m_codeDisplay->setPlainText(code);
}
