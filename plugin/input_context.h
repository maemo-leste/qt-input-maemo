#pragma once
#include <QObject>
#include <QTimer>
#include <QPointer>
#include <QApplication>
#include <QCoreApplication>
#include <QEvent>
#include <QKeyEvent>
#include <QInputMethodEvent>
#include <QTextFormat>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QGraphicsObject>
#include <QClipboard>
#include <QGraphicsView>

#include <qpa/qplatforminputcontext.h>

#include "event_filter.h"
#include "utils-hildon.h"

class QHildonInputContext : public QPlatformInputContext {
public:
  QHildonInputContext();
  void sendHildonCommand(HildonIMCommand cmd, const QWidget *widget = nullptr) const;
  bool parseHildonCommand(xcb_client_message_event_t *event);

  // void showSoftKeyboard();
  void showInputPanel() override;
  void hideInputPanel() override;
  ~QHildonInputContext() override;

  bool is_gui = false; // Qt app is GUI or console?
  // bool show_again = true;

  void setFocusObject(QObject *object) override;
  [[nodiscard]] bool isValid() const override { return true; }

private slots:
  void onLongPressDetected();
private:
  [[nodiscard]] QWidget* focusWidget() const { return m_currentFocus; }
  void insertUtf8(int flag, const QString& text);
  void cancelPreedit();
  void checkSentenceStart();
  void commitPreEditBuffer();
  void clearSelection() const;
  void sendSurrounding(bool sendAllContents = false);
  void sendInputMode() const;
  void setClientCursorLocation(bool offsetIsRelative, int cursorOffset);
  void setCommitMode(HildonIMCommitMode mode, bool clearPreEdit = true);
  void setMaskState(int *mask,
    HildonIMInternalModifierMask lock_mask,
    HildonIMInternalModifierMask sticky_mask,
    bool was_press_and_release) const;
  void updateInputMethodHints();
private:
  static void sendKey(QWidget *keywidget, int qtCode);
  static bool qt_sendSpontaneousEvent(QObject *receiver, QEvent *event);
  static QGraphicsObject* qDeclarativeTextEdit_cast(QWidget *w);
  static void answerClipboardSelectionQuery(const QWidget *widget);
private:
  QHildonEventFilter *m_eventFilter;
  int m_inputMode = 0;
  QWidget* m_currentFocus = nullptr;
  QWidget* m_lastKeyWidget = nullptr;
  QString m_lastCommitString;
  bool m_lastInternalChange;
  int m_mask;
  int m_options;
  int m_textCursorPosOnPress;  //position of the cursor in the surrounding text at the last TabletPress event
  bool m_autoUpper;
  bool lastInternalChange;
  bool m_spaceAfterCommit;
  int m_lastQtKeyCode;
  QTimer *m_longPressTimer;

  QString m_preEditBuffer;

  HildonIMTrigger m_triggerMode = HILDON_IM_TRIGGER_KEYBOARD;
  HildonIMCommitMode m_commitMode;
  HildonIMCommitMode m_lastCommitMode;
};
