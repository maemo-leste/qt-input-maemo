#pragma once
#include <QtCore>
#include <QApplication>
#include <QCoreApplication>

#include <QInputMethodEvent>
#include <QTextFormat>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QGraphicsObject>
#include <QGraphicsView>

#include "event_filter.h"
#include "utils-hildon.h"
#include "utils-xcb.h"
#include "qtkey.h"

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
  QWidget *focusWidget();
  void insertUtf8(int flag, const QString& text);

  //void showSoftKeyboard();
  void showInputPanel() override;
  void hideInputPanel() override;
  ~QHildonInputContext() override;

  bool is_gui = false;
  bool show_again = true;

  void setFocusObject(QObject *object) override;
  [[nodiscard]] bool isValid() const override { return true; }

private:
  // context
  void checkSentenceStart();
  void commitPreeditBuffer();
  void sendSurrounding(bool sendAllContents = false);
  void sendInputMode() const;
  void setClientCursorLocation(bool offsetIsRelative, int cursorOffset);
  void setCommitMode(HildonIMCommitMode mode, bool clearPreEdit = true);

  void setMaskState(int *mask,
                           HildonIMInternalModifierMask lock_mask,
                           HildonIMInternalModifierMask sticky_mask,
                           bool was_press_and_release) const;
  void updateInputMethodHints();

  static void sendKey(QWidget *keywidget, int qtCode);
  static bool qt_sendSpontaneousEvent(QObject *receiver, QEvent *event);
  static QGraphicsObject* qDeclarativeTextEdit_cast(QWidget *w);

private:
  QHildonEventFilter *m_eventFilter;
  int m_inputMode = 0;
  QWidget* m_currentFocus = nullptr;
  bool m_lastInternalChange;
  int m_mask;
  int m_options;
  bool m_autoUpper;
  bool lastInternalChange;
  bool m_spaceAfterCommit;
  int m_lastQtKeyCode;

  QString m_preEditBuffer;

  HildonIMTrigger m_triggerMode = HILDON_IM_TRIGGER_KEYBOARD;
  HildonIMCommitMode m_commitMode, m_lastCommitMode;
};
