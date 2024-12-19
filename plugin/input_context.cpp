#include <QGuiApplication>
#include <QObject>
#include <QWidget>

#include "input_context.h"

QHildonInputContext::QHildonInputContext() {
  qDebug() << "QHildonInputContext created";

  // xcb connection for this application
  QXcb::CONNECTION = QX11Info::connection();
  if (!QXcb::CONNECTION)
    qWarning() << "Failed to retrieve xcb_connection_t*";

  // install native event filter
  m_eventFilter = new QHildonEventFilter(this);
  if (const auto currentApp = qGuiApp) {
    currentApp->installNativeEventFilter(m_eventFilter);
    is_gui = true;
  } else {
    std::cerr << "No application!" << std::endl;
  }

  // generate atoms lookup
  QXcb::initialiseAtoms();
}

bool QHildonInputContext::parseHildonCommand(xcb_client_message_event_t *event) {
  QHildonIMAtom msg_atom = HILDON_ATOM_MAP[event->type];
  const auto message_format = event->format;

  qDebug() << "HIM: parse" << HILDON_ATOM_MAP[event->type].key;

  if (msg_atom.hildon_enum == HILDON_IM_INSERT_UTF8 && message_format == HILDON_IM_INSERT_UTF8_FORMAT) {
    qDebug() << "HIM: parse( HILDON_IM_INSERT_UTF8_FORMAT )";

    auto *msg = reinterpret_cast<HildonIMInsertUtf8Message *>(&event->data);
    insertUtf8(msg->msg_flag, QString::fromUtf8(msg->utf8_str));
    return true;
  } else if (msg_atom.hildon_enum == HILDON_IM_COM) {
    auto *msg = reinterpret_cast<HildonIMComMessage *>(&event->data);
    options = msg->options;

    qHimDebug() << "HIM: x11FilterEvent( _HILDON_IM_COM /" << debugNameForCommunicationId(msg->type) << ")";

    switch (msg->type) {
      // Handle Keys msgs
      case HILDON_IM_CONTEXT_HANDLE_ENTER:
        sendKey(keywidget, Qt::Key_Enter);
        return true;
      case HILDON_IM_CONTEXT_HANDLE_TAB:
        sendKey(keywidget, Qt::Key_Tab);
        return true;
      case HILDON_IM_CONTEXT_HANDLE_BACKSPACE:
        sendKey(keywidget, Qt::Key_Backspace);
        return true;
      case HILDON_IM_CONTEXT_HANDLE_SPACE:
        insertUtf8(HILDON_IM_MSG_CONTINUE, QChar(Qt::Key_Space));
        commitPreeditBuffer();
        return true;

      // Handle Clipboard msgs
      case HILDON_IM_CONTEXT_CLIPBOARD_SELECTION_QUERY:
        answerClipboardSelectionQuery(keywidget);
        return true;
      case HILDON_IM_CONTEXT_CLIPBOARD_PASTE:
        if (QClipboard *clipboard = QApplication::clipboard()) {
          QInputMethodEvent e;
          e.setCommitString(clipboard->text());
          QApplication::sendEvent(keywidget, &e);
        }
        return true;
      case HILDON_IM_CONTEXT_CLIPBOARD_COPY:
        if (QClipboard *clipboard = QApplication::clipboard())
          clipboard->setText(keywidget->inputMethodQuery(Qt::ImCurrentSelection).toString());
        return true;
      case HILDON_IM_CONTEXT_CLIPBOARD_CUT:
        if (QClipboard *clipboard = QApplication::clipboard()) {
          clipboard->setText(keywidget->inputMethodQuery(Qt::ImCurrentSelection).toString());
          QInputMethodEvent ev;
          QApplication::sendEvent(keywidget, &ev);
        }
        return true;

      // Handle commit mode msgs
      case HILDON_IM_CONTEXT_DIRECT_MODE:
        setCommitMode(HILDON_IM_COMMIT_DIRECT);
        return true;
      case HILDON_IM_CONTEXT_BUFFERED_MODE:
        setCommitMode(HILDON_IM_COMMIT_BUFFERED);
        return true;
      case HILDON_IM_CONTEXT_REDIRECT_MODE:
        setCommitMode(HILDON_IM_COMMIT_REDIRECT);
        clearSelection();
        return true;
      case HILDON_IM_CONTEXT_SURROUNDING_MODE:
        setCommitMode(HILDON_IM_COMMIT_SURROUNDING);
        return true;
      case HILDON_IM_CONTEXT_PREEDIT_MODE:
        setCommitMode(HILDON_IM_COMMIT_PREEDIT);
        return true;

      // Handle context
      case HILDON_IM_CONTEXT_CONFIRM_SENTENCE_START:
        checkSentenceStart();
        return true;
      case HILDON_IM_CONTEXT_FLUSH_PREEDIT:
        commitPreeditBuffer();
        return true;
      case HILDON_IM_CONTEXT_REQUEST_SURROUNDING:
        sendSurrounding(false);
        return true;
      case HILDON_IM_CONTEXT_LEVEL_UNSTICKY:
        mask &= ~(HILDON_IM_LEVEL_STICKY_MASK | HILDON_IM_LEVEL_LOCK_MASK);
        return true;
      case HILDON_IM_CONTEXT_SHIFT_UNSTICKY:
        mask &= ~(HILDON_IM_SHIFT_STICKY_MASK | HILDON_IM_SHIFT_LOCK_MASK);
        return true;
      case HILDON_IM_CONTEXT_CANCEL_PREEDIT:
        cancelPreedit();
        return true;
      case HILDON_IM_CONTEXT_REQUEST_SURROUNDING_FULL:
        sendSurrounding(true);
        return true;
      case HILDON_IM_CONTEXT_SPACE_AFTER_COMMIT:
      case HILDON_IM_CONTEXT_NO_SPACE_AFTER_COMMIT:
        spaceAfterCommit = (msg->type == HILDON_IM_CONTEXT_SPACE_AFTER_COMMIT);
        return true;

      case HILDON_IM_CONTEXT_WIDGET_CHANGED:
      case HILDON_IM_CONTEXT_ENTER_ON_FOCUS:
      case HILDON_IM_CONTEXT_SHIFT_LOCKED:
      case HILDON_IM_CONTEXT_SHIFT_UNLOCKED:
      case HILDON_IM_CONTEXT_LEVEL_LOCKED:
      case HILDON_IM_CONTEXT_LEVEL_UNLOCKED:
        // ignore
        return true;

      default:
        qWarning() << "HIM: x11FilterEvent( _HILDON_IM_COM /" << debugNameForCommunicationId(msg->type)
                   << ") was not handled.";
        break;
    }
  }

  return false;
}

