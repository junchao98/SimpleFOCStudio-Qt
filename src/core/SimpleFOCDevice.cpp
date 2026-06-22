#include "SimpleFOCDevice.h"
#include "SerialPortHandler.h"
#include "StateUpdateRunner.h"
#include <QSerialPort>
#include <QMessageBox>
#include <QThread>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QThread>

const QString SimpleFOCDevice::VELOCITY_PID  = "V";
const QString SimpleFOCDevice::ANGLE_PID     = "A";
const QString SimpleFOCDevice::CURRENT_Q_PID = "Q";
const QString SimpleFOCDevice::CURRENT_D_PID = "D";

SimpleFOCDevice* SimpleFOCDevice::m_instance = nullptr;

SimpleFOCDevice::SimpleFOCDevice(QObject* parent) : QObject(parent)
{
    commProvider = new SerialPortHandler(this);
    connect(commProvider,
            &SerialPortHandler::commandDataReceived,
            this,
            &SimpleFOCDevice::parseResponses);
    connect(commProvider,
            &SerialPortHandler::commandDataReceived,
            this,
            &SimpleFOCDevice::commandDataReceived);
    connect(commProvider,
            &SerialPortHandler::stateMonitorReceived,
            this,
            &SimpleFOCDevice::parseStateResponses);
    connect(
        commProvider, &SerialPortHandler::rawDataReceived, this, &SimpleFOCDevice::rawDataReceived);
    connect(commProvider,
            &SerialPortHandler::monitoringDataReceived,
            this,
            [this](const QList<double>& data) { emit monitoringDataReceived(data); });
}

SimpleFOCDevice::~SimpleFOCDevice()
{
    if(m_isConnected)
        disconnectDevice();
    m_instance = nullptr;
}

SimpleFOCDevice* SimpleFOCDevice::instance()
{
    if(!m_instance)
        m_instance = new SimpleFOCDevice();
    return m_instance;
}

void SimpleFOCDevice::configureDevice(const QVariantMap& json)
{
    PIDVelocity.load(json["PIDVelocity"].toMap());
    PIDAngle.load(json["PIDAngle"].toMap());
    PIDCurrentD.load(json["PIDCurrentD"].toMap());
    PIDCurrentQ.load(json["PIDCurrentQ"].toMap());

    LPFVelocity.Tf = json["LPFVelocity"].toDouble();
    LPFAngle.Tf    = json["LPFAngle"].toDouble();
    LPFCurrentQ.Tf = json["LPFCurrentQ"].toDouble();
    LPFCurrentD.Tf = json["LPFCurrentD"].toDouble();

    velocityLimit    = json["velocityLimit"].toDouble();
    voltageLimit     = json["voltageLimit"].toDouble();
    currentLimit     = json["currentLimit"].toDouble();
    controlType      = json["controlType"].toInt();
    torqueType       = json["torqueType"].toInt();
    hapticPreset     = json.value("hapticPreset").toInt();
    motionDownsample = json["motionDownsample"].toDouble();

    sensorElectricalZero = json["sensorElectricalZero"].toDouble();
    sensorZeroOffset     = json["sensorZeroOffset"].toDouble();
    phaseResistance      = json["phaseResistance"].toDouble();
    initialTarget        = json["initialTarget"].toDouble();

    connectionID   = json["connectionID"].toString();
    serialPortName = json["serialPortName"].toString();
    serialRate     = json["serialRate"].toInt();
    serialByteSize = json["serialByteSize"].toInt();
    serialParity   = json["serialParity"].toString();
    stopBits       = json["stopBits"].toDouble();

    devCommandID = json.value("devCommandID").toString();

    customCommands.clear();
    QVariantList cmds = json.value("customCommands").toList();
    for(const auto& c : cmds)
    {
        CustomCommand cmd;
        cmd.load(c.toMap());
        customCommands.append(cmd);
    }

    emit configurationUpdated();
}

void SimpleFOCDevice::configureConnection(const QVariantMap& config)
{
    connectionID   = config["connectionID"].toString();
    serialPortName = config["serialPortName"].toString();
    serialRate     = config["serialRate"].toInt();
    serialByteSize = config["serialByteSize"].toInt();
    serialParity   = config["serialParity"].toString();
    stopBits       = config["stopBits"].toDouble();
}

