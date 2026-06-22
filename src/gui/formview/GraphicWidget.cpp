#include "GraphicWidget.h"
#include "core/SimpleFOCDevice.h"
#include "gui/shared/GUIToolKit.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QValueAxis>
#include <QAreaSeries>
#include <QRandomGenerator>
#include <cmath>

// QtCharts types are available via the <QtCharts> include in the header

static const QColor channelColors[7] = {QColor(255, 92, 92),
                                        QColor(91, 141, 236),
                                        QColor(75, 0, 130),
                                        QColor(255, 255, 51),
                                        QColor(222, 184, 135),
                                        QColor(253, 172, 66),
                                        QColor(57, 217, 138)};

static const QString channelNames[7] = {"Target",
                                        "Vq [Volts]",
                                        "Vd [Volts]",
                                        "Cq [milliAmps]",
                                        "Cd [milliAmps]",
                                        "Vel [rad/sec]",
                                        "Angle [rad]"};

static const QStringList dotIcons = {
    "reddot", "bluedot", "purpledot", "yellowdot", "maroondot", "orangedot", "greendot"};

ControlPlotPanel::ControlPlotPanel(QWidget* parent) : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_startStopBtn = new QPushButton();
    m_startStopBtn->setIcon(GUIToolKit::getIconByName("start"));
    m_startStopBtn->setToolTip("Start");
    m_startStopBtn->setFixedSize(30, 30);
    connect(m_startStopBtn, &QPushButton::clicked, this, &ControlPlotPanel::startStopClicked);
    layout->addWidget(m_startStopBtn);

    m_pauseBtn = new QPushButton();
    m_pauseBtn->setIcon(GUIToolKit::getIconByName("pause"));
    m_pauseBtn->setToolTip("Pause");
    m_pauseBtn->setFixedSize(30, 30);
    connect(m_pauseBtn, &QPushButton::clicked, this, &ControlPlotPanel::pauseContinueClicked);
    layout->addWidget(m_pauseBtn);

    m_viewAllBtn = new QPushButton();
    m_viewAllBtn->setIcon(GUIToolKit::getIconByName("zoomall"));
    m_viewAllBtn->setToolTip("View all");
    m_viewAllBtn->setFixedSize(30, 30);
    connect(m_viewAllBtn, &QPushButton::clicked, this, &ControlPlotPanel::viewAllClicked);
    layout->addWidget(m_viewAllBtn);

    for(int i = 0; i < 7; ++i)
    {
        m_channelChecks[i] = new QCheckBox();
        m_channelChecks[i]->setIcon(GUIToolKit::getIconByName(dotIcons[i]));
        m_channelChecks[i]->setText(channelNames[i]);
        m_channelChecks[i]->setChecked(i == 0 || i == 1 || i == 5 || i == 6);
        connect(m_channelChecks[i], &QCheckBox::toggled, [this, i](bool checked) {
            emit channelToggled(i, checked);
        });
        layout->addWidget(m_channelChecks[i]);
    }

    layout->addStretch();

    layout->addWidget(new QLabel("Downsample"));
    m_downsampleSpin = new QSpinBox();
    m_downsampleSpin->setRange(1, 10000);
    m_downsampleSpin->setValue(100);
    connect(m_downsampleSpin,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            &ControlPlotPanel::downsampleChanged);
    layout->addWidget(m_downsampleSpin);
}

GraphicWidget::GraphicWidget(QWidget* parent)
    : QWidget(parent), m_device(SimpleFOCDevice::instance()), m_xAxis(0)
{
    auto* mainLayout = new QVBoxLayout(this);

    auto* titleLabel = new QLabel("Real time motor variables:");
    mainLayout->addWidget(titleLabel);

    m_chart = new QChart();
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);

    auto* axisX = new QValueAxis();
    axisX->setRange(-BUFFER_SIZE, 0);
    axisX->setTitleText("Samples");
    m_chart->addAxis(axisX, Qt::AlignBottom);

    auto* axisY = new QValueAxis();
    axisY->setRange(-100, 100);
    axisY->setTitleText("Value");
    m_chart->addAxis(axisY, Qt::AlignLeft);

    for(int i = 0; i < 7; ++i)
    {
        m_series[i] = new QLineSeries();
        m_series[i]->setName(channelNames[i]);
        QPen pen(channelColors[i]);
        pen.setWidth(2);
        m_series[i]->setPen(pen);
        m_series[i]->setVisible(m_channelEnabled[i]);

        m_chart->addSeries(m_series[i]);
        m_series[i]->attachAxis(axisX);
        m_series[i]->attachAxis(axisY);

        m_buffers[i].resize(BUFFER_SIZE, 0);
    }

    m_chartView = new QChartView(m_chart, this);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setMinimumHeight(250);
    mainLayout->addWidget(m_chartView);

    m_controlPanel = new ControlPlotPanel(this);
    mainLayout->addWidget(m_controlPanel);

    connect(m_controlPanel, &ControlPlotPanel::startStopClicked, this, &GraphicWidget::onStartStop);
    connect(m_controlPanel,
            &ControlPlotPanel::pauseContinueClicked,
            this,
            &GraphicWidget::onPauseContinue);
    connect(m_controlPanel, &ControlPlotPanel::viewAllClicked, this, &GraphicWidget::onViewAll);
    connect(
        m_controlPanel, &ControlPlotPanel::channelToggled, this, &GraphicWidget::onChannelToggled);
    connect(m_controlPanel,
            &ControlPlotPanel::downsampleChanged,
            this,
            &GraphicWidget::onDownsampleChanged);

    connect(m_device,
            &SimpleFOCDevice::monitoringDataReceived,
            this,
            &GraphicWidget::onMonitoringDataReceived);
    connect(m_device,
            &SimpleFOCDevice::connectionStateChanged,
            this,
            &GraphicWidget::onConnectionStateChanged);
}

