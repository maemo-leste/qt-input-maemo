#include "utils-xcb.h"
#include "utils-hildon.h"

namespace QXcb {
  // extern vars
  xcb_connection_t* CONNECTION = nullptr;
  xcb_atom_t ATOMS[QXcbHIMAtom::NAtoms]{};

  xcb_window_t findHildonIm() {
    xcb_window_t result = 0;
#if 0
    // meh
    int screen_no = QX11Info::appScreen();
    xcb_screen_t  * screen = screen_of_display(conn, screen_no);
    xcb_window_t root_window = 0;
    if (screen)
      root_window = screen->root;
#endif
    auto root_window = static_cast<xcb_window_t>(QX11Info::appRootWindow(-1));

    xcb_get_property_cookie_t prop_cookie = xcb_get_property(CONNECTION, 0, root_window, atom(QXcbHIMAtom::_HILDON_IM_WINDOW), XCB_GET_PROPERTY_TYPE_ANY, 0, 1);
    xcb_get_property_reply_t* prop_reply = xcb_get_property_reply(CONNECTION, prop_cookie, nullptr);

    if (!prop_reply) {
      qWarning() << "prop_reply != !!!!!!!";
      return result;
    }

    xcb_window_t *prop_ret = nullptr;

    if ((prop_reply->value_len > 0) && (prop_reply->type == XCB_ATOM_WINDOW) && (prop_reply->format == HILDON_IM_WINDOW_ID_FORMAT)) {
      const int length = xcb_get_property_value_length(prop_reply);
      prop_ret = static_cast<xcb_window_t *>(malloc(length));
      if (prop_ret) {
        memcpy(prop_ret, xcb_get_property_value(prop_reply), length);
        result = *prop_ret;

        std::cerr << "Window HILDON ID:" << result << std::endl;
      }
    }

    free(prop_reply);
    if (result == 0) {
      // XXX: QHildonInputContext is old name/str
      qWarning("QHildonInputContext: Unable to get the Hildon IM window id");
      free(prop_ret);
    }

    qDebug() << "got hildon IM window id";
    return result;
  }

  void initialiseAtoms() {  // Function taken frm XcbPlatform plugin
    const char *names[QXcbHIMAtom::NAtoms];
    const char *ptr = xcb_atomnames;

    int i = 0;
    while (*ptr) {
      names[i++] = ptr;
      while (*ptr)
        ++ptr;
      ++ptr;
    }

    Q_ASSERT(i == QXcbHIMAtom::NAtoms);

    xcb_intern_atom_cookie_t cookies[QXcbHIMAtom::NAtoms];

    Q_ASSERT(i == QXcbHIMAtom::NAtoms);
    for (i = 0; i < QXcbHIMAtom::NAtoms; ++i)
      cookies[i] = xcb_intern_atom(QXcb::CONNECTION, false, strlen(names[i]), names[i]);

    for (i = 0; i < QXcbHIMAtom::NAtoms; ++i) {
      xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(QXcb::CONNECTION, cookies[i], nullptr);
      ATOMS[i] = reply->atom;
      free(reply);
    }
  }
}


