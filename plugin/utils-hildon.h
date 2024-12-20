#pragma once
#include <QStringList>
#include <QMap>

#include "hildon-im-protocol.h"
#include "xcb/xcb.h"

// undef some macros/symbols from X11, they conflict with Qt
#ifdef KeyPress
#undef KeyPress
#endif
#ifdef KeyRelease
#undef KeyRelease
#endif

extern QStringList hildonAtomNames;
extern QStringList hildonComNames;

struct QHildonIMAtom {
  QString name;
  HildonIMAtom hildon_enum = static_cast<HildonIMAtom>(0);
  xcb_atom_t xcb_atom = 0;
};
extern QMap<xcb_atom_t, QHildonIMAtom> HILDON_ATOM_MAP;

struct QHildonIMCom {
  QString name;
  HildonIMCommunication value;
};

extern QMap<unsigned int, QHildonIMCom> HILDON_COM_MAP;