#include <QtTest/QtTest>
#include <QApplication>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QPushButton>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QTabWidget>
#include <QTextEdit>
#include <QGroupBox>
#include <QChartView>
#include <QChart>
#include <QLineSeries>

#include "gui/MainWindow.h"
#include "core/SimpleFOCDevice.h"

class TestUI : public QObject
{
    Q_OBJECT

    private slots:
    void testOpenSerialConnection();
    void testEnableDisableMotor();
};

// Locate the read-only QTextEdit inside the "Command Line interface" QGroupBox
static QTextEdit* findCommTextDisplay(QWidget* root)
{
    auto groupBoxList = root->findChildren<QGroupBox*>();
    for(auto* box : groupBoxList)
    {
        if(box->title() == "Command Line interface")
        {
            return box->findChild<QTextEdit*>();
        }
    }
    return nullptr;
}

static QPushButton* findButtonByText(QWidget* root, const QString& text)
{
    auto buttons = root->findChildren<QPushButton*>();
    for(auto* btn : buttons)
    {
        if(btn->text() == text)
            return btn;
    }
    return nullptr;
}

// Icon-only buttons have no text, so match by tooltip instead.
static QPushButton* findButtonByToolTip(QWidget* root, const QString& toolTip)
{
    auto buttons = root->findChildren<QPushButton*>();
    for(auto* btn : buttons)
    {
        if(btn->toolTip() == toolTip)
            return btn;
    }
    return nullptr;
}

void TestUI::testOpenSerialConnection()
{
    MainWindow window;
    window.show();
    QVERIFY(QTest::qWaitForWindowExposed(&window));

    // --- Step 1: Trigger "Tree View" via toolbar menu action ---
    // QToolBar creates internal QToolButtons for its actions, so findChild<QToolButton*>
    // is unreliable. Instead, find the QAction by its text directly.
    auto* toolbar = window.findChild<QToolBar*>();
    QVERIFY(toolbar);

    QAction* treeAction = nullptr;
    for(auto* action : window.findChildren<QAction*>())
    {
        if(action->text() == "Tree View")
        {
            treeAction = action;
            break;
        }
    }
    QVERIFY2(treeAction, "Tree View action not found");
    treeAction->trigger();
    QTest::qWait(200);

    auto* tabWidget = window.findChild<QTabWidget*>();
    QVERIFY(tabWidget);
    bool tabFound = false;
    for(int i = 0; i < tabWidget->count(); i++)
    {
        if(tabWidget->tabText(i) == "Device")
        {
            tabFound = true;
            break;
        }
    }
    QVERIFY2(tabFound, "Device tab was not created");

    // --- Step 2: Click "Configure" -> handle modal dialog ---
    auto* configureBtn = findButtonByText(&window, "Configure");
    QVERIFY2(configureBtn, "Configure button not found");

    // Because ConfigureConnectionDialog::exec() blocks the event loop,
    // we use QTimer::singleShot to interact with the dialog after it appears.
    const QString targetPort = "ttyUSB4";
    QTimer::singleShot(500, [&]() {
        auto* dlg = qApp->activeModalWidget();
        QVERIFY2(dlg, "Configure dialog did not appear");

        auto* portCombo = dlg->findChild<QComboBox*>();
        QVERIFY2(portCombo, "Port combo not found in dialog");

        // ttyUSB4 may not be in the system's available-port list,
        // so inject it manually for testing purposes.
        portCombo->addItem(targetPort);
        portCombo->setCurrentText(targetPort);

        auto* btnBox = dlg->findChild<QDialogButtonBox*>();
        QVERIFY2(btnBox, "Dialog button box not found");
        QTest::mouseClick(btnBox->button(QDialogButtonBox::Ok), Qt::LeftButton);
    });

    QTest::mouseClick(configureBtn, Qt::LeftButton);
    QTest::qWait(1000);

    // Verify the port name was stored on the device singleton
    QCOMPARE(SimpleFOCDevice::instance()->serialPortName, targetPort);

    // --- Step 3: Click "Connect" ---
    auto* connectBtn = findButtonByText(&window, "Connect");
    QVERIFY2(connectBtn, "Connect button not found");

    // If the port doesn't exist, connectDevice() shows a QMessageBox.
    // Dismiss it so the test doesn't hang. No-op if connection succeeds.
    QTimer::singleShot(500, [&]() {
        auto* msgBox = qobject_cast<QMessageBox*>(qApp->activeModalWidget());
        if(msgBox)
            msgBox->close();
    });

    QTest::mouseClick(connectBtn, Qt::LeftButton);
    QTest::qWait(1000);

    // Verify the connection state is reflected in the UI: when connected,
    // Connect button is disabled and Disconnect is enabled.
    bool connected = SimpleFOCDevice::instance()->getIsConnected();
    QCOMPARE(!connectBtn->isEnabled(), connected);
    QCOMPARE(findButtonByText(&window, "Disconnect")->isEnabled(), connected);

    if(!connected)
        return;

    // --- Step 4: Read communication text from the command line display ---
    // After PULL_CONFIG connect, the device sends config-pull commands and
    // the responses appear in the read-only QTextEdit inside CommandLineWidget.
    QTest::qWait(2000); // allow time for async serial responses to arrive

    auto* commDisplay = findCommTextDisplay(&window);
    QVERIFY2(commDisplay, "Command line text display not found");

    QString commText = commDisplay->toPlainText();
    qDebug() << "=== Communication log ===\n" << commText;
    qDebug() << "=== End of log ===";

    QVERIFY2(!commText.isEmpty(), "No communication data received from device");

    // --- Step 5: Start real-time monitoring -> verify chart gets data ---
    auto* startBtn = findButtonByToolTip(&window, "Start");
    QVERIFY2(startBtn, "Monitor Start button not found");

    QTest::mouseClick(startBtn, Qt::LeftButton);
    QVERIFY2(startBtn->toolTip() == "Stop", "Start button did not toggle to Stop");

    QTest::qWait(3000); // wait for monitoring data to stream in and chart to repaint

    auto* chartView = window.findChild<QChartView*>();
    QVERIFY2(chartView, "QChartView not found");
    auto* chart = chartView->chart();
    QVERIFY2(chart, "QChart not found inside QChartView");

    bool hasData    = false;
    int totalPoints = 0;
    for(auto* series : chart->series())
    {
        auto* ls = qobject_cast<QLineSeries*>(series);
        if(ls && ls->count() > 0)
        {
            hasData = true;
            totalPoints += ls->count();
            qDebug() << "Series:" << ls->name() << "points:" << ls->count();
        }
    }
    qDebug() << "Total data points across all series:" << totalPoints;
    QVERIFY2(hasData, "Chart has no data points after starting monitoring");

    // Cleanup: disconnect
    SimpleFOCDevice::instance()->disconnectDevice();
}

