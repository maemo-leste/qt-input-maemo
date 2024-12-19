#pragma once
#include <cstdint>

// this is mostly /usr/include/hildon-input-method/hildon-im-protocol.h

#define HILDON_IM_CLIENT_MESSAGE_BUFFER_SIZE (20 - sizeof(int))

typedef enum
{
  HILDON_IM_MODE,
  HILDON_IM_SHOW,
  HILDON_IM_HIDE,
  HILDON_IM_UP,
  HILDON_IM_LOW,
  HILDON_IM_DESTROY,
  HILDON_IM_CLEAR,
  HILDON_IM_SETCLIENT,
  HILDON_IM_SETNSHOW,
  HILDON_IM_SELECT_ALL,

  HILDON_IM_SHIFT_LOCKED,
  HILDON_IM_SHIFT_UNLOCKED,
  HILDON_IM_MOD_LOCKED,
  HILDON_IM_MOD_UNLOCKED,
  HILDON_IM_SHIFT_STICKY,
  HILDON_IM_SHIFT_UNSTICKY,
  HILDON_IM_MOD_STICKY,
  HILDON_IM_MOD_UNSTICKY,

  /* always last */
  HILDON_IM_NUM_COMMANDS
} HildonIMCommand;

typedef enum
{
  HILDON_IM_WINDOW,
  HILDON_IM_ACTIVATE,
  HILDON_IM_INPUT_MODE,
  HILDON_IM_INSERT_UTF8,
  HILDON_IM_SURROUNDING,
  HILDON_IM_SURROUNDING_CONTENT,
  HILDON_IM_KEY_EVENT,
  HILDON_IM_COM,
  HILDON_IM_CLIPBOARD_COPIED,
  HILDON_IM_CLIPBOARD_SELECTION_QUERY,
  HILDON_IM_CLIPBOARD_SELECTION_REPLY,
  HILDON_IM_PREEDIT_COMMITTED,
  HILDON_IM_PREEDIT_COMMITTED_CONTENT,
  HILDON_IM_LONG_PRESS_SETTINGS,

  /* always last */
  HILDON_IM_NUM_ATOMS
} HildonIMAtom;

#include <QMap>
#include <xcb/xcb.h>

typedef enum
{
  HILDON_IM_CONTEXT_HANDLE_ENTER,
  HILDON_IM_CONTEXT_HANDLE_TAB,
  HILDON_IM_CONTEXT_HANDLE_BACKSPACE,
  HILDON_IM_CONTEXT_HANDLE_SPACE,
  HILDON_IM_CONTEXT_CONFIRM_SENTENCE_START,
  HILDON_IM_CONTEXT_FLUSH_PREEDIT,
  HILDON_IM_CONTEXT_CANCEL_PREEDIT,

  HILDON_IM_CONTEXT_BUFFERED_MODE,
  HILDON_IM_CONTEXT_DIRECT_MODE,
  HILDON_IM_CONTEXT_REDIRECT_MODE,
  HILDON_IM_CONTEXT_SURROUNDING_MODE,
  HILDON_IM_CONTEXT_PREEDIT_MODE,

  HILDON_IM_CONTEXT_CLIPBOARD_COPY,
  HILDON_IM_CONTEXT_CLIPBOARD_CUT,
  HILDON_IM_CONTEXT_CLIPBOARD_PASTE,
  HILDON_IM_CONTEXT_CLIPBOARD_SELECTION_QUERY,
  HILDON_IM_CONTEXT_REQUEST_SURROUNDING,
  HILDON_IM_CONTEXT_REQUEST_SURROUNDING_FULL,
  HILDON_IM_CONTEXT_WIDGET_CHANGED,
  HILDON_IM_CONTEXT_OPTION_CHANGED,
  HILDON_IM_CONTEXT_ENTER_ON_FOCUS,

  HILDON_IM_CONTEXT_SPACE_AFTER_COMMIT,
  HILDON_IM_CONTEXT_NO_SPACE_AFTER_COMMIT,

  HILDON_IM_CONTEXT_SHIFT_LOCKED,
  HILDON_IM_CONTEXT_SHIFT_UNLOCKED,
  HILDON_IM_CONTEXT_LEVEL_LOCKED,
  HILDON_IM_CONTEXT_LEVEL_UNLOCKED,
  HILDON_IM_CONTEXT_SHIFT_UNSTICKY,
  HILDON_IM_CONTEXT_LEVEL_UNSTICKY,

  /* always last */
  HILDON_IM_CONTEXT_NUM_COM
} HildonIMCommunication;