QVariantMap SimpleFOCDevice::toJSON() const
{
    QVariantMap m;
    m["PIDVelocity"]          = PIDVelocity.serialize();
    m["PIDAngle"]             = PIDAngle.serialize();
    m["PIDCurrentD"]          = PIDCurrentD.serialize();
    m["PIDCurrentQ"]          = PIDCurrentQ.serialize();
    m["LPFVelocity"]          = LPFVelocity.Tf;
    m["LPFAngle"]             = LPFAngle.Tf;
    m["LPFCurrentD"]          = LPFCurrentD.Tf;
    m["LPFCurrentQ"]          = LPFCurrentQ.Tf;
    m["velocityLimit"]        = velocityLimit;
    m["voltageLimit"]         = voltageLimit;
    m["currentLimit"]         = currentLimit;
    m["controlType"]          = controlType;
    m["motionDownsample"]     = motionDownsample;
    m["torqueType"]           = torqueType;
    m["hapticPreset"]         = hapticPreset;
    m["phaseResistance"]      = phaseResistance;
    m["sensorZeroOffset"]     = sensorZeroOffset;
    m["sensorElectricalZero"] = sensorElectricalZero;
    m["initialTarget"]        = initialTarget;
    m["connectionID"]         = connectionID;
    m["serialPortName"]       = serialPortName;
    m["serialRate"]           = serialRate;
    m["serialByteSize"]       = serialByteSize;
    m["serialParity"]         = serialParity;
    m["stopBits"]             = stopBits;
    m["devCommandID"]         = devCommandID;

    QVariantList cmdList;
    for(const auto& c : customCommands)
        cmdList.append(c.serialize());
    m["customCommands"] = cmdList;

    return m;
}

bool SimpleFOCDevice::connectDevice(ConnectionMode mode)
{
    try
    {
        initCommunications();
    }
    catch(...)
    {
        QMessageBox::warning(
            nullptr, "SimpleFOC ConfigTool", "Error while trying to open serial port");
        return false;
    }

    m_isConnected = true;
    emit connectionStateChanged(true);

    if(mode == PULL_CONFIG)
    {
        pullConfiguration();
    }
    else if(mode == PUSH_CONFIG)
    {
        pushConfiguration();
    }

    if(m_stateUpdater)
    {
        m_stateUpdater->stop();
        m_stateUpdater->wait();
        delete m_stateUpdater;
    }
    m_stateUpdater = new StateUpdateRunner(this, this);
    m_stateUpdater->start();

    return true;
}

void SimpleFOCDevice::disconnectDevice()
{
    m_isConnected = false;
    closeCommunication();
    if(m_stateUpdater)
    {
        m_stateUpdater->stop();
        m_stateUpdater->wait();
        delete m_stateUpdater;
        m_stateUpdater = nullptr;
    }
    emit connectionStateChanged(false);
}

void SimpleFOCDevice::initCommunications()
{
    commProvider->open(serialPortName, serialRate, serialByteSize, serialParity, stopBits);
}

void SimpleFOCDevice::closeCommunication() { commProvider->close(); }

void SimpleFOCDevice::sendCommand(const QString& command)
{
    if(m_isConnected)
        commProvider->sendData(command + "\n");
}

void SimpleFOCDevice::setCommand(const QString& command, const QVariant& value)
{
    if(m_isConnected)
        sendCommand(devCommandID + command + value.toString());
}

void SimpleFOCDevice::getCommand(const QString& command)
{
    if(m_isConnected)
        sendCommand(devCommandID + command);
}

void SimpleFOCDevice::sendControlType(int type)
{
    if(m_isConnected)
    {
        controlType = type;
        setCommand("C", QString::number(type));
    }
}

void SimpleFOCDevice::sendTorqueType(int type)
{
    if(m_isConnected)
    {
        torqueType = type;
        setCommand("T", QString::number(type));
    }
}

