#include "tgui3.h"

TGUI::TGUI(TGUI_Widget *main_widget, int w, int h) :
	main_widget(main_widget),
	w(w),
	h(h),
	focus(NULL),
	offset_x(0),
	offset_y(0)
{
	layout();
	focus_something();
}

void TGUI::layout()
{
	set_sizes(main_widget);
	set_positions(main_widget, offset_x, offset_y);
}

void TGUI::resize(int w, int h)
{
	this->w = w;
	this->h = h;
	layout();
}

void TGUI::draw()
{
	draw(main_widget);
}

void TGUI::handle_event(TGUI_Event *event)
{
	int x = 0;
	int y = 0;
	if (event->type == TGUI_JOY_AXIS && fabs(event->joystick.value) > 0.25f) {
		if (event->joystick.axis == 0) {
			if (event->joystick.value < 0) {
				x = -1;
			}
			else {
				x = 1;
			}
		}
		else if (event->joystick.axis == 1) {
			if (event->joystick.value < 0) {
				y = -1;
			}
			else {
				y = 1;
			}
		}
	}
	else if (event->type == TGUI_KEY_DOWN) {
		if (event->keyboard.code == TGUIK_LEFT) {
			x = -1;
		}
		else if (event->keyboard.code == TGUIK_RIGHT) {
			x = 1;
		}
		else if (event->keyboard.code == TGUIK_UP) {
			y = -1;
		}
		else if (event->keyboard.code == TGUIK_DOWN) {
			y = 1;
		}
	}

	if (x == 0 && y == 0) {
		handle_event(event, main_widget);
	}
	else {
		TGUI_Widget *start = focus == NULL ? main_widget : focus;
		TGUI_Widget *best = start;
		int best_score = INT_MAX;
		int best_grade = 2;
		find_focus(start, best, main_widget, x, y, best_score, best_grade);
		focus = best;
	}
}

void TGUI::set_focus(TGUI_Widget *widget)
{
	focus = widget;
}

void TGUI::focus_something()
{
	focus_something(main_widget);
}

void TGUI::set_offset(int offset_x, int offset_y)
{
	this->offset_x = offset_x;
	this->offset_y = offset_y;
	layout();
}

TGUI_Widget *TGUI::get_focus()
{
	return focus;
}

TGUI_Widget *TGUI::get_event_owner(TGUI_Event *event)
{
	return get_event_owner(event, main_widget);
}

void TGUI::set_sizes(TGUI_Widget *widget)
{
	widget->gui = this;
	int width, height;
	tgui_get_size(widget->parent, widget, &width, &height);
	widget->calculated_w = width;
	widget->calculated_h = height;
	for (size_t i = 0; i < widget->children.size(); i++) {
		set_sizes(widget->children[i]);
	}
}

void TGUI::set_positions(TGUI_Widget *widget, int x, int y)
{
	widget->calculated_x = x;
	widget->calculated_y = y;

	int parent_width, parent_height;

	if (widget->parent) {
		parent_width = widget->parent->calculated_w + widget->parent->padding_left + widget->parent->padding_right;
		parent_height = widget->parent->calculated_h + widget->parent->padding_top + widget->parent->padding_bottom;
	}
	else {
		parent_width = w;
		parent_height = h;
	}

	int max_h = 0;
	int dx = x;
	int dy = y;

	for (size_t i = 0; i < widget->children.size(); i++) {
		TGUI_Widget *d = widget->children[i];

		int width = d->calculated_w + d->padding_left + d->padding_right;
		int height = d->calculated_h + d->padding_top + d->padding_bottom;

		if (dx + width > parent_width) {
			dx = x;
			dy += max_h;
			max_h = 0;
		}

		set_positions(d, dx+d->padding_left+d->get_right_pos(), dy+d->padding_top);

		if (d->float_right == false) {
			dx += width;
		}

		max_h = height > max_h ? height : max_h;
	}
}

void TGUI::draw(TGUI_Widget *widget)
{
	widget->draw();
	for (size_t i = 0; i < widget->children.size(); i++) {
		draw(widget->children[i]);
	}
}

TGUI_Widget *TGUI::get_event_owner(TGUI_Event *event, TGUI_Widget *widget)
{
	for (size_t i = 0; i < widget->children.size(); i++) {
		TGUI_Widget *d = get_event_owner(event, widget->children[i]);
		if (d != NULL) {
			return d;
		}
	}

	if (event->type == TGUI_MOUSE_DOWN || event->type == TGUI_MOUSE_UP || event->type == TGUI_MOUSE_AXIS) {
		if (event->mouse.x >= widget->calculated_x && event->mouse.x < widget->calculated_x+widget->calculated_w && event->mouse.y >= widget->calculated_y && event->mouse.y < widget->calculated_y+widget->calculated_h) {
			return widget;
		}
	}
	else if (event->type != TGUI_UNKNOWN && widget == focus) {
		return widget;
	}

	return NULL;
}

