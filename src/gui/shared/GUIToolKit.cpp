#include "GUIToolKit.h"
#include <QPixmap>
#include <QMap>

QColor GUIToolKit::RED_COLOR(255, 92, 92);
QColor GUIToolKit::GREEN_COLOR(57, 217, 138);
QColor GUIToolKit::BLUE_COLOR(91, 141, 236);
QColor GUIToolKit::ORANGE_COLOR(253, 172, 66);
QColor GUIToolKit::YELLOW_COLOR(255, 255, 51);
QColor GUIToolKit::PURPLE_COLOR(75, 0, 130);
QColor GUIToolKit::MAROON_COLOR(222, 184, 135);

QIcon GUIToolKit::getIconByName(const QString &name)
{
    QMap<QString, QString> fileIndex = {
        {"add", "add"}, {"add_motor", "add_motor"}, {"tree", "tree"},
        {"gen", "gen"}, {"home", "home"}, {"form", "form"},
        {"edit", "edit"}, {"delete", "delete"}, {"statistics", "statistics"},
        {"reddot", "reddot"}, {"orangedot", "orangedot"}, {"greendot", "greendot"},
        {"bluedot", "bluedot"}, {"purpledot", "purpledot"}, {"yellowdot", "yellowdot"},
        {"maroondot", "maroondot"}, {"send", "send"}, {"zoomall", "zoomall"},
        {"connect", "connect"}, {"continue", "continue"}, {"alert", "alert"},
        {"gear", "gear"}, {"generalsettings", "generalsettings"}, {"open", "open"},
        {"loop", "loop"}, {"save", "save"}, {"stop", "stop"}, {"restart", "continue"},
        {"res", "res"}, {"sensor", "sensor"}, {"start", "start"}, {"motor", "motor"},
        {"pause", "pause"}, {"pull", "pull"}, {"push", "push"}, {"list", "list"},
        {"disconnect", "disconnect"}, {"configure", "configure"}, {"pidconfig", "pidconfig"},
        {"consoletool", "consoletool"}, {"fordward", "fordward"},
        {"fastbackward", "fastbackward"}, {"backward", "backward"},
        {"stopjogging", "stopjogging"}, {"fastfordward", "fastfordward"},
        {"customcommands", "customcommands"}
    };

    QString fileName = fileIndex.value(name, name);
    return QIcon(QString(":/icons/%1.png").arg(fileName));
}
