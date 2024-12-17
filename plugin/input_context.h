#pragma once
#include <QObject>
#include <QDataStream>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QApplication>
#include <QAbstractNativeEventFilter>
#include <QtCore>
#include <QWidget>
#include <QCursor>
#include <QX11Info>
#include <iostream>

//#include "lib/xcb-maemo/qxcbintegration.h"
//#include "lib/xcb-maemo/qxcbconnection.h"

#include <xcb/xcb.h>

// Include this last, otherwise things blow up
// #include <hildon-im-protocol.h>
// extern QApplication *currentApp;

// XXX: forward-define this for now
class MyXcbEventFilter;

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

class QHildonInputContext : public QPlatformInputContext {
public:
  QHildonInputContext() {
    qDebug() << "QHildonInputContext created";
  }

  ~QHildonInputContext() override = default;

  bool isValid() const override {
    return true; // The input context is valid and can be used.
  }

  void setFocusObject(QObject *object) override {
    if (object) {
      qDebug() << "Focus set on object:" << object->objectName();
    } else {
      qDebug() << "Focus cleared.";
    }
  }
};

//class QHInputContext : public QPlatformInputContext {
//Q_OBJECT
//
//public:
//    QHInputContext();
//    ~QHInputContext();
//
//    bool isValid() const override;
//    void setFocusObject(QObject *object) override;
//    void reset() override;
//    void update(Qt::InputMethodQueries) override;
//    bool filterEvent(const QEvent *event) override;
//    void lol(QWidget *w);
//
//    void setCommitMode(HildonIMCommitMode mode, bool clearPreEdit);
//    void clearSelection();
//    void cancelPreedit();
//    void checkSentenceStart();
//    QWidget* focusWidget() const;
//    void updateInputMethodHints();
//
//    bool x11FilterEvent(xcb_client_message_event_t *event);
//    inline xcb_atom_t atom(QXcbHIMAtom::Atom atom) const { return m_allAtoms[atom]; }
//protected:
//
//private:
//    MyXcbEventFilter *eventFilter;
//
//    void initialiseAtoms(void);
//    xcb_atom_t m_allAtoms[QXcbHIMAtom::NAtoms];
//
//    void sendHildonCommand(HildonIMCommand cmd, QWidget *widget);
//    void sendInputMode();
//    xcb_window_t findHildonIm(void);
//
//    xcb_connection_t* getXcbConnection(void);
//    void printEventName(xcb_client_message_event_t* message_event);
//
//    //Vars
//    int mask;
//    int options;
//    HildonIMTrigger triggerMode;
//    HildonIMCommitMode commitMode, lastCommitMode;
//    int inputMode;
//    QString preEditBuffer;
//    int textCursorPosOnPress; //position of the cursor in the surrounding text at the last TabletPress event
//    bool autoUpper;
//    bool lastInternalChange;
//    bool spaceAfterCommit;
//    QTimer *longPressTimer;
//    QScopedPointer<QKeyEvent> longPressKeyEvent;
//    QWidget *lastKeyWidget;
//    int lastQtKeyCode;
//    QChar combiningChar;      // Unicode representation of the dead key (combining)
//    QChar plainCombiningChar; // Unicode representation of the dead key (plain)
//    QString lastCommitString;
//
//    QPointer<QWidget> realFocus; // the widget that really has the focus in case of a QGraphicsProxyWidget
//    QPointer<QWidget> lastFocus; // the widget that last had the focus (workaround for HIM bug)
//};
//
//static xcb_screen_t *screen_of_display (xcb_connection_t *c, int screen);
