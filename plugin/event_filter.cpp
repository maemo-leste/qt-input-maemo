#include <QDataStream>

#include "event_filter.h"
#include "input_context.h"

// https://akademy2008.kde.org/conference/slides/maemo_qt.pdf

QHildonEventFilter::QHildonEventFilter(QHildonInputContext* ctx) {
    this->ctx = ctx;
}

// Return false to continue normal event processing
bool QHildonEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result) {
	bool ours = false;
  bool filter = false;
  // HILDON_IM_SURROUNDING_CONTENT
  if (eventType == "xcb_generic_event_t") {
    auto* ev = static_cast<xcb_generic_event_t *>(message);
    if (ev->response_type == 33) {
      int wegewg = 1;
    }
  } else {
    throw "wat";
  }

  // if (eventType == "xcb_generic_event_t") {
  //   auto* ev = static_cast<xcb_generic_event_t *>(message);
  //   if ((ev->response_type & ~0x80) == XCB_CLIENT_MESSAGE) {
  //     auto* ev_msg = reinterpret_cast<xcb_client_message_event_t *>(ev);
  //
  //     for (int i = 0; i < QXcbHIMAtom::NAtoms; i++) {
  //       if (ev_msg->type == QXcb::ATOMS[i]) {
  //         ours = true;
  //       }
  //     }
  //   }
  //
  //   // qDebug() << "xcb_generic_event_t";
  //   // auto* ev = static_cast<xcb_generic_event_t *>(message);
  //   // int egeg = 1;
  //
  // } else {
  //   qDebug() << eventType;
  // }

  return false;
}