void QHildonInputContext::sendHildonCommand(const HildonIMCommand cmd, const QWidget *widget) const {
  qDebug() << "sendHildonCommand" << cmd;
  xcb_window_t hildon_im_window = QXcb::findHildonIm();
  if (!hildon_im_window) {
    qWarning() << "sendHildonCommand: could not find hildon_im_window";
    return;
  }

  // std::cerr << "sendHildonCommand window:" << hildon_im_window << std::endl;

  auto *event = static_cast<xcb_client_message_event_t *>(calloc(32, 1));
  if (!event) {
    qWarning() << "sendHildonCommand: could not allocate memory to send event";
    free(event);
    return;
  }

  event->response_type = XCB_CLIENT_MESSAGE;
  event->format = HILDON_IM_ACTIVATE_FORMAT;
  event->sequence = 0; // XXX: required? we already do calloc
  event->window = hildon_im_window;
  event->type = QXcb::atom(HILDON_IM_ACTIVATE).xcb_atom;
  auto *msg = reinterpret_cast<HildonIMActivateMessage *>(&event->data.data8[0]);

  if (widget) {
    const int winId = static_cast<int>(widget->window()->winId());
    msg->input_window = winId;
    msg->app_window = winId;
  } else if (cmd != HILDON_IM_HIDE) {
    qWarning() << "Invalid Hildon Command:" << cmd;
    free(event);
    return;
  }

  if (cmd == HILDON_IM_SETCLIENT || cmd == HILDON_IM_SETNSHOW)
    sendInputMode();

  msg->cmd = cmd;
  msg->trigger = m_triggerMode;

  xcb_send_event(QXcb::CONNECTION, 0, hildon_im_window, 0, reinterpret_cast<const char *>(event));
  xcb_flush(QXcb::CONNECTION); // @TODO: needed?
  free(event);
}

void QHildonInputContext::sendInputMode() const {
  xcb_window_t hildon_im_window = QXcb::findHildonIm();

  auto *event = static_cast<xcb_client_message_event_t *>(calloc(32, 1));
  if (event == nullptr) {
    qWarning("sendInputMode: could not allocate memory to send event");
    return;
  }

  event->response_type = XCB_CLIENT_MESSAGE;
  event->format = HILDON_IM_INPUT_MODE_FORMAT;
  event->type = QXcb::atom(HILDON_IM_INPUT_MODE).xcb_atom;
  event->window = hildon_im_window;

  auto *msg = reinterpret_cast<HildonIMInputModeMessage *>(&event->data.data8[0]);

  msg->input_mode = static_cast<HildonGtkInputMode>(m_inputMode);
  msg->default_input_mode = HILDON_GTK_INPUT_MODE_FULL;

  qDebug() << "sendInputMode()";
  xcb_send_event(QXcb::CONNECTION, 0, hildon_im_window, 0, reinterpret_cast<const char *>(event));
  xcb_flush(QXcb::CONNECTION); // @TODO: flush needed?
  free(event);
}

