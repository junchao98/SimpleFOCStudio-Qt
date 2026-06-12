#ifndef GENERATECODEDIALOG_H
#define GENERATECODEDIALOG_H

#include <QDialog>
#include <QList>
#include <QCheckBox>

class GenerateCodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GenerateCodeDialog(QWidget *parent = nullptr);
    QList<bool> selectedParameters() const;

private:
    QList<QCheckBox*> m_checkboxes;
};

#endif
