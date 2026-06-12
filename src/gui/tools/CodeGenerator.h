#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <QString>

class CodeGenerator
{
public:
    static QString generateFullCode(const class SimpleFOCDevice *device);
    static QString generateSetupPrefix();
    static QString generateSetupSuffix();
};

#endif
