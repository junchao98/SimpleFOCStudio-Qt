#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>

class WorkAreaTabWidget;

class ToolBar : public QToolBar
{
    Q_OBJECT

public:
    explicit ToolBar(const QString &title, QWidget *parent, WorkAreaTabWidget *tabWidget);
};

#endif