void TGUI::handle_event(TGUI_Event *event, TGUI_Widget *widget)
{
	widget->handle_event(event);

	for (size_t i = 0; i < widget->children.size(); i++) {
		handle_event(event, widget->children[i]);
	}
}

bool TGUI::focus_something(TGUI_Widget *widget)
{
	if (widget->accepts_focus) {
		focus = widget;
		return true;
	}

	for (size_t i = 0; i < widget->children.size(); i++) {
		if (focus_something(widget->children[i])) {
			return true;
		}
	}

	return false;
}

// Returns positive for aligned match, negative for unaligned match
void TGUI::focus_distance(TGUI_Widget *start, TGUI_Widget *widget, int dir_x, int dir_y, int &score, int &grade)
{
	int cx = start->calculated_x + start->calculated_w / 2;
	int cy = start->calculated_y + start->calculated_h / 2;

	int box_x1 = cx;
	int box_x2 = cx + dir_x * 1000000;
	int box_y1 = cy;
	int box_y2 = cy + dir_y * 1000000;

	if (box_x1 == box_x2) {
		box_x1 = start->calculated_x;
		box_x2 = box_x1 + start->calculated_w;
	}
	else {
		box_y1 = start->calculated_y;
		box_y2 = box_y1 + start->calculated_h;
	}

	if (box_x2 < box_x1) {
		int tmp = box_x1;
		box_x1 = box_x2;
		box_x2 = tmp;
	}

	if (box_y2 < box_y1) {
		int tmp = box_y1;
		box_y1 = box_y2;
		box_y2 = tmp;
	}

	int widget_x1 = widget->calculated_x;
	int widget_x2 = widget_x1 + widget->calculated_w;
	int widget_y1 = widget->calculated_y;
	int widget_y2 = widget_y1 + widget->calculated_h;

	int widget_cx = (widget_x1 + widget_x2) / 2;
	int widget_cy = (widget_y1 + widget_y2) / 2;

	int dx = widget_cx - cx;
	int dy = widget_cy - cy;
	int dist = int(sqrtf(float(dx*dx + dy*dy)));

	if (!(widget_x1 > box_x2 || widget_x2 < box_x1 || widget_y1 > box_y2 || widget_y2 < box_y1)) {
		grade = 0;
	}
	else if (dir_x < 0 && widget_cx < cx || dir_x > 0 && widget_cx > cx || dir_y < 0 && widget_cy < cy || dir_y > 0 && widget_cy > cy) {
		grade = 1;
	}
	else {
		grade = 2;
	}

	score = dist;
}

void TGUI::find_focus(TGUI_Widget *start, TGUI_Widget *&current_best, TGUI_Widget *widget, int dir_x, int dir_y, int &best_score, int &best_grade)
{
	if (widget->accepts_focus && widget != start) {
		int score, grade;
		focus_distance(start, widget, dir_x, dir_y, score, grade);
		if (grade < best_grade) {
			best_score = score;
			best_grade = grade;
			current_best = widget;
		}
		else if (grade == best_grade && score < best_score) {
			best_score = score;
			best_grade = grade;
			current_best = widget;
		}
	}

	for (size_t i = 0; i < widget->children.size(); i++) {
		find_focus(start, current_best, widget->children[i], dir_x, dir_y, best_score, best_grade);
	}
}

TGUI_Widget::TGUI_Widget(int w, int h) :
	parent(NULL),
	percent_x(false),
	percent_y(false),
	w(w),
	h(h),
	padding_left(0),
	padding_right(0),
	padding_top(0),
	padding_bottom(0),
	float_right(false),
	accepts_focus(false)
{
}

TGUI_Widget::TGUI_Widget(float percent_w, float percent_h) :
	parent(NULL),
	percent_x(true),
	percent_y(true),
	percent_w(percent_w),
	percent_h(percent_h),
	padding_left(0),
	padding_right(0),
	padding_top(0),
	padding_bottom(0),
	float_right(false),
	accepts_focus(false)
{
}

TGUI_Widget::TGUI_Widget(int w, float percent_h) :
	parent(NULL),
	percent_x(false),
	percent_y(true),
	percent_h(percent_h),
	w(w),
	padding_left(0),
	padding_right(0),
	padding_top(0),
	padding_bottom(0),
	float_right(false),
	accepts_focus(false)
{
}

