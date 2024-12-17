#include "utils-hildon.h"

// sigh
QStringList qxcbAtomNames = {
   "_HILDON_IM_WINDOW",
   "_HILDON_IM_ACTIVATE",
   "_HILDON_IM_SURROUNDING",
   "_HILDON_IM_SURROUNDING_CONTENT",
   "_HILDON_IM_KEY_EVENT",
   "_HILDON_IM_INSERT_UTF8",
   "_HILDON_IM_COM",
   "_HILDON_IM_CLIPBOARD_COPIED",
   "_HILDON_IM_CLIPBOARD_SELECTION_QUERY",
   "_HILDON_IM_CLIPBOARD_SELECTION_REPLY",
   "_HILDON_IM_INPUT_MODE",
   "_HILDON_IM_PREEDIT_COMMITTED",
   "_HILDON_IM_PREEDIT_COMMITTED_CONTENT",
   "_HILDON_IM_LONG_PRESS_SETTINGS"
 };

int getAtomID_bykey(const QString &key) {
  static QMap<QString, int> keyMap = {
    {"HILDON_IM_WINDOW", 529},
    {"HILDON_IM_ACTIVATE", 530},
    {"HILDON_IM_SURROUNDING", 533},
    {"HILDON_IM_SURROUNDING_CONTENT", 534},
    {"HILDON_IM_KEY_EVENT", 535},
    {"HILDON_IM_INSERT_UTF8", 532},
    {"HILDON_IM_COM", 536},
    {"HILDON_IM_CLIPBOARD_COPIED", 537},
    {"HILDON_IM_CLIPBOARD_SELECTION_QUERY", 538},
    {"HILDON_IM_CLIPBOARD_SELECTION_REPLY", 539},
    {"HILDON_IM_INPUT_MODE", 531},
    {"HILDON_IM_PREEDIT_COMMITTED", 540},
    {"HILDON_IM_PREEDIT_COMMITTED_CONTENT", 541},
    {"HILDON_IM_LONG_PRESS_SETTINGS", 542}
  };
  return keyMap.value(key, -1);
}

QString getAtomName(HildonIMAtom atom) {
  switch (atom) {
    case HILDON_IM_WINDOW:
      return "HILDON_IM_WINDOW";
    case HILDON_IM_ACTIVATE:
      return "HILDON_IM_ACTIVATE";
    case HILDON_IM_INPUT_MODE:
      return "HILDON_IM_INPUT_MODE";
    case HILDON_IM_INSERT_UTF8:
      return "HILDON_IM_INSERT_UTF8";
    case HILDON_IM_SURROUNDING:
      return "HILDON_IM_SURROUNDING";
    case HILDON_IM_SURROUNDING_CONTENT:
      return "HILDON_IM_SURROUNDING_CONTENT";
    case HILDON_IM_KEY_EVENT:
      return "HILDON_IM_KEY_EVENT";
    case HILDON_IM_COM:
      return "HILDON_IM_COM";
    case HILDON_IM_CLIPBOARD_COPIED:
      return "HILDON_IM_CLIPBOARD_COPIED";
    case HILDON_IM_CLIPBOARD_SELECTION_QUERY:
      return "HILDON_IM_CLIPBOARD_SELECTION_QUERY";
    case HILDON_IM_CLIPBOARD_SELECTION_REPLY:
      return "HILDON_IM_CLIPBOARD_SELECTION_REPLY";
    case HILDON_IM_PREEDIT_COMMITTED:
      return "HILDON_IM_PREEDIT_COMMITTED";
    case HILDON_IM_PREEDIT_COMMITTED_CONTENT:
      return "HILDON_IM_PREEDIT_COMMITTED_CONTENT";
    case HILDON_IM_LONG_PRESS_SETTINGS:
      return "HILDON_IM_LONG_PRESS_SETTINGS";
    default:
      return "Unknown atom";
  }
}