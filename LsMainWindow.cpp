#include "LsMainWindow.h"
#include "ui_LsMainWindow.h"

LsMainWindow::LsMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LsMainWindow)
{
    ui->setupUi(this);
}

LsMainWindow::~LsMainWindow()
{
    delete ui;
}