void GraphicWidget::onMonitoringDataReceived(const QList<double>& data)
{
    if(data.size() >= 7)
    {
        for(int i = 0; i < 7; ++i)
        {
            m_buffers[i].removeFirst();
            m_buffers[i].append(data[i]);
        }
    }
    if(m_state == ConnectedPlottingStartedState)
    {
        updatePlot();
    }
}

void GraphicWidget::updatePlot()
{
    for(int i = 0; i < 7; ++i)
    {
        if(!m_channelEnabled[i])
            continue;
        QVector<QPointF> points;
        points.reserve(BUFFER_SIZE);
        for(int j = 0; j < BUFFER_SIZE; ++j)
        {
            points.append(QPointF(j - BUFFER_SIZE, m_buffers[i][j]));
        }
        m_series[i]->replace(points);
    }
}

void GraphicWidget::onStartStop()
{
    if(m_state == DisconnectedState)
        return;

    if(m_state == ConnectedPlottingStartedState || m_state == ConnectedPausedState)
    {
        m_state = InitialConnectedState;
        m_controlPanel->startStopBtn()->setIcon(GUIToolKit::getIconByName("start"));
        m_controlPanel->startStopBtn()->setToolTip("Start");
        m_device->sendMonitorDownsample(0);
    }
    else
    {
        m_state = ConnectedPlottingStartedState;
        m_controlPanel->startStopBtn()->setIcon(GUIToolKit::getIconByName("stop"));
        m_controlPanel->startStopBtn()->setToolTip("Stop");
        sendMonitorSetup();
    }
}

void GraphicWidget::onPauseContinue()
{
    if(m_state == ConnectedPlottingStartedState)
    {
        m_state = ConnectedPausedState;
        m_controlPanel->pauseBtn()->setIcon(GUIToolKit::getIconByName("continue"));
        m_controlPanel->pauseBtn()->setToolTip("Continue");
    }
    else if(m_state == ConnectedPausedState)
    {
        m_state = ConnectedPlottingStartedState;
        m_controlPanel->pauseBtn()->setIcon(GUIToolKit::getIconByName("pause"));
        m_controlPanel->pauseBtn()->setToolTip("Pause");
    }
}

void GraphicWidget::onViewAll()
{
    auto axes = m_chart->axes(Qt::Vertical);
    if(!axes.isEmpty())
        axes.first()->setRange(NAN, NAN);
}

void GraphicWidget::onChannelToggled(int index, bool checked)
{
    m_channelEnabled[index] = checked;
    m_series[index]->setVisible(checked);
    sendMonitorSetup();
}

void GraphicWidget::onDownsampleChanged(int value)
{
    m_downsample = value;
    if(m_state == ConnectedPlottingStartedState)
        m_device->sendMonitorDownsample(value);
}

void GraphicWidget::onConnectionStateChanged(bool connected)
{
    if(connected)
    {
        m_state = InitialConnectedState;
    }
    else
    {
        m_state = DisconnectedState;
        for(int i = 0; i < 7; ++i)
            m_buffers[i].fill(0);
    }
}

void GraphicWidget::sendMonitorSetup()
{
    if(!m_device->getIsConnected())
        return;

    QList<int> vars;
    for(int i = 0; i < 7; ++i)
        vars.append(m_channelEnabled[i] ? 1 : 0);

    m_device->sendMonitorVariables(vars);
    m_device->sendMonitorDownsample(m_downsample);
}
