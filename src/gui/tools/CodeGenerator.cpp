#include "CodeGenerator.h"
#include "core/SimpleFOCDevice.h"

QString CodeGenerator::generateSetupPrefix()
{
    return "#include <SimpleFOC.h>\n\nvoid setup(){\n\n";
}

QString CodeGenerator::generateSetupSuffix()
{
    return "\n\nmotor.init();\nmotor.initFOC();\n\n}\n\nvoid loop() {\n\n}";
}

QString CodeGenerator::generateFullCode(const SimpleFOCDevice *device)
{
    QString code = generateSetupPrefix();
    code += device->toArduinoCode();
    code += generateSetupSuffix();
    return code;
}