void SimpleFOCDevice::sendMotionDownsample(double value)
{
    if(m_isConnected)
    {
        motionDownsample = value;
        setCommand("CD", QString::number(value));
    }
}

void SimpleFOCDevice::sendHapticPreset(int preset)
{
    if(m_isConnected)
    {
        hapticPreset = preset;
        setCommand("H", QString::number(preset));
    }
}

void SimpleFOCDevice::sendProportionalGain(PIDController& pid, double value)
{
    if(m_isConnected)
    {
        if(value != 0)
            pid.P = value;
        setCommand(pid.cmd + "P", QString::number(value));
    }
}

void SimpleFOCDevice::sendIntegralGain(PIDController& pid, double value)
{
    if(m_isConnected)
    {
        if(value != 0)
            pid.I = value;
        setCommand(pid.cmd + "I", QString::number(value));
    }
}

void SimpleFOCDevice::sendDerivativeGain(PIDController& pid, double value)
{
    if(m_isConnected)
    {
        if(value != 0)
            pid.D = value;
        setCommand(pid.cmd + "D", QString::number(value));
    }
}

void SimpleFOCDevice::sendOutputRamp(PIDController& pid, double value)
{
    if(m_isConnected)
    {
        if(value != 0)
            pid.outputRamp = value;
        setCommand(pid.cmd + "R", QString::number(value));
    }
}

void SimpleFOCDevice::sendOutputLimit(PIDController& pid, double value)
{
    if(m_isConnected)
    {
        if(value != 0)
            pid.outputLimit = value;
        setCommand(pid.cmd + "L", QString::number(value));
    }
}

void SimpleFOCDevice::sendLowPassFilter(LowPassFilter& lpf, double value)
{
    if(m_isConnected)
    {
        if(value != 0)
            lpf.Tf = value;
        setCommand(lpf.cmd + "F", QString::number(value));
    }
}

void SimpleFOCDevice::sendVelocityLimit(double value)
{
    if(m_isConnected)
    {
        if(value != 0)
            velocityLimit = value;
        setCommand("LV", QString::number(value));
    }
}

void SimpleFOCDevice::sendVoltageLimit(double value)
{
    if(m_isConnected)
    {
        if(value != 0)
            voltageLimit = value;
        setCommand("LU", QString::number(value));
    }
}

void SimpleFOCDevice::sendCurrentLimit(double value)
{
    if(m_isConnected)
    {
        if(value != 0)
            currentLimit = value;
        setCommand("LC", QString::number(value));
    }
}

void SimpleFOCDevice::sendPhaseResistance(double value)
{
    if(m_isConnected)
    {
        if(value != 0)
            phaseResistance = value;
        setCommand("R", QString::number(value));
    }
}

void SimpleFOCDevice::sendTargetValue(double value)
{
    if(m_isConnected)
    {
        target = value;
        setCommand("", QString::number(value));
    }
}

void SimpleFOCDevice::sendSensorZeroOffset(double value)
{
    if(m_isConnected)
    {
        if(value != 0)
            sensorZeroOffset = value;
        setCommand("SM", QString::number(value));
    }
}

void SimpleFOCDevice::sendSensorZeroElectrical(double value)
{
    if(m_isConnected)
    {
        if(value != 0)
            sensorElectricalZero = value;
        setCommand("SE", QString::number(value));
    }
}

void SimpleFOCDevice::sendDeviceStatus(int value)
{
    if(m_isConnected)
    {
        deviceStatus = value;
        setCommand("E", QString::number(value));
    }
}

void SimpleFOCDevice::sendModulationCentered(double value)
{
    if(m_isConnected)
    {
        modulationCentered = value;
        setCommand("WC", QString::number(value));
    }
}

void SimpleFOCDevice::sendModulationType(int value)
{
    if(m_isConnected)
    {
        modulationType = value;
        setCommand("WT", QString::number(value));
    }
}

void SimpleFOCDevice::sendMonitorDownsample(int value)
{
    if(m_isConnected)
    {
        monitorDownsample = value;
        setCommand("MD", QString::number(value));
    }
}

void SimpleFOCDevice::sendMonitorClearVariables()
{
    if(m_isConnected)
    {
        monitorVariables.clear();
        getCommand("MC");
    }
}

