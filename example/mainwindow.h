#pragma once

#include <iostream>

#include <QMainWindow>
#include <QCompleter>
#include <QPushButton>
#include <QClipboard>
// #include <QtWebEngineCore>
// #include <QWebEngineView>
#include <QQuickWidget>
#include <QQmlContext>
#include <QStringListModel>
#include <QTimer>
#include <QDebug>
#include <QUrl>
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
  void appendQuickView();
  void appendWebView();
  ~MainWindow() override;

  //QHildonInputContext* plugin = nullptr;

public slots:
    void onQuitApplication();

private:
  Ui::MainWindow *ui;
  QTimer* m_timer;

  QQuickWidget* m_view;
  //QWebEngineView* m_view;

};
