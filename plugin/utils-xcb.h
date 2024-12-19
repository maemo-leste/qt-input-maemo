#pragma once
#include <QDebug>
#include <QObject>
#include <QX11Info>

#include <iostream>
#include <algorithm>
#include <xcb/xcb.h>

#include "utils-hildon.h"

namespace QXcb {
  extern xcb_atom_t ATOMS[];

  extern xcb_connection_t* CONNECTION;
  inline QHildonIMAtom atom(const HildonIMAtom hildon_enum) {
    for (QMap<xcb_atom_t, QHildonIMAtom>::iterator it = HILDON_ATOM_MAP.begin(); it != HILDON_ATOM_MAP.end(); ++it)
      if (it.value().hildon_enum == hildon_enum) return it.value();
    qWarning() << "Could not resolve Hildon atom: " << hildon_enum;
    return {};
  }
  void initialiseAtoms();
  xcb_window_t findHildonIm();
}