typedef enum {
  HILDON_IM_AUTOCASE          = 1 << 0,
  HILDON_IM_AUTOCORRECT       = 1 << 1,
  HILDON_IM_AUTOLEVEL_NUMERIC = 1 << 2,
  HILDON_IM_LOCK_LEVEL        = 1 << 3,
} HildonIMOptionMask;

typedef enum
{
  HILDON_IM_TRIGGER_NONE = -1,
  HILDON_IM_TRIGGER_STYLUS,
  HILDON_IM_TRIGGER_FINGER,
  HILDON_IM_TRIGGER_KEYBOARD,
  HILDON_IM_TRIGGER_UNKNOWN
} HildonIMTrigger;

typedef enum
{
  HILDON_IM_COMMIT_DIRECT,
  HILDON_IM_COMMIT_REDIRECT,
  HILDON_IM_COMMIT_SURROUNDING,
  HILDON_IM_COMMIT_BUFFERED,
  HILDON_IM_COMMIT_PREEDIT
} HildonIMCommitMode;

typedef enum {
  HILDON_IM_SHIFT_STICKY_MASK     = 1 << 0,
  HILDON_IM_SHIFT_LOCK_MASK       = 1 << 1,
  HILDON_IM_LEVEL_STICKY_MASK     = 1 << 2,
  HILDON_IM_LEVEL_LOCK_MASK       = 1 << 3,
  HILDON_IM_COMPOSE_MASK          = 1 << 4,
  HILDON_IM_DEAD_KEY_MASK         = 1 << 5
} HildonIMInternalModifierMask;

typedef enum {
  HILDON_SIZE_AUTO_WIDTH       = 0 << 0, /* set to automatic width */
  HILDON_SIZE_HALFSCREEN_WIDTH = 1 << 0, /* set to 50% screen width */
  HILDON_SIZE_FULLSCREEN_WIDTH = 2 << 0, /* set to 100% screen width */
  HILDON_SIZE_AUTO_HEIGHT      = 0 << 2, /* set to automatic height */
  HILDON_SIZE_FINGER_HEIGHT    = 1 << 2, /* set to finger height */
  HILDON_SIZE_THUMB_HEIGHT     = 2 << 2, /* set to thumb height */
  HILDON_SIZE_AUTO             = (HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_AUTO_HEIGHT)
} HildonSizeType;

typedef enum
{
  HILDON_DIABLO,
  HILDON_FREMANTLE
} HildonMode;

typedef enum
{
  HILDON_UI_MODE_NORMAL,
  HILDON_UI_MODE_EDIT
} HildonUIMode;

typedef struct
{
  int input_window;
  int app_window;
  HildonIMCommand cmd;
  HildonIMTrigger trigger;
} HildonIMActivateMessage;

typedef enum
{
  HILDON_GTK_INPUT_MODE_ALPHA             = 1 << 0,
  HILDON_GTK_INPUT_MODE_NUMERIC           = 1 << 1,
  HILDON_GTK_INPUT_MODE_SPECIAL           = 1 << 2,
  HILDON_GTK_INPUT_MODE_HEXA              = 1 << 3,
  HILDON_GTK_INPUT_MODE_TELE              = 1 << 4,
  HILDON_GTK_INPUT_MODE_FULL              = (HILDON_GTK_INPUT_MODE_ALPHA | HILDON_GTK_INPUT_MODE_NUMERIC | HILDON_GTK_INPUT_MODE_SPECIAL),
  HILDON_GTK_INPUT_MODE_NO_SCREEN_PLUGINS = 1 << 27,
  HILDON_GTK_INPUT_MODE_MULTILINE         = 1 << 28,
  HILDON_GTK_INPUT_MODE_INVISIBLE         = 1 << 29,
  HILDON_GTK_INPUT_MODE_AUTOCAP           = 1 << 30,
  HILDON_GTK_INPUT_MODE_DICTIONARY        = 1 << 31
} HildonGtkInputMode;

typedef struct
{
  HildonGtkInputMode input_mode;
  HildonGtkInputMode default_input_mode;
} HildonIMInputModeMessage;

/* Text insertion message, from IM to context */
typedef struct
{
  int msg_flag;
  char utf8_str[HILDON_IM_CLIENT_MESSAGE_BUFFER_SIZE];
} HildonIMInsertUtf8Message;

