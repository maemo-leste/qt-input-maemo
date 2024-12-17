#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  this->show();

  // plugin = new QHildonInputContext();
  // QWidget *parentWidget = ui->line->parentWidget();
  // plugin->setFocusObject(parentWidget);

  // connect(ui->btn, &QPushButton::clicked, [this]{
  //   QWidget *w = ui->line->parentWidget();
  //   ui->line->setFocus();
  //   // plugin->lol(w);
  // });
}

void MainWindow::onQuitApplication() {
  this->close();
}

MainWindow::~MainWindow() {
  delete ui;
}

