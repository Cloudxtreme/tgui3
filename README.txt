TGUI3 is the latest version of TGUI which has been used in games such as
Monster RPG 2 and Crystal Picnic.  TGUI gets its name from the author's
initials, "Trent Gamblin". TGUI3's first use is the GUI for Monster RPG 3.

TGUI3 currently requires SDL, although effort has been made to keep platform
specifics sectioned off. Since TGUI3 doesn't do anything platform specific,
the only SDL function is tgui_sdl_convert_event which converts SDL events to
TGUI events, which you then feed to TGUI in your game loop.

TGUI3 does not draw anything to the screen. It lays widgets out and sends
events to the appropriate ones. This makes building your own widgets, which
you can style any way you like, very easy. The public interface you can supply
for each widget is very simple, just drawing and handling events. You call
gui->draw() to send off the appropriate draw events to your widgets when you
want the GUI drawn.

As an example, to create a button, you would draw it using the widget's
members calculated_x, calculated_y, calculated_w and calculated_h plus your
own data.  You would then handle the TGUI_MOUSE_DOWN, TGUI_KEY_DOWN,
TGUI_JOY_DOWN events and their complementing _UP events. There are also
TGUI_MOUSE_AXIS and TGUI_JOY_AXIS events.

You can convert key and button presses to TGUI_FOCUS events (e.g., arrow keys)
which will make TGUI focus the correct widget to the left, right, up or down
from the current one. You can see which widget has the focus with
gui->get_focus() (check if it's this for the current widget, which allows
you to display your own custom type of widget focus.)
