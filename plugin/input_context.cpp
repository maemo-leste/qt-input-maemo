#include <QGuiApplication>
#include <QObject>
#include <QWidget>

#include "input_context.h"

QHildonInputContext::QHildonInputContext() :
      m_inputMode(HILDON_GTK_INPUT_MODE_FULL),
      m_lastKeyWidget(nullptr),
      m_lastInternalChange(false),
      m_mask(0),
      m_options(0),
      m_textCursorPosOnPress(0),
      m_autoUpper(false),
      lastInternalChange(false),
      m_spaceAfterCommit(false),
      m_lastQtKeyCode(Qt::Key_unknown),
      m_triggerMode(HILDON_IM_TRIGGER_NONE),
      m_commitMode(HILDON_IM_COMMIT_REDIRECT),
      m_lastCommitMode(HILDON_IM_COMMIT_REDIRECT) {
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
    qWarning() << "No application!";
  }

  // generate lookups
  QXcb::initialiseHildonAtoms();
  QXcb::initialiseHildonComEnums();

  // sendKey(new QWidget(), Qt::Key_Enter);
  // setCommitMode(HILDON_IM_COMMIT_DIRECT, false);
}

/*! \internal
In redirect mode we use a proxy widget (fullscreen vkb). When the cursor position
 *  changes there, the HIM update the cursor position in the client (Qt application)
 */
void QHildonInputContext::setClientCursorLocation(const bool offsetIsRelative, int cursorOffset) {
  qDebug() << "HIM: setClientCursorLocation(" << offsetIsRelative << ", " << cursorOffset<< ")";

  QWidget *w = focusWidget();
  if (!w)
    return;

  if (offsetIsRelative)
    cursorOffset += w->inputMethodQuery(Qt::ImCursorPosition).toInt();

  QList<QInputMethodEvent::Attribute> attributes;
  attributes << QInputMethodEvent::Attribute(QInputMethodEvent::Selection, cursorOffset, 0, QVariant());

  QInputMethodEvent e(QString(), attributes);
  QApplication::sendEvent(w, &e);
}

void QHildonInputContext::commitPreEditBuffer() {
  qDebug() << "HIM: commitPreeditBuffer()";

  QWidget *w = focusWidget();
  if (!w)
    return;

  QInputMethodEvent e;
  if (m_spaceAfterCommit)
    e.setCommitString(m_preEditBuffer + QLatin1Char(' '));
  else
    e.setCommitString(m_preEditBuffer);

  QApplication::sendEvent(w, &e);
  m_preEditBuffer.clear();
}

