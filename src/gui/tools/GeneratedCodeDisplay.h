#ifndef GENERATEDCODEDISPLAY_H
#define GENERATEDCODEDISPLAY_H

#include <QWidget>
#include <QIcon>
#include <QTextEdit>

class SimpleFOCDevice;

class GeneratedCodeDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit GeneratedCodeDisplay(QWidget *parent = nullptr);
    QIcon getTabIcon() const;
    QString getTabName() const { return "Generated Code"; }
    void setCode(const QString &code);

private:
    SimpleFOCDevice *m_device;
    QTextEdit *m_codeDisplay;
};

#endif
