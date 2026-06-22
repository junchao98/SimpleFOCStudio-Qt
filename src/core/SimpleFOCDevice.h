#ifndef SIMPLEFOCDEVICE_H
#define SIMPLEFOCDEVICE_H

#include <QObject>
#include <QString>
#include <QList>
#include <QVariantMap>
#include <QVariantList>

struct PIDController
{
    double P           = 0;
    double I           = 0;
    double D           = 0;
    double outputRamp  = 0;
    double outputLimit = 0;
    QString cmd;

    PIDController() = default;
    PIDController(const QString& c) : cmd(c) {}

    QVariantMap serialize() const
    {
        return {
            {"P", P}, {"I", I}, {"D", D}, {"outputRamp", outputRamp}, {"outputLimit", outputLimit}};
    }

    void load(const QVariantMap& json)
    {
        P           = json["P"].toDouble();
        I           = json["I"].toDouble();
        D           = json["D"].toDouble();
        outputRamp  = json["outputRamp"].toDouble();
        outputLimit = json["outputLimit"].toDouble();
    }
};

struct LowPassFilter
{
    double Tf = 0;
    QString cmd;
    LowPassFilter() = default;
    LowPassFilter(const QString& c) : cmd(c) {}
};

struct CustomCommand
{
    QString cmdName = "Command";
    QString cmd     = "";

    CustomCommand() = default;
    CustomCommand(const QString& name, const QString& c) : cmdName(name), cmd(c) {}

    QVariantMap serialize() const { return {{"commandName", cmdName}, {"commandValue", cmd}}; }

    void load(const QVariantMap& json)
    {
        cmdName = json["commandName"].toString();
        cmd     = json["commandValue"].toString();
    }
};

class SerialPortHandler;
class StateUpdateRunner;

class SimpleFOCDevice : public QObject
{
    Q_OBJECT

    public:
    enum ControlType
    {
        TORQUE_CONTROL            = 0,
        VELOCITY_CONTROL          = 1,
        ANGLE_CONTROL             = 2,
        VELOCITY_OPENLOOP_CONTROL = 3,
        ANGLE_OPENLOOP_CONTROL    = 4,
        HAPTIC_CONTROL            = 5
    };
    Q_ENUM(ControlType)

    enum PWMModulation
    {
        SINE_PWM         = 0,
        SPACE_VECTOR_PWM = 1,
        TRAPEZOIDAL_120  = 2,
        TRAPEZOIDAL_150  = 3
    };
    Q_ENUM(PWMModulation)

    enum TorqueType
    {
        VOLTAGE_TORQUE     = 0,
        DC_CURRENT_TORQUE  = 1,
        FOC_CURRENT_TORQUE = 2
    };
    Q_ENUM(TorqueType)

    enum ConnectionMode
    {
        PULL_CONFIG  = 0,
        PUSH_CONFIG  = 1,
        ONLY_CONNECT = 2
    };
    Q_ENUM(ConnectionMode)

    static const QString VELOCITY_PID;
    static const QString ANGLE_PID;
    static const QString CURRENT_Q_PID;
    static const QString CURRENT_D_PID;

    static SimpleFOCDevice* instance();

    void configureDevice(const QVariantMap& jsonValue);
    void configureConnection(const QVariantMap& configDict);
    QVariantMap toJSON() const;

    bool connectDevice(ConnectionMode mode);
    void disconnectDevice();
    bool getIsConnected() const { return m_isConnected; }

    void sendCommand(const QString& command);
    void setCommand(const QString& command, const QVariant& value);
    void getCommand(const QString& command);

    void sendControlType(int type);
    void sendTorqueType(int type);
    void sendMotionDownsample(double value);
    void sendHapticPreset(int preset);

    void sendProportionalGain(PIDController& pid, double value);
    void sendIntegralGain(PIDController& pid, double value);
    void sendDerivativeGain(PIDController& pid, double value);
    void sendOutputRamp(PIDController& pid, double value);
    void sendOutputLimit(PIDController& pid, double value);
    void sendLowPassFilter(LowPassFilter& lpf, double value);

    void sendVelocityLimit(double value);
    void sendVoltageLimit(double value);
    void sendCurrentLimit(double value);
    void sendPhaseResistance(double value);
    void sendTargetValue(double value);
    void sendSensorZeroOffset(double value);
    void sendSensorZeroElectrical(double value);
    void sendDeviceStatus(int value);
    void sendModulationCentered(double value);
    void sendModulationType(int value);
    void sendMonitorDownsample(int value);
    void sendMonitorClearVariables();
    void sendMonitorVariables(const QList<int>& varArray);
    void sendListDevices();

    void pullConfiguration();
    void pushConfiguration();
    void pullPIDConf(PIDController& pid, LowPassFilter& lpf);

    QString openedFile;

    PIDController PIDVelocity{VELOCITY_PID};
    PIDController PIDAngle{ANGLE_PID};
    PIDController PIDCurrentQ{CURRENT_Q_PID};
    PIDController PIDCurrentD{CURRENT_D_PID};

    LowPassFilter LPFVelocity{VELOCITY_PID};
    LowPassFilter LPFAngle{ANGLE_PID};
    LowPassFilter LPFCurrentQ{CURRENT_Q_PID};
    LowPassFilter LPFCurrentD{CURRENT_D_PID};

    double velocityLimit    = 0;
    double voltageLimit     = 0;
    double currentLimit     = 0;
    int controlType         = ANGLE_CONTROL;
    int torqueType          = VOLTAGE_TORQUE;
    int hapticPreset        = 6;
    double initialTarget    = 0;
    double motionDownsample = 0;

    int monitorDownsample = 0;
    QList<int> monitorVariables;

    double target      = 0;
    double targetNow   = 0;
    double angleNow    = 0;
    double velocityNow = 0;
    double voltageQNow = 0;
    double voltageDNow = 0;
    double currentQNow = 0;
    double currentDNow = 0;

    double phaseResistance    = 0;
    int deviceStatus          = 0;
    int modulationType        = 0;
    double modulationCentered = 1;

    double sensorElectricalZero = 0;
    double sensorZeroOffset     = 0;

    QList<CustomCommand> customCommands;

    QString serialPortName;
    int serialRate       = 115200;
    int serialByteSize   = 8;
    QString serialParity = "N";
    double stopBits      = 1;
    QString connectionID;
    QString devCommandID = "M";

    SerialPortHandler* commProvider = nullptr;

    signals:
    void connectionStateChanged(bool connected);
    void stateUpdated();
    void commandDataReceived(const QString& data);
    void monitoringDataReceived(const QList<double>& data);
    void rawDataReceived(const QString& data);
    void configurationUpdated();

    private:
    SimpleFOCDevice(QObject* parent = nullptr);
    ~SimpleFOCDevice();
    static SimpleFOCDevice* m_instance;

    bool m_isConnected                = false;
    StateUpdateRunner* m_stateUpdater = nullptr;

    void initCommunications();
    void closeCommunication();

    void parseResponses(const QString& commandResponse);
    void parseStateResponses(const QString& commandResponse);
    void parsePIDFResponse(PIDController& pid, LowPassFilter& lpf, const QString& response);
    void parseLimitsResponse(const QString& response);
    void parseMotionResponse(const QString& response);
    void parseTorqueResponse(const QString& response);
    void parseHapticResponse(const QString& response);
    void parseSensorResponse(const QString& response);
    void parseMonitorResponse(const QString& response);
    void parsePWMModResponse(const QString& response);

    friend class StateUpdateRunner;
};

#endif
