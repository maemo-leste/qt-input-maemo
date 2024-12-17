#pragma once
#include <QtCore>

#include "event_filter.h"
#include "utils-hildon.h"
#include "utils-xcb.h"

// #include "lib/xcb-maemo/qxcbintegration.h"
// #include "lib/xcb-maemo/qxcbconnection.h"

#include <qpa/qplatforminputcontext.h>
#include <xcb/xcb.h>

class MyXcbEventFilter;
class QHildonInputContext : public QPlatformInputContext {
public:
  QHildonInputContext();
  static void show_vkb(const QWidget *w);
  static void sendHildonCommand(HildonIMCommand cmd, const QWidget *widget = nullptr);
  static void sendInputMode();
  ~QHildonInputContext() override;

  bool is_gui = false;
  bool show_again = true;

  void setFocusObject(QObject *object) override;
  [[nodiscard]] bool isValid() const override { return true; }

private:
  QHildonEventFilter *m_eventFilter;
};