void SimpleFOCDevice::sendMonitorVariables(const QList<int>& varArray)
{
    if(!m_isConnected)
        return;
    if(varArray.isEmpty())
    {
        getCommand("MS");
        return;
    }

    int val = 0;
    int m   = 1000000;
    for(int v : varArray)
    {
        val += v * m;
        m /= 10;
    }
    monitorVariables = varArray;
    setCommand("MS", QString("%1").arg(val, 7, 10, QChar('0')));
}

void SimpleFOCDevice::sendListDevices()
{
    if(m_isConnected)
        sendCommand("?");
}

void SimpleFOCDevice::pullConfiguration()
{
    QThread::msleep(5);
    getCommand("C");
    QThread::msleep(5);
    getCommand("T");
    QThread::msleep(5);
    pullPIDConf(PIDVelocity, LPFVelocity);
    QThread::msleep(5);
    pullPIDConf(PIDAngle, LPFAngle);
    QThread::msleep(5);
    pullPIDConf(PIDCurrentD, LPFCurrentD);
    QThread::msleep(5);
    pullPIDConf(PIDCurrentQ, LPFCurrentQ);
    QThread::msleep(5);
    getCommand("LV");
    QThread::msleep(5);
    getCommand("LU");
    QThread::msleep(5);
    getCommand("LC");
    QThread::msleep(5);
    getCommand("SE");
    QThread::msleep(5);
    getCommand("SM");
    QThread::msleep(5);
    getCommand("CD");
    QThread::msleep(5);
    getCommand("R");
    QThread::msleep(5);
    getCommand("WC");
    QThread::msleep(5);
    getCommand("E");
    QThread::msleep(5);
    getCommand("H");
}

void SimpleFOCDevice::pushConfiguration() {}

void SimpleFOCDevice::pullPIDConf(PIDController& pid, LowPassFilter& lpf)
{
    Q_UNUSED(lpf);
    getCommand(pid.cmd + "P");
    QThread::msleep(5);
    getCommand(pid.cmd + "I");
    QThread::msleep(5);
    getCommand(pid.cmd + "D");
    QThread::msleep(5);
    getCommand(pid.cmd + "R");
    QThread::msleep(5);
    getCommand(pid.cmd + "L");
    QThread::msleep(5);
    getCommand(pid.cmd + "F");
}

void SimpleFOCDevice::parseResponses(const QString& response)
{
    QString r = response;
    if(r.contains("PID vel"))
    {
        r = r.mid(r.indexOf("PID vel|") + 8);
        parsePIDFResponse(PIDVelocity, LPFVelocity, r);
    }
    else if(r.contains("PID angle"))
    {
        r = r.mid(r.indexOf("PID angle|") + 10);
        parsePIDFResponse(PIDAngle, LPFAngle, r);
    }
    else if(r.contains("PID curr q"))
    {
        r = r.mid(r.indexOf("PID curr q|") + 11);
        parsePIDFResponse(PIDCurrentQ, LPFCurrentQ, r);
    }
    else if(r.contains("PID curr d"))
    {
        r = r.mid(r.indexOf("PID curr d|") + 11);
        parsePIDFResponse(PIDCurrentD, LPFCurrentD, r);
    }
    else if(r.contains("Limits"))
    {
        r = r.mid(r.indexOf("Limits|") + 7);
        parseLimitsResponse(r);
    }
    else if(r.contains("Motion"))
    {
        r = r.mid(r.indexOf("Motion:") + 7);
        parseMotionResponse(r);
    }
    else if(r.contains("Haptic"))
    {
        r = r.mid(r.indexOf("Haptic:") + 7);
        parseHapticResponse(r);
    }
    else if(r.contains("Torque"))
    {
        r = r.mid(r.indexOf("Torque:") + 7);
        parseTorqueResponse(r);
    }
    else if(r.contains("Sensor"))
    {
        r = r.mid(r.indexOf("Sensor |") + 8);
        parseSensorResponse(r);
    }
    else if(r.contains("Monitor"))
    {
        r = r.mid(r.indexOf("Monitor |") + 9);
        parseMonitorResponse(r);
    }
    else if(r.contains("Status"))
    {
        deviceStatus = r.mid(r.indexOf("Status:") + 7).toInt();
    }
    else if(r.contains("R phase"))
    {
        phaseResistance = r.mid(r.indexOf("R phase:") + 8).toDouble();
    }
    else if(r.contains("PWM Mod"))
    {
        r = r.mid(r.indexOf("PWM Mod | ") + 10);
        parsePWMModResponse(r);
    }
    emit configurationUpdated();
}

