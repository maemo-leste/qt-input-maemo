#include <QDataStream>

#include <xcb/xinput.h>

#include "event_filter.h"
#include "input_context.h"

// https://akademy2008.kde.org/conference/slides/maemo_qt.pdf

QHildonEventFilter::QHildonEventFilter(QHildonInputContext* ctx) {
    this->ctx = ctx;
}

// Return false to continue normal event processing
bool QHildonEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result) {
  if (eventType == "xcb_generic_event_t") {
    switch (auto *event = static_cast<xcb_generic_event_t *>(message); event->response_type & ~0x80) {
      case XCB_CLIENT_MESSAGE: {
        auto *event_message = reinterpret_cast<xcb_client_message_event_t *>(event);
        if (const xcb_atom_t atom = event_message->type; HILDON_ATOM_MAP.contains(atom)) {
          return this->ctx->parseHildonCommand(event_message);
        }
        break;
      }
      default:;
    }
  } else {
    qDebug() << "ignoring unknown event type" << eventType;
  }

  return false;
}