TGUI_Widget::TGUI_Widget(float percent_w, int h) :
	parent(NULL),
	percent_x(true),
	percent_y(false),
	percent_w(percent_w),
	h(h),
	padding_left(0),
	padding_right(0),
	padding_top(0),
	padding_bottom(0),
	float_right(false),
	accepts_focus(false)
{
}

void TGUI_Widget::set_parent(TGUI_Widget *widget)
{
	parent = widget;
	parent->children.push_back(this);
}

void TGUI_Widget::set_padding(int padding)
{
	padding_left = padding_right = padding_top = padding_bottom = padding;
}

void TGUI_Widget::set_padding(int left, int right, int top, int bottom)
{
	padding_left = left;
	padding_right = right;
	padding_top = top;
	padding_bottom = bottom;
}

void TGUI_Widget::set_float_right(bool float_right)
{
	this->float_right = float_right;
}

void TGUI_Widget::set_accepts_focus(bool accepts_focus)
{
	this->accepts_focus = accepts_focus;
	if (accepts_focus == false) {
		if (gui->get_focus() == this) {
			gui->focus_something();
		}
	}
}

TGUI_Widget *TGUI_Widget::get_parent()
{
	return parent;
}

int TGUI_Widget::get_x()
{
	return calculated_x;
}

int TGUI_Widget::get_y()
{
	return calculated_y;
}

int TGUI_Widget::get_width()
{
	return calculated_w;
}

int TGUI_Widget::get_height()
{
	return calculated_h;
}

int TGUI_Widget::get_padding_left()
{
	return padding_left;
}

int TGUI_Widget::get_padding_right()
{
	return padding_right;
}

int TGUI_Widget::get_padding_top()
{
	return padding_top;
}

int TGUI_Widget::get_padding_bottom()
{
	return padding_bottom;
}

int TGUI_Widget::get_right_pos()
{
	if (float_right == false) {
		return 0;
	}
	int parent_width;
	tgui_get_size(parent->parent, parent, &parent_width, NULL);
	parent_width += parent->padding_left + parent->padding_right;
	int width;
	tgui_get_size(parent, this, &width, NULL);
	width += padding_left + padding_right;
	int right = 0;
	for (size_t i = 0; i < parent->children.size(); i++) {
		TGUI_Widget *d = parent->children[i];
		if (d == this) {
			break;
		}
		if (d->float_right) {
			int w2;
			tgui_get_size(parent, d, &w2, NULL);
			w2 += d->padding_left + d->padding_right;
			right += w2;
		}
	}
	return parent_width - (right + width);
}

void tgui_get_size(TGUI_Widget *parent, TGUI_Widget *widget, int *width, int *height)
{
	if (parent == NULL) {
		*width = widget->gui->w;
		*height = widget->gui->h;
	}
	else {
		int w, h;
		tgui_get_size(parent->parent, parent, &w, &h);
		w += parent->padding_left + parent->padding_right;
		h += parent->padding_top + parent->padding_bottom;
		if (width) {
			if (widget->percent_x) {
				if (widget->percent_w < 0) {
					int total_w = 0;
					float total_percent = 0.0f;
					for (size_t i = 0; i < parent->children.size(); i++) {
						int this_w = 0;
						TGUI_Widget *d = parent->children[i];
						if (d->percent_x) {
							if (d->percent_w < 0) {
								total_percent += -d->percent_w;
							}
							else {
								int w2;
								tgui_get_size(parent, d, &w2, NULL);
								w2 += d->padding_left + d->padding_right;
								this_w = w2;
							}
						}
						else {
							this_w = d->w + d->padding_left + d->padding_right;
						}
						if (total_w + this_w > w) {
							total_w = 0;
						}
						if (d->float_right == false) {
							total_w += this_w;
						}
						if (d == widget) {
							break;
						}
					}
					int remainder = w - total_w;
					if (remainder > 0) {
						*width = remainder * int(-widget->percent_w / total_percent) - (widget->padding_left + widget->padding_right);
					}
					else {
						*width = 0;
					}
				}
				else {
					*width = int(w * widget->percent_w);
				}
			}
			else {
				*width = widget->w;
			}
		}
		if (height) {
			if (widget->percent_y) {
				if (widget->percent_h < 0) {
					int total_w = 0;
					int total_h = 0;
					float total_percent = 0.0f;
					int max_h = 0;
					float max_percent = 0.0f;
					for (size_t i = 0; i < parent->children.size(); i++) {
						int this_w = 0;
						int this_h = 0;
						float this_percent = 0.0f;
						TGUI_Widget *d = parent->children[i];
						tgui_get_size(parent, d, &this_w, NULL);
						this_w += d->padding_left + d->padding_right;
						if (d->percent_y) {
							if (d->percent_h < 0) {
								this_percent = -d->percent_h;
							}
							else {
								int h2;
								tgui_get_size(parent, d, NULL, &h2);
								h2 += d->padding_top + d->padding_bottom;
								this_h = h2;
							}
						}
						else {
							this_h = d->h + d->padding_top + d->padding_bottom;
						}
						if (total_w + this_w <= w) {
							if (this_h > max_h) {
								max_h = this_h;
							}
							if (this_percent > max_percent) {
								max_percent = this_percent;
							}
						}
						if (total_w + this_w >= w) {
							total_h += max_h;
							total_percent += max_percent;
							if (total_w + this_w > w) {
								max_h = this_h;
								max_percent = this_percent;
								total_w = this_w;
							}
							else {
								max_h = 0;
								max_percent = 0.0f;
								total_w = 0;
							}
						}
						else if (i == parent->children.size()-1) {
							total_percent += max_percent;
						}
						else if (d->float_right == false) {
							total_w += this_w;
						}
					}
					int remainder = h - total_h;
					if (remainder > 0) {
						*height = remainder * int(-widget->percent_h / total_percent) - (widget->padding_top + widget->padding_bottom);
					}
					else {
						*height = 0;
					}
				}
				else {
					*height = int(h * widget->percent_h);
				}
			}
			else {
				*height = widget->h;
			}
		}
	}
}

