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
  void sendHildonCommand(HildonIMCommand cmd, const QWidget *widget = nullptr) const;
  bool parseHildonCommand(xcb_client_message_event_t *event);
  void sendInputMode() const;
  void updateInputMethodHints();
  void showSoftKeyboard();
  // bool filterEvent(const QEvent *event) override;
  void showInputPanel() override;
  void hideInputPanel() override;
  ~QHildonInputContext() override;

  bool is_gui = false;
  bool show_again = true;

  void setFocusObject(QObject *object) override;
  [[nodiscard]] bool isValid() const override { return true; }

private:
  QHildonEventFilter *m_eventFilter;
  int m_inputMode = 0;
  HildonIMTrigger m_triggerMode = HILDON_IM_TRIGGER_KEYBOARD;
  QWidget* m_currentFocus = nullptr;
};
