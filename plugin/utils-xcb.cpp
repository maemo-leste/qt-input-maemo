#include "utils-xcb.h"
#include "utils-hildon.h"
#include "xcbkeys.h"

namespace QXcb {
  // extern
  xcb_connection_t* CONNECTION = nullptr;

  xcb_window_t findHildonIm() {
    xcb_window_t result = 0;
    const auto root_window = static_cast<xcb_window_t>(QX11Info::appRootWindow(-1));

    // still have app window?
    if (root_window == 0)
      return result;

    // still have a valid XCB connection?
    if (CONNECTION == nullptr || xcb_connection_has_error(CONNECTION)) {
      qWarning("Failed to connect to X server");
      return result;
    }

    const xcb_get_property_cookie_t prop_cookie = xcb_get_property(
        CONNECTION, 0, root_window, hildon_atom(HILDON_IM_WINDOW).xcb_atom, XCB_GET_PROPERTY_TYPE_ANY, 0, 1);
    xcb_get_property_reply_t *prop_reply = xcb_get_property_reply(CONNECTION, prop_cookie, nullptr);

    if (!prop_reply) {
      qWarning() << "empty prop_reply";
      return result;
    }

    xcb_window_t *prop_ret = nullptr;
    if (prop_reply->value_len > 0 && prop_reply->type == XCB_ATOM_WINDOW &&
        prop_reply->format == HILDON_IM_WINDOW_ID_FORMAT) {
      const int length = xcb_get_property_value_length(prop_reply);
      prop_ret = static_cast<xcb_window_t *>(malloc(length));
      if (prop_ret) {
        memcpy(prop_ret, xcb_get_property_value(prop_reply), length);
        result = *prop_ret;
      }
    }

    free(prop_reply);
    if (result == 0) {
      qWarning("QHildonInputContext: Unable to get the Hildon IM window id");
      free(prop_ret);
    }

    qDebug() << "got hildon IM window id" << result;
    return result;
  }

  xcb_keysym_t XKeycodeToKeysym(const int keycode) {
    const xcb_setup_t *setup = xcb_get_setup(CONNECTION);
    xcb_generic_error_t *error = nullptr;

    const int mincode = setup->min_keycode;
    const int maxcode = setup->max_keycode;

    const xcb_get_keyboard_mapping_cookie_t cookie = xcb_get_keyboard_mapping(CONNECTION, mincode, maxcode - mincode + 1);
    xcb_get_keyboard_mapping_reply_t *reply = xcb_get_keyboard_mapping_reply(CONNECTION, cookie, &error);
    if (error || !reply) {
      qWarning("xcb_get_keyboard_mapping_reply failed");
      return 0;
    }

    return xcb_keycode_to_keysym(setup, reply, keycode);
  }

  void initialiseHildonAtoms() {
    xcb_intern_atom_cookie_t cookies[HILDON_IM_NUM_ATOMS];
    int i = 0;
    for(i = 0; i < HILDON_IM_NUM_ATOMS; ++i) {
      QString name = hildonAtomNames.at(i);
      name = "_" + name;
      cookies[i] = xcb_intern_atom(CONNECTION, false, name.length(), name.toStdString().c_str());
    }

    for (i = 0; i < HILDON_IM_NUM_ATOMS; ++i) {
      xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(CONNECTION, cookies[i], nullptr);
      QHildonIMAtom atom;
      atom.name = hildonAtomNames.at(i);
      atom.hildon_enum = static_cast<HildonIMAtom>(i);
      atom.xcb_atom = reply->atom;
      HILDON_ATOM_MAP[reply->atom] = atom;
      free(reply);
    }

    Q_ASSERT(i == HILDON_IM_NUM_ATOMS);
  }

  void initialiseHildonComEnums() {
    for (int i = 0; i < hildonComNames.length(); ++i) {
      const QString& name = hildonComNames.at(i);
      QHildonIMCom com;
      com.name = name;
      com.value = static_cast<HildonIMCommunication>(i);
      HILDON_COM_MAP[i] = com;
    }
  }
}
