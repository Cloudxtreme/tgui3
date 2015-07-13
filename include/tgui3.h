#ifndef TGUI3_H
#define TGUI3_H

#include <climits>
#include <vector>

#ifdef TGUI_WINDOWS
#pragma warning(disable : 4251)
#ifdef TGUI_LIB_BUILD
#define TGUI_EXPORT __declspec(dllexport)
#else
#define TGUI_EXPORT __declspec(dllimport)
#endif
#else
#define TGUI_EXPORT
#endif

enum TGUI_Event_Type {
	TGUI_UNKNOWN,
	TGUI_KEY_DOWN,
	TGUI_KEY_UP,
	TGUI_MOUSE_DOWN,
	TGUI_MOUSE_UP,
	TGUI_MOUSE_AXIS,
	TGUI_JOY_DOWN,
	TGUI_JOY_UP,
	TGUI_JOY_AXIS
};

struct TGUI_Event {
	TGUI_Event_Type type;
	struct TGUI_Event_Keyboard {
		int code;
	} keyboard;
	struct TGUI_Event_Mouse {
		int button;
		int x;
		int y;
	} mouse;
	struct TGUI_Event_Joystick {
		int button;
		int axis;
		float value; // -1.0 -> 1.0
	} joystick;
};

class TGUI;
class TGUI_Widget;

TGUI_EXPORT void tgui_get_size(TGUI_Widget *parent, TGUI_Widget *widget, int *width, int *height);
TGUI_EXPORT TGUI_Event tgui_get_relative_event(TGUI_Widget *widget, TGUI_Event *event);

// a GUI hierarchy
class TGUI_EXPORT TGUI {
	friend TGUI_EXPORT void tgui_get_size(TGUI_Widget *parent, TGUI_Widget *widget, int *width, int *height);

public:
	TGUI(TGUI_Widget *main_widget, int w, int h);
	~TGUI();

	void layout();
	void resize(int w, int h);
	void draw();
	void handle_event(TGUI_Event *event);

	void set_focus(TGUI_Widget *widget);
	void focus_something();
	void set_offset(int offset_x, int offset_y);

	TGUI_Widget *get_focus();
	TGUI_Widget *get_event_owner(TGUI_Event *event);

private:
	void destroy(TGUI_Widget *widget);
	void set_sizes(TGUI_Widget *widget);
	void set_positions(TGUI_Widget *widget, int x, int y);
	void draw(TGUI_Widget *widget);
	TGUI_Widget *get_event_owner(TGUI_Event *event, TGUI_Widget *widget);
	void handle_event(TGUI_Event *event, TGUI_Widget *widget);
	bool focus_something(TGUI_Widget *widget);
	void focus_distance(TGUI_Widget *start, TGUI_Widget *widget, int dir_x, int dir_y, int &score, int &grade);
	void find_focus(TGUI_Widget *start, TGUI_Widget *&current_best, TGUI_Widget *widget, int dir_x, int dir_y, int &best_score, int &best_grade);

	TGUI_Widget *main_widget;
	TGUI_Widget *focus;
	int w, h;
	int offset_x, offset_y;
};

class TGUI_Widget;

class TGUI_EXPORT TGUI_Widget {
	friend TGUI_EXPORT void tgui_get_size(TGUI_Widget *parent, TGUI_Widget *widget, int *width, int *height);
	friend class TGUI_EXPORT TGUI;

public:
	/* Percentage sizes can be negative or positive: positive means % of parent size,
	 * negative means % of whatever's left after fixed and positive percentage sized
	 * widgets.
	 */
	TGUI_Widget(int w, int h);
	TGUI_Widget(float percent_w, float percent_h);
	TGUI_Widget(int w, float percent_h);
	TGUI_Widget(float percent_w, int h);
	virtual ~TGUI_Widget();

	virtual void draw() {}
	virtual void handle_event(TGUI_Event *event) {}

	void set_parent(TGUI_Widget *widget);
	void set_padding(int padding);
	void set_padding(int left, int right, int top, int bottom);
	void set_float_right(bool float_right);
	void set_centered_x(bool centered_x);
	void set_clear_floats(bool clear_floats);
	void set_accepts_focus(bool accepts_focus);