/* Message carrying surrounding interpretation info, sent by both IM and context */
typedef struct
{
  HildonIMCommitMode commit_mode;
  int offset_is_relative;
  int cursor_offset;
} HildonIMSurroundingMessage;

/* The surrounding text, sent by both IM and context */
typedef struct
{
  int msg_flag;
  char surrounding[HILDON_IM_CLIENT_MESSAGE_BUFFER_SIZE];
} HildonIMSurroundingContentMessage;

/* Message carrying information about the committed preedit */
typedef struct
{
  int msg_flag;
  HildonIMCommitMode commit_mode;
} HildonIMPreeditCommittedMessage;

/* The committed preedit text, sent by context */
typedef struct
{
  int msg_flag;
  char committed_preedit[HILDON_IM_CLIENT_MESSAGE_BUFFER_SIZE];
} HildonIMPreeditCommittedContentMessage;

/* Key event message, from context to IM */
typedef struct
{
  int input_window;
  int type;
  unsigned int state;
  unsigned int keyval;
  unsigned int hardware_keycode;
} HildonIMKeyEventMessage;

/* Type markers for IM messages that span several ClientMessages */
enum
{
  HILDON_IM_MSG_START,
  HILDON_IM_MSG_CONTINUE,
  HILDON_IM_MSG_END
};

/* Communication message from IM to context */
typedef struct
{
  int input_window;
  HildonIMCommunication type;
  HildonIMOptionMask options;

} HildonIMComMessage;

/* Long-press settings message from IM to context */
typedef struct
{
  bool enable_long_press;
  uint16_t long_press_timeout;
} HildonIMLongPressSettingsMessage;

#define HILDON_IM_WINDOW_NAME                    "_HILDON_IM_WINDOW"
#define HILDON_IM_ACTIVATE_NAME                  "_HILDON_IM_ACTIVATE"
#define HILDON_IM_INPUT_MODE_NAME                "_HILDON_IM_INPUT_MODE"
#define HILDON_IM_SURROUNDING_NAME               "_HILDON_IM_SURROUNDING"
#define HILDON_IM_SURROUNDING_CONTENT_NAME       "_HILDON_IM_SURROUNDING_CONTENT"
#define HILDON_IM_KEY_EVENT_NAME                 "_HILDON_IM_KEY_EVENT"
#define HILDON_IM_INSERT_UTF8_NAME               "_HILDON_IM_INSERT_UTF8"
#define HILDON_IM_COM_NAME                       "_HILDON_IM_COM"
#define HILDON_IM_CLIPBOARD_COPIED_NAME          "_HILDON_IM_CLIPBOARD_COPIED"
#define HILDON_IM_CLIPBOARD_SELECTION_QUERY_NAME "_HILDON_IM_CLIPBOARD_SELECTION_QUERY"
#define HILDON_IM_CLIPBOARD_SELECTION_REPLY_NAME "_HILDON_IM_CLIPBOARD_SELECTION_REPLY"
#define HILDON_IM_PREEDIT_COMMITTED_NAME         "_HILDON_IM_PREEDIT_COMMITTED"
#define HILDON_IM_PREEDIT_COMMITTED_CONTENT_NAME "_HILDON_IM_PREEDIT_COMMITTED_CONTENT"
#define HILDON_IM_LONG_PRESS_SETTINGS_NAME       "_HILDON_IM_LONG_PRESS_SETTINGS"

#define HILDON_IM_WINDOW_ID_FORMAT 32
#define HILDON_IM_ACTIVATE_FORMAT 8
#define HILDON_IM_INPUT_MODE_FORMAT 8
#define HILDON_IM_SURROUNDING_FORMAT 8
#define HILDON_IM_SURROUNDING_CONTENT_FORMAT 8
#define HILDON_IM_KEY_EVENT_FORMAT 8
#define HILDON_IM_INSERT_UTF8_FORMAT 8
#define HILDON_IM_COM_FORMAT 8
#define HILDON_IM_CLIPBOARD_FORMAT 32
#define HILDON_IM_CLIPBOARD_SELECTION_REPLY_FORMAT 32
#define HILDON_IM_PREEDIT_COMMITTED_FORMAT 8
#define HILDON_IM_PREEDIT_COMMITTED_CONTENT_FORMAT 8
#define HILDON_IM_LONG_PRESS_SETTINGS_FORMAT 32

// Qt5 port stuff
#include <QStringList>
#include <QMap>
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