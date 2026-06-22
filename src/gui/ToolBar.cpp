#include "ToolBar.h"
#include "WorkAreaTabWidget.h"
#include "gui/shared/GUIToolKit.h"
#include <QToolButton>
#include <QMenu>
#include <QAction>

ToolBar::ToolBar(const QString& title, QWidget* parent, WorkAreaTabWidget* tabWidget)
    : QToolBar(title, parent)
{
    auto* addDeviceBtn = new QToolButton(this);
    addDeviceBtn->setIcon(GUIToolKit::getIconByName("add_motor"));
    addDeviceBtn->setPopupMode(QToolButton::InstantPopup);
    addDeviceBtn->setToolTip("Add Device");

    auto* addMenu    = new QMenu(addDeviceBtn);
    auto* treeAction = addMenu->addAction(GUIToolKit::getIconByName("tree"), "Tree View");
    auto* formAction = addMenu->addAction(GUIToolKit::getIconByName("form"), "Form View");

    connect(treeAction, &QAction::triggered, tabWidget, &WorkAreaTabWidget::addDeviceTree);
    connect(formAction, &QAction::triggered, tabWidget, &WorkAreaTabWidget::addDeviceForm);

    addDeviceBtn->setMenu(addMenu);
    addWidget(addDeviceBtn);

    auto* openAction = addAction(GUIToolKit::getIconByName("open"), "Open");
    connect(openAction, &QAction::triggered, tabWidget, &WorkAreaTabWidget::openDevice);

    auto* saveAction = addAction(GUIToolKit::getIconByName("save"), "Save");
    connect(saveAction, &QAction::triggered, tabWidget, &WorkAreaTabWidget::saveDevice);

    addSeparator();

    auto* consoleAction = addAction(GUIToolKit::getIconByName("consoletool"), "Serial Console");
    consoleAction->setToolTip("Open Serial Console tool");
    connect(consoleAction, &QAction::triggered, tabWidget, &WorkAreaTabWidget::openConsoleTool);

    addSeparator();
}