	TGUI_Widget *get_parent();
	int get_x();
	int get_y();
	int get_width();
	int get_height();
	int get_padding_left();
	int get_padding_right();
	int get_padding_top();
	int get_padding_bottom();

protected:
	int get_right_pos();

	TGUI *gui;
	TGUI_Widget *parent;
	bool percent_x, percent_y;
	float percent_w, percent_h;
	int w, h;
	std::vector<TGUI_Widget *> children;
	int padding_left, padding_right, padding_top, padding_bottom;
	bool float_right;
	bool centered_x;
	bool accepts_focus;

	int calculated_x, calculated_y;
	int calculated_w, calculated_h;

	bool clear_floats;
};

#ifdef WITH_SDL
#include <SDL2/SDL.h>

enum
{
	TGUIK_UNKNOWN = SDLK_UNKNOWN,
	TGUIK_RETURN = SDLK_RETURN,
	TGUIK_ESCAPE = SDLK_ESCAPE,
	TGUIK_BACKSPACE = SDLK_BACKSPACE,
	TGUIK_TAB = SDLK_TAB,
	TGUIK_SPACE = SDLK_SPACE,
	TGUIK_EXCLAIM = SDLK_EXCLAIM,
	TGUIK_QUOTEDBL = SDLK_QUOTEDBL,
	TGUIK_HASH = SDLK_HASH,
	TGUIK_PERCENT = SDLK_PERCENT,
	TGUIK_DOLLAR = SDLK_DOLLAR,
	TGUIK_AMPERSAND = SDLK_AMPERSAND,
	TGUIK_QUOTE = SDLK_QUOTE,
	TGUIK_LEFTPAREN = SDLK_LEFTPAREN,
	TGUIK_RIGHTPAREN = SDLK_RIGHTPAREN,
	TGUIK_ASTERISK = SDLK_ASTERISK,
	TGUIK_PLUS = SDLK_PLUS,
	TGUIK_COMMA = SDLK_COMMA,
	TGUIK_MINUS = SDLK_MINUS,
	TGUIK_PERIOD = SDLK_PERIOD,
	TGUIK_SLASH = SDLK_SLASH,
	TGUIK_0 = SDLK_0,
	TGUIK_1 = SDLK_1,
	TGUIK_2 = SDLK_2,
	TGUIK_3 = SDLK_3,
	TGUIK_4 = SDLK_4,
	TGUIK_5 = SDLK_5,
	TGUIK_6 = SDLK_6,
	TGUIK_7 = SDLK_7,
	TGUIK_8 = SDLK_8,
	TGUIK_9 = SDLK_9,
	TGUIK_COLON = SDLK_COLON,
	TGUIK_SEMICOLON = SDLK_SEMICOLON,
	TGUIK_LESS = SDLK_LESS,
	TGUIK_EQUALS = SDLK_EQUALS,
	TGUIK_GREATER = SDLK_GREATER,
	TGUIK_QUESTION = SDLK_QUESTION,
	TGUIK_AT = SDLK_AT,
	TGUIK_LEFTBRACKET = SDLK_LEFTBRACKET,
	TGUIK_BACKSLASH = SDLK_BACKSLASH,
	TGUIK_RIGHTBRACKET = SDLK_RIGHTBRACKET,
	TGUIK_CARET = SDLK_CARET,
	TGUIK_UNDERSCORE = SDLK_UNDERSCORE,
	TGUIK_BACKQUOTE = SDLK_BACKQUOTE,
	TGUIK_a = SDLK_a,
	TGUIK_b = SDLK_b,
	TGUIK_c = SDLK_c,
	TGUIK_d = SDLK_d,
	TGUIK_e = SDLK_e,
	TGUIK_f = SDLK_f,
	TGUIK_g = SDLK_g,
	TGUIK_h = SDLK_h,
	TGUIK_i = SDLK_i,
	TGUIK_j = SDLK_j,
	TGUIK_k = SDLK_k,
	TGUIK_l = SDLK_l,
	TGUIK_m = SDLK_m,
	TGUIK_n = SDLK_n,
	TGUIK_o = SDLK_o,
	TGUIK_p = SDLK_p,
	TGUIK_q = SDLK_q,
	TGUIK_r = SDLK_r,
	TGUIK_s = SDLK_s,
	TGUIK_t = SDLK_t,
	TGUIK_u = SDLK_u,
	TGUIK_v = SDLK_v,
	TGUIK_w = SDLK_w,
	TGUIK_x = SDLK_x,
	TGUIK_y = SDLK_y,
	TGUIK_z = SDLK_z,
	TGUIK_CAPSLOCK = SDLK_CAPSLOCK,
	TGUIK_F1 = SDLK_F1,
	TGUIK_F2 = SDLK_F2,
	TGUIK_F3 = SDLK_F3,
	TGUIK_F4 = SDLK_F4,
	TGUIK_F5 = SDLK_F5,
	TGUIK_F6 = SDLK_F6,
	TGUIK_F7 = SDLK_F7,
	TGUIK_F8 = SDLK_F8,
	TGUIK_F9 = SDLK_F9,
	TGUIK_F10 = SDLK_F10,
	TGUIK_F11 = SDLK_F11,
	TGUIK_F12 = SDLK_F12,
	TGUIK_PRINTSCREEN = SDLK_PRINTSCREEN,
	TGUIK_SCROLLLOCK = SDLK_SCROLLLOCK,
	TGUIK_PAUSE = SDLK_PAUSE,
	TGUIK_INSERT = SDLK_INSERT,
	TGUIK_HOME = SDLK_HOME,
	TGUIK_PAGEUP = SDLK_PAGEUP,
	TGUIK_DELETE = SDLK_DELETE,
	TGUIK_END = SDLK_END,
	TGUIK_PAGEDOWN = SDLK_PAGEDOWN,
	TGUIK_RIGHT = SDLK_RIGHT,
	TGUIK_LEFT = SDLK_LEFT,
	TGUIK_DOWN = SDLK_DOWN,
	TGUIK_UP = SDLK_UP,
	TGUIK_NUMLOCKCLEAR = SDLK_NUMLOCKCLEAR,
	TGUIK_KP_DIVIDE = SDLK_KP_DIVIDE,
	TGUIK_KP_MULTIPLY = SDLK_KP_MULTIPLY,
	TGUIK_KP_MINUS = SDLK_KP_MINUS,
	TGUIK_KP_PLUS = SDLK_KP_PLUS,
	TGUIK_KP_ENTER = SDLK_KP_ENTER,
	TGUIK_KP_1 = SDLK_KP_1,
	TGUIK_KP_2 = SDLK_KP_2,
	TGUIK_KP_3 = SDLK_KP_3,
	TGUIK_KP_4 = SDLK_KP_4,
	TGUIK_KP_5 = SDLK_KP_5,
	TGUIK_KP_6 = SDLK_KP_6,
	TGUIK_KP_7 = SDLK_KP_7,
	TGUIK_KP_8 = SDLK_KP_8,
	TGUIK_KP_9 = SDLK_KP_9,
	TGUIK_KP_0 = SDLK_KP_0,
	TGUIK_KP_PERIOD = SDLK_KP_PERIOD,
	TGUIK_APPLICATION = SDLK_APPLICATION,
	TGUIK_POWER = SDLK_POWER,
	TGUIK_KP_EQUALS = SDLK_KP_EQUALS,
	TGUIK_F13 = SDLK_F13,
	TGUIK_F14 = SDLK_F14,
	TGUIK_F15 = SDLK_F15,
	TGUIK_F16 = SDLK_F16,
	TGUIK_F17 = SDLK_F17,
	TGUIK_F18 = SDLK_F18,
	TGUIK_F19 = SDLK_F19,
	TGUIK_F20 = SDLK_F20,
	TGUIK_F21 = SDLK_F21,
	TGUIK_F22 = SDLK_F22,
	TGUIK_F23 = SDLK_F23,
	TGUIK_F24 = SDLK_F24,
	TGUIK_EXECUTE = SDLK_EXECUTE,
	TGUIK_HELP = SDLK_HELP,
	TGUIK_MENU = SDLK_MENU,
	TGUIK_SELECT = SDLK_SELECT,
	TGUIK_STOP = SDLK_STOP,
	TGUIK_AGAIN = SDLK_AGAIN,
	TGUIK_UNDO = SDLK_UNDO,
	TGUIK_CUT = SDLK_CUT,
	TGUIK_COPY = SDLK_COPY,
	TGUIK_PASTE = SDLK_PASTE,
	TGUIK_FIND = SDLK_FIND,
	TGUIK_MUTE = SDLK_MUTE,
	TGUIK_VOLUMEUP = SDLK_VOLUMEUP,
	TGUIK_VOLUMEDOWN = SDLK_VOLUMEDOWN,
	TGUIK_KP_COMMA = SDLK_KP_COMMA,
	TGUIK_KP_EQUALSAS400 = SDLK_KP_EQUALSAS400,
	TGUIK_ALTERASE = SDLK_ALTERASE,
	TGUIK_SYSREQ = SDLK_SYSREQ,
	TGUIK_CANCEL = SDLK_CANCEL,
	TGUIK_CLEAR = SDLK_CLEAR,
	TGUIK_PRIOR = SDLK_PRIOR,
	TGUIK_RETURN2 = SDLK_RETURN2,
	TGUIK_SEPARATOR = SDLK_SEPARATOR,
	TGUIK_OUT = SDLK_OUT,
	TGUIK_OPER = SDLK_OPER,
	TGUIK_CLEARAGAIN = SDLK_CLEARAGAIN,
	TGUIK_CRSEL = SDLK_CRSEL,
	TGUIK_EXSEL = SDLK_EXSEL,
	TGUIK_KP_00 = SDLK_KP_00,
	TGUIK_KP_000 = SDLK_KP_000,
	TGUIK_THOUSANDSSEPARATOR = SDLK_THOUSANDSSEPARATOR,
	TGUIK_DECIMALSEPARATOR = SDLK_DECIMALSEPARATOR,
	TGUIK_CURRENCYUNIT = SDLK_CURRENCYUNIT,
	TGUIK_CURRENCYSUBUNIT = SDLK_CURRENCYSUBUNIT,
	TGUIK_KP_LEFTPAREN = SDLK_KP_LEFTPAREN,
	TGUIK_KP_RIGHTPAREN = SDLK_KP_RIGHTPAREN,
	TGUIK_KP_LEFTBRACE = SDLK_KP_LEFTBRACE,
	TGUIK_KP_RIGHTBRACE = SDLK_KP_RIGHTBRACE,
	TGUIK_KP_TAB = SDLK_KP_TAB,
	TGUIK_KP_BACKSPACE = SDLK_KP_BACKSPACE,
	TGUIK_KP_A = SDLK_KP_A,
	TGUIK_KP_B = SDLK_KP_B,
	TGUIK_KP_C = SDLK_KP_C,
	TGUIK_KP_D = SDLK_KP_D,
	TGUIK_KP_E = SDLK_KP_E,
	TGUIK_KP_F = SDLK_KP_F,
	TGUIK_KP_XOR = SDLK_KP_XOR,
	TGUIK_KP_POWER = SDLK_KP_POWER,
	TGUIK_KP_PERCENT = SDLK_KP_PERCENT,
	TGUIK_KP_LESS = SDLK_KP_LESS,
	TGUIK_KP_GREATER = SDLK_KP_GREATER,
	TGUIK_KP_AMPERSAND = SDLK_KP_AMPERSAND,
	TGUIK_KP_DBLAMPERSAND = SDLK_KP_DBLAMPERSAND,
	TGUIK_KP_VERTICALBAR = SDLK_KP_VERTICALBAR,
	TGUIK_KP_DBLVERTICALBAR = SDLK_KP_DBLVERTICALBAR,
	TGUIK_KP_COLON = SDLK_KP_COLON,
	TGUIK_KP_HASH = SDLK_KP_HASH,
	TGUIK_KP_SPACE = SDLK_KP_SPACE,
	TGUIK_KP_AT = SDLK_KP_AT,
	TGUIK_KP_EXCLAM = SDLK_KP_EXCLAM,
	TGUIK_KP_MEMSTORE = SDLK_KP_MEMSTORE,
	TGUIK_KP_MEMRECALL = SDLK_KP_MEMRECALL,
	TGUIK_KP_MEMCLEAR = SDLK_KP_MEMCLEAR,
	TGUIK_KP_MEMADD = SDLK_KP_MEMADD,
	TGUIK_KP_MEMSUBTRACT = SDLK_KP_MEMSUBTRACT,
	TGUIK_KP_MEMMULTIPLY = SDLK_KP_MEMMULTIPLY,
	TGUIK_KP_MEMDIVIDE = SDLK_KP_MEMDIVIDE,
	TGUIK_KP_PLUSMINUS = SDLK_KP_PLUSMINUS,
	TGUIK_KP_CLEAR = SDLK_KP_CLEAR,
	TGUIK_KP_CLEARENTRY = SDLK_KP_CLEARENTRY,
	TGUIK_KP_BINARY = SDLK_KP_BINARY,
	TGUIK_KP_OCTAL = SDLK_KP_OCTAL,
	TGUIK_KP_DECIMAL = SDLK_KP_DECIMAL,
	TGUIK_KP_HEXADECIMAL = SDLK_KP_HEXADECIMAL,
	TGUIK_LCTRL = SDLK_LCTRL,
	TGUIK_LSHIFT = SDLK_LSHIFT,
	TGUIK_LALT = SDLK_LALT,
	TGUIK_LGUI = SDLK_LGUI,
	TGUIK_RCTRL = SDLK_RCTRL,
	TGUIK_RSHIFT = SDLK_RSHIFT,
	TGUIK_RALT = SDLK_RALT,
	TGUIK_RGUI = SDLK_RGUI,
	TGUIK_MODE = SDLK_MODE,
	TGUIK_AUDIONEXT = SDLK_AUDIONEXT,
	TGUIK_AUDIOPREV = SDLK_AUDIOPREV,
	TGUIK_AUDIOSTOP = SDLK_AUDIOSTOP,
	TGUIK_AUDIOPLAY = SDLK_AUDIOPLAY,
	TGUIK_AUDIOMUTE = SDLK_AUDIOMUTE,
	TGUIK_MEDIASELECT = SDLK_MEDIASELECT,
	TGUIK_WWW = SDLK_WWW,
	TGUIK_MAIL = SDLK_MAIL,
	TGUIK_CALCULATOR = SDLK_CALCULATOR,
	TGUIK_COMPUTER = SDLK_COMPUTER,
	TGUIK_AC_SEARCH = SDLK_AC_SEARCH,
	TGUIK_AC_HOME = SDLK_AC_HOME,
	TGUIK_AC_BACK = SDLK_AC_BACK,
	TGUIK_AC_FORWARD = SDLK_AC_FORWARD,
	TGUIK_AC_STOP = SDLK_AC_STOP,
	TGUIK_AC_REFRESH = SDLK_AC_REFRESH,
	TGUIK_AC_BOOKMARKS = SDLK_AC_BOOKMARKS,
	TGUIK_BRIGHTNESSDOWN = SDLK_BRIGHTNESSDOWN,
	TGUIK_BRIGHTNESSUP = SDLK_BRIGHTNESSUP,
	TGUIK_DISPLAYSWITCH = SDLK_DISPLAYSWITCH,
	TGUIK_KBDILLUMTOGGLE = SDLK_KBDILLUMTOGGLE,
	TGUIK_KBDILLUMDOWN = SDLK_KBDILLUMDOWN,
	TGUIK_KBDILLUMUP = SDLK_KBDILLUMUP,
	TGUIK_EJECT = SDLK_EJECT,
	TGUIK_SLEEP = SDLK_SLEEP,
};

TGUI_EXPORT TGUI_Event tgui_sdl_convert_event(SDL_Event *sdl_event);
#endif // WITH_SDL

#endif // TGUI3_H
