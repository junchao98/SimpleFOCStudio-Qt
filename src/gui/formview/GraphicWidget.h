#ifndef GRAPHICWIDGET_H
#define GRAPHICWIDGET_H

#include <QWidget>
#include <QtCharts>
#include <QVector>
#include <QCheckBox>
#include <QPushButton>
#include <QSpinBox>

QT_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_END_NAMESPACE

class SimpleFOCDevice;
class ControlPlotPanel;

class GraphicWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GraphicWidget(QWidget *parent = nullptr);

    static const int BUFFER_SIZE = 300;

    enum PlotState {
        DisconnectedState = 0,
        InitialConnectedState = 1,
        ConnectedPausedState = 2,
        ConnectedPlottingStartedState = 3
    };

public slots:
    void onStartStop();
    void onPauseContinue();
    void onViewAll();
    void onChannelToggled(int index, bool checked);
    void onDownsampleChanged(int value);
    void onMonitoringDataReceived(const QList<double> &data);
    void onConnectionStateChanged(bool connected);

private:
    void updatePlot();
    void sendMonitorSetup();

    QChartView *m_chartView;
    QChart *m_chart;
    QLineSeries *m_series[7];
    QVector<double> m_buffers[7];
    double m_xAxis;
    int m_state = DisconnectedState;

    bool m_channelEnabled[7] = {true, false, false, false, false, false, true};
    int m_downsample = 100;

    SimpleFOCDevice *m_device;
    ControlPlotPanel *m_controlPanel;
};

class ControlPlotPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ControlPlotPanel(QWidget *parent = nullptr);

    QPushButton *startStopBtn() const { return m_startStopBtn; }
    QPushButton *pauseBtn() const { return m_pauseBtn; }
    QPushButton *viewAllBtn() const { return m_viewAllBtn; }
    QSpinBox *downsampleSpin() const { return m_downsampleSpin; }
    QCheckBox *channelCheckbox(int i) const { return m_channelChecks[i]; }

signals:
    void startStopClicked();
    void pauseContinueClicked();
    void viewAllClicked();
    void channelToggled(int index, bool checked);
    void downsampleChanged(int value);

private:
    QPushButton *m_startStopBtn;
    QPushButton *m_pauseBtn;
    QPushButton *m_viewAllBtn;
    QSpinBox *m_downsampleSpin;
    QCheckBox *m_channelChecks[7];
};

#endif