bool QHildonInputContext::parseHildonCommand(xcb_client_message_event_t *event) {
  const QHildonIMAtom msg_atom = HILDON_ATOM_MAP[event->type];
  const auto message_format = event->format;

  qDebug() << "HIM: parse" << HILDON_ATOM_MAP[event->type].name;

  if (msg_atom.hildon_enum == HILDON_IM_INSERT_UTF8 && message_format == HILDON_IM_INSERT_UTF8_FORMAT) {
    qDebug() << "HIM: parse( HILDON_IM_INSERT_UTF8_FORMAT )";

    const auto *msg = reinterpret_cast<HildonIMInsertUtf8Message *>(&event->data);
    insertUtf8(msg->msg_flag, QString::fromUtf8(msg->utf8_str));
    return true;
  } else if (msg_atom.hildon_enum == HILDON_IM_COM) {
    auto *msg = reinterpret_cast<HildonIMComMessage *>(&event->data);
    if (!HILDON_COM_MAP.contains(msg->type)) {
      qWarning() << QString("HIM: parse( _HILDON_IM_COM / unknown type %1, skipping)").arg(msg->type);
      return false;
    }

    auto com = HILDON_COM_MAP[msg->type];

    // eww
    m_options = msg->options;

    qDebug() << "HIM: parse( _HILDON_IM_COM /" << com.name << ")";

    switch (msg->type) {
      // Handle Keys msgs
      case HILDON_IM_CONTEXT_HANDLE_ENTER:
        sendKey(m_currentFocus, Qt::Key_Enter);
        return true;
      case HILDON_IM_CONTEXT_HANDLE_TAB:
        sendKey(m_currentFocus, Qt::Key_Tab);
        return true;
      case HILDON_IM_CONTEXT_HANDLE_BACKSPACE:
        sendKey(m_currentFocus, Qt::Key_Backspace);
        return true;
      case HILDON_IM_CONTEXT_HANDLE_SPACE:
        insertUtf8(HILDON_IM_MSG_CONTINUE, QChar(Qt::Key_Space));
        commitPreEditBuffer();
        return true;

      // Handle Clipboard msgs
      case HILDON_IM_CONTEXT_CLIPBOARD_SELECTION_QUERY:
        answerClipboardSelectionQuery(m_currentFocus);
        return true;
      case HILDON_IM_CONTEXT_CLIPBOARD_PASTE:
        if (const QClipboard *clipboard = QApplication::clipboard()) {
          QInputMethodEvent e;
          e.setCommitString(clipboard->text());
          QApplication::sendEvent(m_currentFocus, &e);
        }
        return true;
      case HILDON_IM_CONTEXT_CLIPBOARD_COPY:
        if (QClipboard *clipboard = QApplication::clipboard())
          clipboard->setText(m_currentFocus->inputMethodQuery(Qt::ImCurrentSelection).toString());
        return true;
      case HILDON_IM_CONTEXT_CLIPBOARD_CUT:
        if (QClipboard *clipboard = QApplication::clipboard()) {
          clipboard->setText(m_currentFocus->inputMethodQuery(Qt::ImCurrentSelection).toString());
          QInputMethodEvent ev;
          QApplication::sendEvent(m_currentFocus, &ev);
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
        commitPreEditBuffer();
        return true;
      case HILDON_IM_CONTEXT_REQUEST_SURROUNDING:
        sendSurrounding(false);
        return true;
      case HILDON_IM_CONTEXT_LEVEL_UNSTICKY:
        m_mask &= ~(HILDON_IM_LEVEL_STICKY_MASK | HILDON_IM_LEVEL_LOCK_MASK);
        return true;
      case HILDON_IM_CONTEXT_SHIFT_UNSTICKY:
        m_mask &= ~(HILDON_IM_SHIFT_STICKY_MASK | HILDON_IM_SHIFT_LOCK_MASK);
        return true;
      case HILDON_IM_CONTEXT_CANCEL_PREEDIT:
        cancelPreedit();
        return true;
      case HILDON_IM_CONTEXT_REQUEST_SURROUNDING_FULL:
        sendSurrounding(true);
        return true;
      case HILDON_IM_CONTEXT_SPACE_AFTER_COMMIT:
      case HILDON_IM_CONTEXT_NO_SPACE_AFTER_COMMIT:
        m_spaceAfterCommit = (msg->type == HILDON_IM_CONTEXT_SPACE_AFTER_COMMIT);
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
        break;
    }
  } else if (msg_atom.hildon_enum == HILDON_IM_SURROUNDING_CONTENT) {
    auto* msg = reinterpret_cast<HildonIMSurroundingContentMessage *>(&event->data);
    qDebug() << "received:" << msg->surrounding;
  }

  return false;
}

void QHildonInputContext::clearSelection() const {
  qDebug() << "HIM: clearSelection()";

  QWidget *w = focusWidget();
  if (!w)
    return;

  int textCursorPos = w->inputMethodQuery(Qt::ImCursorPosition).toInt();
  QString selection = w->inputMethodQuery(Qt::ImCurrentSelection).toString();

  if (selection.isEmpty())
    return;

  //Remove the selection
  QInputMethodEvent e;
  e.setCommitString(selection);
  QApplication::sendEvent(w, &e);

  //Move the cursor backward if the text has been selected from right to left
  if (textCursorPos < m_textCursorPosOnPress){
    QInputMethodEvent e;
    e.setCommitString(QString(), -selection.length(),0);
    QApplication::sendEvent(w, &e);
  }
}

void QHildonInputContext::cancelPreedit() {
  qDebug() << "HIM: cancelPreedit()";

  QWidget *w = focusWidget();
  if (!w)
    return;

  if (m_preEditBuffer.isEmpty())
    return;
  m_preEditBuffer.clear();

  QInputMethodEvent e;
  QApplication::sendEvent(w, &e);
}

void QHildonInputContext::sendHildonCommand(const HildonIMCommand cmd, const QWidget *widget) const {
  qDebug() << "sendHildonCommand" << cmd;
  xcb_window_t hildon_im_window = QXcb::findHildonIm();
  if (!hildon_im_window) {
    qWarning() << "sendHildonCommand: could not find hildon_im_window";
    return;
  }

  // std::cerr << "sendHildonCommand window:" << hildon_im_window << std::endl;

  auto *event = QXcb::createMessageEvent();
  if (!event) {
    free(event);
    return;
  }

  event->response_type = XCB_CLIENT_MESSAGE;
  event->format = HILDON_IM_ACTIVATE_FORMAT;
  event->sequence = 0; // XXX: required? we already do calloc
  event->window = hildon_im_window;
  event->type = QXcb::hildon_atom(HILDON_IM_ACTIVATE).xcb_atom;
  auto *msg = reinterpret_cast<HildonIMActivateMessage *>(&event->data.data8[0]);

  if (widget) {
    // @TODO: uhmmm
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

/*! \internal
Ask the client widget to insert the specified text at the cursor
 *  position, by triggering the commit signal on the context
 */
void QHildonInputContext::insertUtf8(int flag, const QString &text) {
  qDebug() << "HIM: insertUtf8(" << flag << ", " << text << ")";

  QWidget *w = focusWidget();
  if (!w)
    return;

  QString cleanText = text;
  if (m_mask & HILDON_IM_SHIFT_LOCK_MASK)
    cleanText = cleanText.toUpper();

  m_lastInternalChange = true;

  // TODO HILDON_IM_AUTOCORRECT is used by the handwriting plugin
  // Writing CiAo in the plugin add Ciao in the widget.
  if (m_options & HILDON_IM_AUTOCORRECT) {
    qWarning() << "HILDON_IM_AUTOCORRECT Not Implemented Yet";
  }

  // Delete suroundings when we are using the m_preEditBuffer.
  //  Eg: For the HandWriting plugin
  if (!m_preEditBuffer.isNull()) {
    // Updates m_preEditBuffer
    if (flag != HILDON_IM_MSG_START) {
      m_preEditBuffer.append(cleanText);
      cleanText = m_preEditBuffer;
    }
  }

  if (m_commitMode == HILDON_IM_COMMIT_PREEDIT) {
    if (m_preEditBuffer.isNull())
      m_preEditBuffer = cleanText;

    // Creating attribute list
    QList<QInputMethodEvent::Attribute> attributes;
    QTextCharFormat textCharFormat;
    textCharFormat.setFontUnderline(true);
    textCharFormat.setBackground(w->palette().highlight());
    textCharFormat.setForeground(w->palette().base());
    attributes << QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat, 0, cleanText.length(), textCharFormat);
    attributes << QInputMethodEvent::Attribute(QInputMethodEvent::Cursor, 0, 1, QVariant());

    QInputMethodEvent e(cleanText, attributes);
    QApplication::sendEvent(w, &e);

    // Reset commit mode
    if (flag == HILDON_IM_MSG_END)
      setCommitMode(m_lastCommitMode, false);
  } else { // commitMode != HILDON_IM_COMMIT_PREEDIT
    QInputMethodEvent e;
    e.setCommitString(cleanText);
    QApplication::sendEvent(w, &e);
  }
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
  event->type = QXcb::hildon_atom(HILDON_IM_INPUT_MODE).xcb_atom;
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

  // @TODO: pretty print inputMode

#ifdef DEBUG
  printf("updateInputMethodHints: mapped hint: 0x%x to mode: 0x%x\n", static_cast<int>(hints), m_inputMode);
#endif
}

void QHildonInputContext::setFocusObject(QObject *object) {
  // if (!show_again)
  //   return;

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
  // show_again = false;
}

void QHildonInputContext::sendSurrounding(bool sendAllContents) {
  QWidget *w = focusWidget();
  if (!w) {
    qWarning() << "sendSurrounding: bad widget";
    return;
  }

  xcb_window_t hildon_im_window = QXcb::findHildonIm();
  if (!hildon_im_window) {
    qWarning() << "sendSurrounding: could not find hildon_im_window";
    return;
  }

  QString surrounding;
  int cpos;
  if (sendAllContents) {
    // Qt::ImSurrounding only returns the current block

    if (const auto te = qobject_cast<QTextEdit *>(w)) {
      surrounding = te->toPlainText();
      cpos = te->textCursor().position();
    } else if (const auto *pte = qobject_cast<QPlainTextEdit *>(w)) {
      surrounding = pte->toPlainText();
      cpos = pte->textCursor().position();
    } else if (const QGraphicsObject *declarativeTextEdit = qDeclarativeTextEdit_cast(w)) {
      surrounding = declarativeTextEdit->property("text").toString();
      cpos = declarativeTextEdit->property("cursorPosition").toInt();
    } else {
      surrounding = w->inputMethodQuery(Qt::ImSurroundingText).toString();
      cpos = w->inputMethodQuery(Qt::ImCursorPosition).toInt();
    }
  } else {
    surrounding = w->inputMethodQuery(Qt::ImSurroundingText).toString();
    cpos = w->inputMethodQuery(Qt::ImCursorPosition).toInt();
  }

  if (surrounding.isEmpty())
    cpos = 0;

  // Split surrounding context into parts that are small enough to send in a X11 message
  const QByteArray ba = surrounding.toUtf8();
  bool firstPart = true;
  int offset = 0;

  while (firstPart || (offset < ba.size())) {
    auto *event = QXcb::createMessageEvent();
    if (!event) {
      return;
    }

    event->response_type = XCB_CLIENT_MESSAGE;
    event->sequence = 0;

    auto *msg = reinterpret_cast<HildonIMSurroundingContentMessage *>(&event->data.data8[0]);
    // this call will take care of adding the trailing '\0' for surrounding string
    event->type = QXcb::hildon_atom(HILDON_IM_SURROUNDING_CONTENT).xcb_atom;
    event->format = HILDON_IM_SURROUNDING_CONTENT_FORMAT;

    const int len = qMin(ba.size() - offset, static_cast<int>(HILDON_IM_CLIENT_MESSAGE_BUFFER_SIZE) - 1);
    memcpy(msg->surrounding, ba.constData() + offset, len);
    offset += len;

    if (firstPart)
      msg->msg_flag = HILDON_IM_MSG_START;
    else if (offset == ba.size())
      msg->msg_flag = HILDON_IM_MSG_END;
    else
      msg->msg_flag = HILDON_IM_MSG_CONTINUE;

    event->window = hildon_im_window;
    xcb_send_event(QXcb::CONNECTION, 0, hildon_im_window, 0, reinterpret_cast<const char *>(event));
    xcb_flush(QXcb::CONNECTION);
    free(event);
    firstPart = false;
  }

  auto *event = QXcb::createMessageEvent();
  if (!event) {
    free(event);
    return;
  }

  event->response_type = XCB_CLIENT_MESSAGE;
  event->sequence = 0;
  event->window = hildon_im_window;

  // Send the cursor offset in the surrounding
  event->type = QXcb::hildon_atom(HILDON_IM_SURROUNDING).xcb_atom;
  event->format = HILDON_IM_SURROUNDING_FORMAT;

  auto* msg_cursor = reinterpret_cast<HildonIMSurroundingMessage *>(&event->data.data8[0]);
  msg_cursor->commit_mode = m_commitMode;
  msg_cursor->cursor_offset = cpos;

  xcb_send_event(QXcb::CONNECTION, 0, hildon_im_window, 0, reinterpret_cast<const char *>(event));
  xcb_flush(QXcb::CONNECTION);
  free(event);
}

// void QHildonInputContext::showSoftKeyboard() {
//   // Hacky fix for the misbehaving QGraphicsProxyWidget:
//   // we do not get any notification if the input focus changes to another
//   // widget inside a single QGraphicsProxyWidget
//   // @TODO: actually fix
//   // QWidget *newFocus = resolveFocusWidget(QInputContext::focusWidget());
//   // if (realFocus && (newFocus != realFocus)) {
//   //   cancelPreedit();
//   //   longPressKeyEvent.reset(0);
//   //   longPressTimer->stop();
//   //   realFocus = newFocus;
//   //   updateInputMethodHints();
//   // }
//   // sendHildonCommand(HILDON_IM_SETNSHOW, QInputContext::focusWidget());
// }

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

void QHildonInputContext::setCommitMode(HildonIMCommitMode mode, bool clearPreEdit) {
  if (m_commitMode != mode) {
    if (clearPreEdit)
      m_preEditBuffer.clear();
    m_lastCommitMode = m_commitMode;
  }

  m_commitMode = mode;
}

// CONTEXT
/*! \internal
Updates the IM with the autocap state at the active cursor position
 */
void QHildonInputContext::checkSentenceStart() {
  qDebug() << "HIM: checkSentenceStart()";

  if (!(m_options & HILDON_IM_AUTOCASE))
    return;

  QWidget *w = focusWidget();
  if (!w)
    return;

  if ((m_inputMode & (HILDON_GTK_INPUT_MODE_ALPHA | HILDON_GTK_INPUT_MODE_AUTOCAP)) !=
      (HILDON_GTK_INPUT_MODE_ALPHA | HILDON_GTK_INPUT_MODE_AUTOCAP)) {
    // If autocap is off, but the mode contains alpha, send autocap message.
    // The important part is that when entering a numerical entry the autocap
    // is not defined, and the plugin sets the mode appropriate for the language */
    if (m_inputMode & HILDON_GTK_INPUT_MODE_ALPHA) {
      m_autoUpper = false;
      sendHildonCommand(HILDON_IM_SHIFT_UNSTICKY, w);
    }
    return;
  }

  if (m_inputMode & HILDON_GTK_INPUT_MODE_INVISIBLE) {
    // no autocap for passwords
    m_autoUpper = false;
    sendHildonCommand(HILDON_IM_SHIFT_UNSTICKY, w);
  }

  int cpos = w->inputMethodQuery(Qt::ImCursorPosition).toInt();
  QString analyze;

  // Improve performance: only analyze 10 chars before the cursor
  if (cpos) {
    constexpr int analyzeCount = 10;
    analyze = w->inputMethodQuery(Qt::ImSurroundingText)
                  .toString()
                  .mid(qMax(cpos - analyzeCount, 0), qMin(cpos, analyzeCount));
  }

  int spaces = 0;

  while (spaces < analyze.length()) {
    if (analyze.at(analyze.length() - spaces - 1).isSpace())
      spaces++;
    else
      break;
  }

  // not very nice, but QTextBoundaryFinder doesn't really work here
  static const QString punctuation = QLatin1String(".!?\xa1\xbf"); // spanish inverted ! and ?

  if (!cpos || analyze.length() == spaces ||
      (spaces && punctuation.contains(analyze.at(analyze.length() - spaces - 1)))) {
    m_autoUpper = true;
    sendHildonCommand(HILDON_IM_SHIFT_STICKY, w);
  } else {
    m_autoUpper = false;
    sendHildonCommand(HILDON_IM_SHIFT_UNSTICKY, w);
  }
}

void QHildonInputContext::setMaskState(int *mask, HildonIMInternalModifierMask lock_mask,
                                       const HildonIMInternalModifierMask sticky_mask,
                                       const bool was_press_and_release) const {
  // LOGMESSAGE3("setMaskState", lock_mask, sticky_mask)
  // LOGMESSAGE3(" - ", "mask=", *mask)

  /* Locking Fn is disabled in TELE and NUMERIC */
  if (!(m_inputMode & HILDON_GTK_INPUT_MODE_ALPHA) && !(m_inputMode & HILDON_GTK_INPUT_MODE_HEXA) &&
      ((m_inputMode & HILDON_GTK_INPUT_MODE_TELE) || (m_inputMode & HILDON_GTK_INPUT_MODE_NUMERIC))) {
    if (*mask & lock_mask) {
      /* already locked, remove lock and set it to sticky */
      *mask &= ~(lock_mask | sticky_mask);
      *mask |= sticky_mask;
    } else if (*mask & sticky_mask) {
      /* the key is already sticky, it's fine */
    } else if (was_press_and_release) {
      /* Pressing the key for the first time stickies the key for one character,
       * but only if no characters were entered while holding the key down */
      *mask |= sticky_mask;
    }
    return;
  }

  if (*mask & lock_mask) {
    /* Pressing the key while already locked clears the state */
    if (lock_mask & HILDON_IM_SHIFT_LOCK_MASK)
      sendHildonCommand(HILDON_IM_SHIFT_UNLOCKED, m_currentFocus);
    else if (lock_mask & HILDON_IM_LEVEL_LOCK_MASK)
      sendHildonCommand(HILDON_IM_MOD_UNLOCKED, m_currentFocus);

    *mask &= ~(lock_mask | sticky_mask);
  } else if (*mask & sticky_mask) {
    /* When the key is already sticky, a second press locks the key */
    *mask |= lock_mask;

    if (lock_mask & HILDON_IM_SHIFT_LOCK_MASK)
      sendHildonCommand(HILDON_IM_SHIFT_LOCKED, m_currentFocus);
    else if (lock_mask & HILDON_IM_LEVEL_LOCK_MASK)
      sendHildonCommand(HILDON_IM_MOD_LOCKED, m_currentFocus);
  } else if (was_press_and_release) {
    /* Pressing the key for the first time stickies the key for one character,
     * but only if no characters were entered while holding the key down */
    *mask |= sticky_mask;
  }
}

#include "xcb/xcb.h"
/*! Sends the key as a spontaneous event.
 */
void QHildonInputContext::sendKey(QWidget *keywidget, int qtCode) {
  const QPointer guard = keywidget;

  xcb_keysym_t keysym = XCB_NO_SYMBOL;
  int keycode;

  switch (qtCode) {
    case Qt::Key_Enter:
      keycode = 36;
      break;
    case Qt::Key_Tab:
      keycode = 66;
      break;
    case Qt::Key_Backspace:
      keycode = 22;
      break;
    case Qt::Key_Left:
      keycode = 116;
      break;
    case Qt::Key_Right:
      keycode = 114;
      break;
    default:
      qDebug("HIM: sendKey() keycode %d not allowed", qtCode);
      return;
  }

  keysym = QXcb::XKeycodeToKeysym(keycode);

  QKeyEvent click(QEvent::KeyPress, qtCode, Qt::NoModifier, keycode, keysym, 0, QString(), false, 1);
  qt_sendSpontaneousEvent(keywidget, &click);

  // in case the widget was destroyed when the key went down
  if (guard.isNull())
    return;

  QKeyEvent release(QEvent::KeyRelease, qtCode, Qt::NoModifier, keycode, keysym, 0, QString(), false, 1);
  qt_sendSpontaneousEvent(keywidget, &click);
}

bool QHildonInputContext::qt_sendSpontaneousEvent(QObject *receiver, QEvent *event) {
  return QCoreApplication::sendEvent(receiver, event);
}

/*!
 */
void QHildonInputContext::answerClipboardSelectionQuery(const QWidget *widget) {
  bool hasSelection = !widget->inputMethodQuery(Qt::ImCurrentSelection).toString().isEmpty();

  xcb_window_t hildon_im_window = QXcb::findHildonIm();
  if (!hildon_im_window) {
    qWarning() << "sendHildonCommand: could not find hildon_im_window";
    return;
  }

  auto *event = QXcb::createMessageEvent();
  if (!event) {
    free(event);
    return;
  }

  event->response_type = XCB_CLIENT_MESSAGE;
  event->format = HILDON_IM_CLIPBOARD_SELECTION_REPLY_FORMAT;
  event->sequence = 0;
  event->window = hildon_im_window;
  event->type = QXcb::hildon_atom(HILDON_IM_CLIPBOARD_SELECTION_REPLY).xcb_atom;
  event->data.data32[0] = hasSelection;  // @TODO: test if this works

  xcb_send_event(QXcb::CONNECTION, 0, hildon_im_window, 0, reinterpret_cast<const char *>(event));
  xcb_flush(QXcb::CONNECTION);
  free(event);
}

QGraphicsObject* QHildonInputContext::qDeclarativeTextEdit_cast(QWidget *w) {
  if (const auto *view = qobject_cast<QGraphicsView *>(w)) {
    if (auto *item = qgraphicsitem_cast<QGraphicsObject *>(view->scene()->focusItem())) {
      if (item->inherits("QDeclarativeTextEdit"))
        return item;
    }
  }
  return nullptr;
}
