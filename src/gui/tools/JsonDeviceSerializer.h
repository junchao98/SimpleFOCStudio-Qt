#ifndef JSONDEVICESERIALIZER_H
#define JSONDEVICESERIALIZER_H

#include <QString>
#include <QVariantMap>

class SimpleFOCDevice;

class JsonDeviceSerializer
{
public:
    static bool saveToFile(const SimpleFOCDevice *device, const QString &filePath);
    static bool loadFromFile(SimpleFOCDevice *device, const QString &filePath);
};

#endif
