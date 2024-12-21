#include <QObject>
#include <QApplication>

#include <unistd.h>
#include <sys/types.h>
#include "globals.h"
#include "mainwindow.h"

#ifdef DEBUG
  #include "clion_debug.h"
#endif

int main(int argc, char *argv[]) {
#ifdef DEBUG
  clion_debug_setup();
#endif

  // dont use maemo xcb QPA plugin
  qputenv("QT_QPA_PLATFORM", QByteArray(""));

  // force input module to 'him'
  qputenv("QT_IM_MODULE", QByteArray("him"));

#ifdef DEBUG
  qputenv("QT_DEBUG_PLUGINS", QByteArray("0"));
#endif

  qputenv("QT_STYLE_OVERRIDE", QByteArray("maemo5"));

  // append ${CMAKE_BINARY_DIR/qt} to Qt plugin library path
  QCoreApplication::addLibraryPath(QT_CUSTOM_PLUGIN_PATH);
  qDebug() << "Qt plugin paths:" << QCoreApplication::libraryPaths();

  QApplication::setApplicationName("example");
  QApplication::setOrganizationDomain("https://maemo-leste.github.io/");
  QApplication::setOrganizationName("Maemo Leste");
  QApplication app(argc, argv);

  auto *mainWindow = new MainWindow();
  return QApplication::exec();
}
