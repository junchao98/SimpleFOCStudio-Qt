#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class WorkAreaTabWidget;
class ToolBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    WorkAreaTabWidget *m_tabWidget;
    ToolBar *m_toolBar;
};

#endif
