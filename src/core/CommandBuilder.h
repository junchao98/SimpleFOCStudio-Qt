#ifndef COMMANDBUILDER_H
#define COMMANDBUILDER_H

#include <QString>
#include <QVariant>

class CommandBuilder
{
public:
    static QString build(const QString &devId, const QString &cmd, const QVariant &value)
    {
        return devId + cmd + value.toString();
    }
};

#endif
