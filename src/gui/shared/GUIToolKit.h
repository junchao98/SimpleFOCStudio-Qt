#ifndef GUITOOLKIT_H
#define GUITOOLKIT_H

#include <QIcon>
#include <QColor>

class GUIToolKit
{
public:
    static QColor RED_COLOR;
    static QColor GREEN_COLOR;
    static QColor BLUE_COLOR;
    static QColor ORANGE_COLOR;
    static QColor YELLOW_COLOR;
    static QColor PURPLE_COLOR;
    static QColor MAROON_COLOR;

    static QIcon getIconByName(const QString &name);
};

#endif
