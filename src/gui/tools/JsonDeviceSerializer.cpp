#include "JsonDeviceSerializer.h"
#include "core/SimpleFOCDevice.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

bool JsonDeviceSerializer::saveToFile(const SimpleFOCDevice *device, const QString &filePath)
{
    QVariantMap data = device->toJSON();
    QJsonObject jsonObj = QJsonObject::fromVariantMap(data);
    QJsonDocument doc(jsonObj);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool JsonDeviceSerializer::loadFromFile(SimpleFOCDevice *device, const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (doc.isNull() || !doc.isObject())
        return false;

    QVariantMap data = doc.object().toVariantMap();
    device->configureDevice(data);
    device->openedFile = filePath;
    return true;
}
