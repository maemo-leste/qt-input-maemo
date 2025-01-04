#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  // ui->line->setInputMethodHints(Qt::ImhDigitsOnly);
  this->appendQuickView();

  this->show();
}

void MainWindow::appendQuickView() {
  m_view = new QQuickWidget(this);
  m_view->setResizeMode(QQuickWidget::SizeRootObjectToView);
  m_view->setSource(QUrl(QStringLiteral("qrc:/main.qml")));


  QWidget *centralWidget = ui->centralWidget;
  QLayout *existingLayout = centralWidget->layout();
  existingLayout->addWidget(m_view);

  QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_view->setSizePolicy(policy);
}

void MainWindow::appendWebView() {
  // m_view = new QWebEngineView(this);
  //
  // m_view->setUrl(QUrl("https://www.google.com/"));
  //
  // QWidget *centralWidget = ui->centralWidget;
  // QLayout *existingLayout = centralWidget->layout();
  // existingLayout->addWidget(m_view);
  //
  // QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  // m_view->setSizePolicy(policy);
}

void MainWindow::onQuitApplication() {
  this->close();
}

MainWindow::~MainWindow() {
  delete ui;
}

