#include "MainWindow.h"
#include "WorkAreaTabWidget.h"
#include "ToolBar.h"
#include <QStatusBar>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("SimpleFOC Configuration Tool");
    resize(1300, 900);

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);

    m_tabWidget = new WorkAreaTabWidget(central);
    layout->addWidget(m_tabWidget);

    m_toolBar = new ToolBar("Main Toolbar", this, m_tabWidget);
    addToolBar(Qt::TopToolBarArea, m_toolBar);

    setStatusBar(new QStatusBar(this));
    setCentralWidget(central);
}
