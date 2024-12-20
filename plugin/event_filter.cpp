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
    auto *event = static_cast<xcb_generic_event_t *>(message);
    auto bla = event->response_type & ~0x80;
    switch (bla) {
      case XCB_CLIENT_MESSAGE: {
        auto *event_message = reinterpret_cast<xcb_client_message_event_t *>(event);
        if (const xcb_atom_t atom = event_message->type; HILDON_ATOM_MAP.contains(atom)) {
          return this->ctx->parseHildonCommand(event_message);
        }

        printf("unrelated ClientMessage received, type: %u\n", event_message->type);
        fflush(stdout);
        break;
      }
      default:;
    }
  } else {
    qDebug() << "ignoring unknown event type" << eventType;
  }

  return false;
}
