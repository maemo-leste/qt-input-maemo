#include <QObject>
#include <QGuiApplication>
#include <QWidget>

#include "input_context.h"

QHildonInputContext::QHildonInputContext() {
  qDebug() << "QHildonInputContext created";

  // xcb connection for this application
  QXcb::CONNECTION = QX11Info::connection();
  if(!QXcb::CONNECTION)
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

void QHildonInputContext::show_vkb(const QWidget *w) {
  sendInputMode();
  sendHildonCommand(HILDON_IM_SHOW, w);
}

void QHildonInputContext::sendHildonCommand(const HildonIMCommand cmd, const QWidget *widget) {
  qDebug() << "sendHildonCommand" << cmd;
  xcb_window_t hildon_im_window = QXcb::findHildonIm();

  //std::cerr << "sendHildonCommand window:" << hildon_im_window << std::endl;

  auto* event = static_cast<xcb_client_message_event_t *>(calloc(32, 1));
  if (event == nullptr) {
    qWarning("sendHildonCommand: could not allocate memory to send event");
    free(event);
    return;
  }

  event->response_type = XCB_CLIENT_MESSAGE;
  event->format = HILDON_IM_ACTIVATE_FORMAT;
  event->sequence = 0; // XXX: required? we already do calloc
  event->window = hildon_im_window;
  event->type = QXcb::atom(QXcbHIMAtom::_HILDON_IM_ACTIVATE);

  auto *msg = reinterpret_cast<HildonIMActivateMessage *>(&event->data.data8[0]);

  if (widget) {
    msg->input_window = widget->window()->winId();
    //msg->input_window = QHIMProxyWidget::proxyFor(widget)->winId();
    msg->app_window = widget->window()->winId();
  } else if (cmd != HILDON_IM_HIDE) {
    qWarning() << "Invalid Hildon Command:" << cmd;
    free(event);
    return;
  }

  //// TODO
  if (cmd == HILDON_IM_SETCLIENT || cmd == HILDON_IM_SETNSHOW)
    sendInputMode();

  //msg->cmd = cmd;
  msg->cmd = cmd;
  msg->trigger = HILDON_IM_TRIGGER_KEYBOARD; // XXX
  //msg->trigger = triggerMode;

  xcb_send_event(QXcb::CONNECTION, 0, hildon_im_window, 0, reinterpret_cast<const char *>(event));
  xcb_flush(QXcb::CONNECTION); // XXX: maybe remove
  free(event);
}

void QHildonInputContext::sendInputMode() {
  // std::cerr << "sendInputMode" << std::endl;

  // qDebug() << "HIM: sendInputMode";
  xcb_window_t hildon_im_window = QXcb::findHildonIm();

  auto *event = static_cast<xcb_client_message_event_t *>(calloc(32, 1));
  if (event == nullptr) {
    qWarning("sendInputMode: could not allocate memory to send event");
    return;
  }

  event->response_type = XCB_CLIENT_MESSAGE;
  event->format = HILDON_IM_INPUT_MODE_FORMAT;
  event->type = QXcb::atom(QXcbHIMAtom::_HILDON_IM_INPUT_MODE);
  event->window = hildon_im_window;

  auto *msg = reinterpret_cast<HildonIMInputModeMessage *>(&event->data.data8[0]);

  msg->input_mode = static_cast<HildonGtkInputMode>(inputMode);
  msg->default_input_mode = HILDON_GTK_INPUT_MODE_FULL;

  std::cerr << "sendInputMode()" << std::endl;
  xcb_send_event(QXcb::CONNECTION, 0, hildon_im_window, 0, reinterpret_cast<const char *>(event));
  xcb_flush(QXcb::CONNECTION); // XXX: maybe remove

  free(event);
}

QHildonInputContext::~QHildonInputContext() {
  sendHildonCommand(HILDON_IM_HIDE);
}

void QHildonInputContext::setFocusObject(QObject *object) {
  if (object) {
    qDebug() << "Focus set on object:" << object->objectName();
    if(object->objectName() == "line" && show_again) {
      show_vkb(dynamic_cast<QWidget *>(object));
      show_again = false;
    }
  } else {
    qDebug() << "Focus cleared.";
  }

  QPlatformInputContext::setFocusObject(object);
}