TGUI_Event tgui_get_relative_event(TGUI_Widget *widget, TGUI_Event *event)
{
	TGUI_Event new_event = *event;

	if (new_event.type == TGUI_MOUSE_DOWN || new_event.type == TGUI_MOUSE_UP || new_event.type == TGUI_MOUSE_AXIS) {
		new_event.mouse.x -= widget->get_x();
		new_event.mouse.y -= widget->get_y();
	}

	return new_event;
}

#ifdef WITH_SDL
TGUI_Event tgui_sdl_convert_event(SDL_Event *sdl_event)
{
	TGUI_Event event;

	switch (sdl_event->type) {
		case SDL_KEYDOWN:
			event.type = TGUI_KEY_DOWN;
			event.keyboard.code = sdl_event->key.keysym.sym;
			break;
		case SDL_KEYUP:
			event.type = TGUI_KEY_UP;
			event.keyboard.code = sdl_event->key.keysym.sym;
			break;
		case SDL_MOUSEBUTTONDOWN:
			event.type = TGUI_MOUSE_DOWN;
			event.mouse.button = sdl_event->button.button;
			event.mouse.x = sdl_event->button.x;
			event.mouse.y = sdl_event->button.y;
			break;
		case SDL_MOUSEBUTTONUP:
			event.type = TGUI_MOUSE_UP;
			event.mouse.button = sdl_event->button.button;
			event.mouse.x = sdl_event->button.x;
			event.mouse.y = sdl_event->button.y;
			break;
		case SDL_MOUSEMOTION:
			event.type = TGUI_MOUSE_AXIS;
			event.mouse.button = -1;
			event.mouse.x = sdl_event->motion.x;
			event.mouse.y = sdl_event->motion.y;
			break;
		case SDL_JOYBUTTONDOWN:
			event.type = TGUI_JOY_DOWN;
			event.joystick.button = sdl_event->jbutton.button;
			event.joystick.axis = -1;
			event.joystick.value = 0.0f;
			break;
		case SDL_JOYBUTTONUP:
			event.type = TGUI_JOY_UP;
			event.joystick.button = sdl_event->jbutton.button;
			event.joystick.axis = -1;
			event.joystick.value = 0.0f;
			break;
		case SDL_JOYAXISMOTION:
			event.type = TGUI_JOY_AXIS;
			event.joystick.button = -1;
			event.joystick.axis = sdl_event->jaxis.axis;
			event.joystick.value = float(sdl_event->jaxis.value + 32768) / 65535.0f * 2.0f - 1.0f;
			break;
		default:
			event.type = TGUI_UNKNOWN;
			break;
	}

#ifdef TGUI_DEBUG
	switch (event.type) {
		case TGUI_KEY_DOWN:
		case TGUI_KEY_UP:
			printf("[%2d] %d\n", event.type, event.keyboard.code);
			break;
		case TGUI_MOUSE_DOWN:
		case TGUI_MOUSE_UP:
		case TGUI_MOUSE_AXIS:
			printf("[%2d] %d %d,%d\n", event.type, event.mouse.button, event.mouse.x, event.mouse.y);
			break;
		case TGUI_JOY_DOWN:
		case TGUI_JOY_UP:
		case TGUI_JOY_AXIS:
			printf("[%2d] %d %d %f\n", event.type, event.joystick.button, event.joystick.axis, event.joystick.value);
			break;
	}
#endif

	return event;
}
#endif