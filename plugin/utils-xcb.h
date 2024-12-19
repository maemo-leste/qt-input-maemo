#pragma once
#include <QDebug>
#include <QObject>
#include <QX11Info>

#include <iostream>
#include <xcb/xcb.h>

#include "utils-hildon.h"

namespace QXcb {
  extern xcb_connection_t* CONNECTION;
  inline xcb_client_message_event_t* createMessageEvent() {
    auto *event = static_cast<xcb_client_message_event_t *>(calloc(32, 1));
    if (!event) {
      qWarning() << "could not allocate memory for xcb event";
      free(event);
      return nullptr;
    }
    return event;
  }
  inline QHildonIMAtom hildon_atom(const HildonIMAtom hildon_enum) {
    for (QMap<xcb_atom_t, QHildonIMAtom>::iterator it = HILDON_ATOM_MAP.begin(); it != HILDON_ATOM_MAP.end(); ++it)
      if (it.value().hildon_enum == hildon_enum) return it.value();
    qWarning() << "Could not resolve Hildon atom: " << hildon_enum;
    return {};
  }
  void initialiseAtoms();
  xcb_window_t findHildonIm();
}