void SimpleFOCDevice::parseStateResponses(const QString& response)
{
    QString r = response;
    if(r.contains("Monitor"))
    {
        r = r.mid(r.indexOf("Monitor |") + 9);
        parseMonitorResponse(r);
    }
    emit stateUpdated();
}

void SimpleFOCDevice::parsePIDFResponse(PIDController& pid, LowPassFilter& lpf, const QString& r)
{
    if(r.contains("P: "))
        pid.P = r.mid(r.indexOf("P: ") + 3).toDouble();
    else if(r.contains("I: "))
        pid.I = r.mid(r.indexOf("I: ") + 3).toDouble();
    else if(r.contains("D: "))
        pid.D = r.mid(r.indexOf("D: ") + 3).toDouble();
    else if(r.contains("ramp:"))
    {
        QString val    = r.mid(r.indexOf("ramp:") + 5);
        pid.outputRamp = val.contains("ovf") ? 0 : val.toDouble();
    }
    else if(r.contains("limit:"))
        pid.outputLimit = r.mid(r.indexOf("limit:") + 6).toDouble();
    else if(r.contains("Tf: "))
        lpf.Tf = r.mid(r.indexOf("Tf: ") + 4).toDouble();
}

void SimpleFOCDevice::parseLimitsResponse(const QString& r)
{
    if(r.contains("vel:"))
        velocityLimit = r.mid(r.indexOf("vel:") + 4).toDouble();
    else if(r.contains("volt:"))
        voltageLimit = r.mid(r.indexOf("volt:") + 5).toDouble();
    else if(r.contains("curr:"))
        currentLimit = r.mid(r.indexOf("curr:") + 5).toDouble();
}

void SimpleFOCDevice::parseMotionResponse(const QString& r)
{
    if(r.contains("downsample"))
        motionDownsample = r.mid(r.indexOf("downsample:") + 11).toDouble();
    else if(r.contains("haptic"))
        controlType = HAPTIC_CONTROL;
    else if(r.contains("torque"))
        controlType = TORQUE_CONTROL;
    else if(r.contains("angle open"))
        controlType = ANGLE_OPENLOOP_CONTROL;
    else if(r.contains("angle"))
        controlType = ANGLE_CONTROL;
    else if(r.contains("vel open"))
        controlType = VELOCITY_OPENLOOP_CONTROL;
    else if(r.contains("vel"))
        controlType = VELOCITY_CONTROL;
}

void SimpleFOCDevice::parseTorqueResponse(const QString& r)
{
    if(r.contains("volt"))
        torqueType = VOLTAGE_TORQUE;
    else if(r.contains("dc curr"))
        torqueType = DC_CURRENT_TORQUE;
    else if(r.contains("foc curr"))
        torqueType = FOC_CURRENT_TORQUE;
}

void SimpleFOCDevice::parseHapticResponse(const QString& r)
{
    if(r.contains("spring_det"))
        hapticPreset = 10;
    else if(r.contains("fine_nd"))
        hapticPreset = 5;
    else if(r.contains("coarse_str"))
        hapticPreset = 7;
    else if(r.contains("coarse_weak"))
        hapticPreset = 8;
    else if(r.contains("bounded10"))
        hapticPreset = 1;
    else if(r.contains("unbounded"))
        hapticPreset = 0;
    else if(r.contains("multirev"))
        hapticPreset = 2;
    else if(r.contains("onoff"))
        hapticPreset = 3;
    else if(r.contains("spring"))
        hapticPreset = 4;
    else if(r.contains("fine"))
        hapticPreset = 6;
    else if(r.contains("magnetic"))
        hapticPreset = 9;
}

