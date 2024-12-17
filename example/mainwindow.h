#pragma once

#include <iostream>

#include <QMainWindow>
#include <QCompleter>
#include <QPushButton>
#include <QClipboard>
#include <QStringListModel>
#include <QTimer>
#include <QDebug>
#include <QQueue>
#include <QThread>
#include <QMutex>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

  //QHildonInputContext* plugin = nullptr;

public slots:
    void onQuitApplication();

private:
  Ui::MainWindow *ui;
};