void TestUI::testEnableDisableMotor()
{
    MainWindow window;
    window.show();
    QVERIFY(QTest::qWaitForWindowExposed(&window));

    // Open Tree View
    QAction* treeAction = nullptr;
    for(auto* action : window.findChildren<QAction*>())
    {
        if(action->text() == "Tree View")
        {
            treeAction = action;
            break;
        }
    }
    QVERIFY2(treeAction, "Tree View action not found");
    treeAction->trigger();
    QTest::qWait(200);

    // Configure serial port
    const QString targetPort = "ttyUSB4";
    auto* configureBtn       = findButtonByText(&window, "Configure");
    QVERIFY2(configureBtn, "Configure button not found");
    QTimer::singleShot(500, [&]() {
        auto* dlg = qApp->activeModalWidget();
        QVERIFY2(dlg, "Configure dialog did not appear");
        auto* portCombo = dlg->findChild<QComboBox*>();
        portCombo->addItem(targetPort);
        portCombo->setCurrentText(targetPort);
        auto* btnBox = dlg->findChild<QDialogButtonBox*>();
        QTest::mouseClick(btnBox->button(QDialogButtonBox::Ok), Qt::LeftButton);
    });
    QTest::mouseClick(configureBtn, Qt::LeftButton);
    QTest::qWait(1000);

    // Connect
    auto* connectBtn = findButtonByText(&window, "Connect");
    QVERIFY2(connectBtn, "Connect button not found");
    QTimer::singleShot(500, [&]() {
        auto* msgBox = qobject_cast<QMessageBox*>(qApp->activeModalWidget());
        if(msgBox)
            msgBox->close();
    });
    QTest::mouseClick(connectBtn, Qt::LeftButton);
    QTest::qWait(2000); // wait for pull config + state polling

    QVERIFY2(SimpleFOCDevice::instance()->getIsConnected(), "Device not connected");

    // Find the enable/disable toggle button (text changes between the two)
    auto findEnableBtn = [&]() -> QPushButton* {
        for(auto* btn : window.findChildren<QPushButton*>())
        {
            if(btn->text() == "Enable Device" || btn->text() == "Disable Device")
                return btn;
        }
        return nullptr;
    };

    auto* enableBtn = findEnableBtn();
    QVERIFY2(enableBtn, "Enable/Disable Device button not found");

    // Cycle enable/disable several times — this reproduces the crash scenario
    for(int i = 0; i < 5; ++i)
    {
        int before = SimpleFOCDevice::instance()->deviceStatus;
        qDebug() << "Cycle" << (i + 1) << "status before:" << before
                 << "| button:" << enableBtn->text();

        QTest::mouseClick(enableBtn, Qt::LeftButton);
        QTest::qWait(500); // allow serial response + UI update

        int after = SimpleFOCDevice::instance()->deviceStatus;
        qDebug() << "  status after:" << after << "| button:" << enableBtn->text();
    }

    // If we reach here the app did not crash
    QVERIFY2(SimpleFOCDevice::instance()->getIsConnected(),
             "Lost connection during enable/disable cycling");

    SimpleFOCDevice::instance()->disconnectDevice();
}

QTEST_MAIN(TestUI)
#include "test_ui.moc"
