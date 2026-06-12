#ifndef CONFIGURECONNECTIONWIDGET_H
#define CONFIGURECONNECTIONWIDGET_H

#include <QWidget>
class SimpleFOCDevice;

class ConfigureConnectionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConfigureConnectionWidget(QWidget *parent = nullptr);
};

#endif
