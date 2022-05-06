#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_qtmapsdemowindow.h"

class QtMapsDemoWindow : public QMainWindow
{
    Q_OBJECT

public:
    QtMapsDemoWindow(QWidget *parent = Q_NULLPTR);

private:
    Ui::qtmapsdemowindowClass ui;
};