void SimpleFOCDevice::parseSensorResponse(const QString& r)
{
    if(r.contains("el. offset"))
        sensorElectricalZero = r.mid(r.indexOf("el. offset:") + 11).toDouble();
    else if(r.contains("offset"))
        sensorZeroOffset = r.mid(r.indexOf("offset:") + 7).toDouble();
}

void SimpleFOCDevice::parseMonitorResponse(const QString& r)
{
    if(r.contains("all"))
    {
        QString varStr     = r.mid(r.indexOf("all:") + 4);
        QStringList states = varStr.trimmed().split('\t');
        if(states.size() >= 7)
        {
            targetNow   = states[0].toDouble();
            voltageQNow = states[1].toDouble();
            voltageDNow = states[2].toDouble();
            currentQNow = states[3].toDouble();
            currentDNow = states[4].toDouble();
            velocityNow = states[5].toDouble();
            angleNow    = states[6].toDouble();
        }
    }
    else if(r.contains("target"))
        targetNow = r.mid(r.indexOf("target:") + 7).toDouble();
    else if(r.contains("Vq"))
        voltageQNow = r.mid(r.indexOf("Vq:") + 3).toDouble();
    else if(r.contains("Vd"))
        voltageDNow = r.mid(r.indexOf("Vd:") + 3).toDouble();
    else if(r.contains("Cq"))
        currentQNow = r.mid(r.indexOf("Cq:") + 3).toDouble();
    else if(r.contains("Cd"))
        currentDNow = r.mid(r.indexOf("Cd:") + 3).toDouble();
    else if(r.contains("vel"))
        velocityNow = r.mid(r.indexOf("vel:") + 4).toDouble();
    else if(r.contains("angle"))
        angleNow = r.mid(r.indexOf("angle:") + 6).toDouble();
}

void SimpleFOCDevice::parsePWMModResponse(const QString& r)
{
    if(r.contains("center"))
        modulationCentered = r.mid(r.indexOf("center:") + 7).toDouble();
    else if(r.contains("type"))
    {
        QString t = r.mid(r.indexOf("type:") + 5);
        if(t.contains("Sine"))
            modulationType = SINE_PWM;
        else if(t.contains("SVPWM"))
            modulationType = SPACE_VECTOR_PWM;
        else if(t.contains("Trap 120"))
            modulationType = TRAPEZOIDAL_120;
        else if(t.contains("Trap 150"))
            modulationType = TRAPEZOIDAL_150;
    }
}