void QHildonInputContext::updateInputMethodHints() {
  if (!m_currentFocus) {
    m_inputMode = 0;
    return;
  }

  const Qt::InputMethodHints hints = m_currentFocus->inputMethodHints();

  // restrictions
  if ((hints & Qt::ImhExclusiveInputMask) == Qt::ImhDialableCharactersOnly) {
    m_inputMode = HILDON_GTK_INPUT_MODE_TELE;
  } else if (((hints & Qt::ImhExclusiveInputMask) == (Qt::ImhDigitsOnly | Qt::ImhUppercaseOnly)) ||
             ((hints & Qt::ImhExclusiveInputMask) == (Qt::ImhDigitsOnly | Qt::ImhLowercaseOnly))) {
    m_inputMode = HILDON_GTK_INPUT_MODE_ALPHA;
  } else if ((hints & Qt::ImhExclusiveInputMask) == Qt::ImhDigitsOnly) {
    m_inputMode = HILDON_GTK_INPUT_MODE_NUMERIC;
  } else if (((hints & Qt::ImhExclusiveInputMask) == Qt::ImhFormattedNumbersOnly) ||
             ((hints & Qt::ImhExclusiveInputMask) == (Qt::ImhFormattedNumbersOnly | Qt::ImhDigitsOnly))) {
    m_inputMode = HILDON_GTK_INPUT_MODE_NUMERIC | HILDON_GTK_INPUT_MODE_SPECIAL;
  } else {
    m_inputMode = HILDON_GTK_INPUT_MODE_FULL;
  }

  bool isAutoCapable = (hints & (Qt::ImhExclusiveInputMask | Qt::ImhNoAutoUppercase)) == 0;
  bool isPredictive = (hints & (Qt::ImhDigitsOnly | Qt::ImhFormattedNumbersOnly | Qt::ImhUppercaseOnly |
                                Qt::ImhLowercaseOnly | Qt::ImhDialableCharactersOnly | Qt::ImhNoPredictiveText)) == 0;

  // behavior flags
  if (hints & Qt::ImhHiddenText) {
    m_inputMode |= HILDON_GTK_INPUT_MODE_INVISIBLE;
  } else {
    // no auto upper case or predictive text for passwords
    if (isAutoCapable)
      m_inputMode |= HILDON_GTK_INPUT_MODE_AUTOCAP;
    if (isPredictive)
      m_inputMode |= HILDON_GTK_INPUT_MODE_DICTIONARY;
  }

  // @TODO: multi-line support
  // QWidget *multi = realFocus ? realFocus.data() : QInputContext::focusWidget();
  // if (multi)
  //     m_inputMode |= HILDON_GTK_INPUT_MODE_MULTILINE;
  // }

#ifdef DEBUG
  printf("updateInputMethodHints: mapped hint: 0x%x to mode: 0x%x\n", static_cast<int>(hints), m_inputMode);
#endif
}

void QHildonInputContext::setFocusObject(QObject *object) {
  if (!show_again)
    return;

  const auto w = qobject_cast<QWidget *>(object);
  if (!w) {
    qDebug() << "Focus cleared.";
    return;
  }

  const bool is_input = w->inherits("QLineEdit") || w->inherits("QTextEdit") || w->inherits("QPlainTextEdit");

  if (!is_input) {
    qDebug() << "skipping widget; not input";
    return;
  }

  // As soon as the virtual keyboard is mapped by the X11 server,
  // it is also activated, which essentially steals our focus.
  // The same happens for the symbol picker.
  // This is a bug in the HIM, that we try to work around here.
  qDebug() << "Focus set on object:" << object->objectName();
  m_currentFocus = w;

  // Workaround for the GraphicsView.
  // In case of a Widget inside a GraphicsViewProxyWidget we need to remember
  // that it had the focus
  // @TODO
  // m_realFocus = resolveFocusWidget(w);

  QPlatformInputContext::setFocusObject(object);
  updateInputMethodHints();
  sendHildonCommand(HILDON_IM_SETCLIENT, w);
  show_again = false;
}

void QHildonInputContext::showSoftKeyboard() {
  // Hacky fix for the misbehaving QGraphicsProxyWidget:
  // we do not get any notification if the input focus changes to another
  // widget inside a single QGraphicsProxyWidget
  // @TODO: actually fix
  // QWidget *newFocus = resolveFocusWidget(QInputContext::focusWidget());
  // if (realFocus && (newFocus != realFocus)) {
  //   cancelPreedit();
  //   longPressKeyEvent.reset(0);
  //   longPressTimer->stop();
  //   realFocus = newFocus;
  //   updateInputMethodHints();
  // }
  // sendHildonCommand(HILDON_IM_SETNSHOW, QInputContext::focusWidget());
}
void QHildonInputContext::showInputPanel() {
  if (!m_currentFocus)
    return;

  m_triggerMode = HILDON_IM_TRIGGER_FINGER;
  sendHildonCommand(HILDON_IM_SETNSHOW, m_currentFocus);
}

void QHildonInputContext::hideInputPanel() {
  // unfortunately we cannot use this because as
  // soon as the virtual keyboard is mapped by the X11 server,
  // it is also activated, which steals our focus and that
  // causes this function to immediately be called
  // sendHildonCommand(HILDON_IM_HIDE)
}

QHildonInputContext::~QHildonInputContext() { sendHildonCommand(HILDON_IM_HIDE); }
