#pragma once
#include <QObject>
#include <QDebug>
#include <QX11Info>
#include <iostream>
#include <xcb/xcb.h>

namespace QXcbHIMAtom {
  enum Atom {
    _HILDON_IM_WINDOW,
    _HILDON_IM_ACTIVATE,
    _HILDON_IM_SURROUNDING,
    _HILDON_IM_SURROUNDING_CONTENT,
    _HILDON_IM_KEY_EVENT,
    _HILDON_IM_INSERT_UTF8,
    _HILDON_IM_COM,
    _HILDON_IM_CLIPBOARD_COPIED,
    _HILDON_IM_CLIPBOARD_SELECTION_QUERY,
    _HILDON_IM_CLIPBOARD_SELECTION_REPLY,
    _HILDON_IM_INPUT_MODE,
    _HILDON_IM_PREEDIT_COMMITTED,
    _HILDON_IM_PREEDIT_COMMITTED_CONTENT,
    _HILDON_IM_LONG_PRESS_SETTINGS,
    NAtoms,
  };
}


namespace QXcb {
  extern xcb_connection_t* CONNECTION;
  extern xcb_atom_t ATOMS[];
  inline xcb_atom_t atom(const QXcbHIMAtom::Atom atom) { return ATOMS[atom]; }
  void initialiseAtoms();

  xcb_window_t findHildonIm();

}