QString SimpleFOCDevice::toArduinoCode(const QList<bool>& gen) const
{
    QString code = "\n";
    bool genAll  = gen.isEmpty();

    auto check = [&](int i) { return genAll || (i < gen.size() && gen[i]); };

    if(check(0))
    {
        code += "// control loop type and torque mode \n";
        code += "motor.torque_controller = TorqueControlType::";
        switch(torqueType)
        {
        case VOLTAGE_TORQUE: code += "voltage"; break;
        case DC_CURRENT_TORQUE: code += "dc_current"; break;
        case FOC_CURRENT_TORQUE: code += "foc_current"; break;
        }
        code += ";\n";
        code += "motor.controller = MotionControlType::";
        switch(controlType)
        {
        case TORQUE_CONTROL: code += "torque"; break;
        case VELOCITY_CONTROL: code += "velocity"; break;
        case ANGLE_CONTROL: code += "angle"; break;
        case VELOCITY_OPENLOOP_CONTROL: code += "velocity_openloop"; break;
        case ANGLE_OPENLOOP_CONTROL: code += "angle_openloop"; break;
        case HAPTIC_CONTROL: code += "haptic"; break;
        }
        code += ";\n";
        code += QString("motor.motion_downsample = %1;\n\n").arg(motionDownsample);
    }

    if(check(1))
    {
        code += "// velocity loop PID\n";
        code += QString("motor.PID_velocity.P = %1;\n").arg(PIDVelocity.P);
        code += QString("motor.PID_velocity.I = %1;\n").arg(PIDVelocity.I);
        code += QString("motor.PID_velocity.D = %1;\n").arg(PIDVelocity.D);
        code += QString("motor.PID_velocity.output_ramp = %1;\n").arg(PIDVelocity.outputRamp);
        code += QString("motor.PID_velocity.limit = %1;\n").arg(PIDVelocity.outputLimit);
        code += "// Low pass filtering time constant \n";
        code += QString("motor.LPF_velocity.Tf = %1;\n").arg(LPFVelocity.Tf);
    }
    if(check(2))
    {
        code += "// angle loop PID\n";
        code += QString("motor.P_angle.P = %1;\n").arg(PIDAngle.P);
        code += QString("motor.P_angle.I = %1;\n").arg(PIDAngle.I);
        code += QString("motor.P_angle.D = %1;\n").arg(PIDAngle.D);
        code += QString("motor.P_angle.output_ramp = %1;\n").arg(PIDAngle.outputRamp);
        code += QString("motor.P_angle.limit = %1;\n").arg(PIDAngle.outputLimit);
        code += "// Low pass filtering time constant \n";
        code += QString("motor.LPF_angle.Tf = %1;\n").arg(LPFAngle.Tf);
    }
    if(check(3))
    {
        code += "// current q loop PID \n";
        code += QString("motor.PID_current_q.P = %1;\n").arg(PIDCurrentQ.P);
        code += QString("motor.PID_current_q.I = %1;\n").arg(PIDCurrentQ.I);
        code += QString("motor.PID_current_q.D = %1;\n").arg(PIDCurrentQ.D);
        code += QString("motor.PID_current_q.output_ramp = %1;\n").arg(PIDCurrentQ.outputRamp);
        code += QString("motor.PID_current_q.limit = %1;\n").arg(PIDCurrentQ.outputLimit);
        code += "// Low pass filtering time constant \n";
        code += QString("motor.LPF_current_q.Tf = %1;\n").arg(LPFCurrentQ.Tf);
    }
    if(check(4))
    {
        code += "// current d loop PID\n";
        code += QString("motor.PID_current_d.P = %1;\n").arg(PIDCurrentD.P);
        code += QString("motor.PID_current_d.I = %1;\n").arg(PIDCurrentD.I);
        code += QString("motor.PID_current_d.D = %1;\n").arg(PIDCurrentD.D);
        code += QString("motor.PID_current_d.output_ramp = %1;\n").arg(PIDCurrentD.outputRamp);
        code += QString("motor.PID_current_d.limit = %1;\n").arg(PIDCurrentD.outputLimit);
        code += "// Low pass filtering time constant \n";
        code += QString("motor.LPF_current_d.Tf = %1;\n").arg(LPFCurrentD.Tf);
    }
    if(check(5))
    {
        code += "// Limits \n";
        code += QString("motor.velocity_limit = %1;\n").arg(velocityLimit);
        code += QString("motor.voltage_limit = %1;\n").arg(voltageLimit);
        code += QString("motor.current_limit = %1;\n").arg(currentLimit);
    }
    if(check(6))
    {
        code += "// sensor zero offset - home position \n";
        code += QString("motor.sensor_offset = %1;\n").arg(sensorZeroOffset);
    }
    if(check(7))
    {
        code += "// sensor zero electrical angle \n";
        code += QString("motor.sensor_electrical_offset = %1;\n").arg(sensorElectricalZero);
    }
    if(check(8))
    {
        code += "// general settings \n";
        code += QString("motor.phase_resistance = %1;\n").arg(phaseResistance);
    }
    if(check(9))
    {
        code += "// pwm modulation settings \n";
        code += "motor.foc_modulation = FOCModulationType::";
        switch(modulationType)
        {
        case SINE_PWM: code += "SinePWM"; break;
        case SPACE_VECTOR_PWM: code += "SpaceVectorPWM"; break;
        case TRAPEZOIDAL_120: code += "Trapezoid_120"; break;
        case TRAPEZOIDAL_150: code += "Trapezoid_150"; break;
        }
        code += ";\n";
        code += QString("motor.modulation_centered = %1;\n").arg(modulationCentered);
    }

    return code;
}
