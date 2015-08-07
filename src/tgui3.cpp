#include <cmath>

#include "tgui3.h"

int TGUI::focus_sloppiness = 2;

void TGUI::set_focus_sloppiness(int sloppiness)
{
	focus_sloppiness = sloppiness;
}

TGUI::TGUI(TGUI_Widget *main_widget, int w, int h) :
	main_widget(main_widget),
	w(w),
	h(h),
	focus(0),
	offset_x(0),
	offset_y(0)
{
	layout();
	focus_something();
}

TGUI::~TGUI()
{
	destroy(main_widget);
}

void TGUI::destroy(TGUI_Widget *widget)
{
	for (size_t i = 0; i < widget->children.size(); i++) {
		destroy(widget->children[i]);
	}

	delete widget;
}

void TGUI::layout()
{
	set_sizes(main_widget);
	set_positions(main_widget, offset_x+main_widget->get_padding_left(), offset_y+main_widget->get_padding_top());
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
	if (event->type == TGUI_JOY_AXIS && fabsf(event->joystick.value) > 0.25f) {
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
	else if (event->type == TGUI_MOUSE_DOWN) {
		TGUI_Widget *widget = get_event_owner(event);
		if (widget && widget->accepts_focus) {
			focus = widget;
		}
	}

	if (x == 0 && y == 0) {
		handle_event(event, main_widget);
	}
	else {
		TGUI_Widget *start = focus == 0 ? main_widget : focus;
		TGUI_Widget *best = start;
		int best_score = INT_MAX;
		int best_grade = 2;
		find_focus(start, best, main_widget, x, y, best_score, best_grade);
		if (focus_sloppiness > 0) {
			if (best_grade == 2) {
				int new_best_score = INT_MAX;
				int new_best_grade = 2;
				TGUI_Widget *new_best = start;
				if (x < 0) {
					x = 0;
					y = -1;
				}
				else if (x > 0) {
					x = 0;
					y = 1;
				}
				else if (y < 0) {
					x = 1;
					y = 0;
				}
				else {
					x = -1;
					y = 0;
				}
				find_focus(start, new_best, main_widget, x, y, new_best_score, best_grade);
				if (best_grade == 0) {
					best = new_best;
				}
			}
		}
		if (best_grade <= focus_sloppiness) {
			focus = best;
		}
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

TGUI_Widget *TGUI::get_main_widget()
{
	return main_widget;
}

TGUI_Widget *TGUI::get_focus()
{
	return focus;
}

TGUI_Widget *TGUI::get_event_owner(TGUI_Event *event)
{
	TGUI_Widget *widget = get_event_owner(event, main_widget);
#ifdef TGUI_DEBUG_XXX
	printf("event owner: %p\n", widget);
#endif
	return widget;
}

int TGUI::get_width()
{
	return w;
}

int TGUI::get_height()
{
	return h;
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

	parent_width = widget->calculated_w + widget->get_padding_left() + widget->get_padding_right();
	parent_height = widget->calculated_h + widget->get_padding_top() + widget->get_padding_bottom();

	int max_h = 0;
	int dx = 0;
	int dy = 0;

	for (size_t i = 0; i < widget->children.size(); i++) {
		TGUI_Widget *d = widget->children[i];

		int width = d->calculated_w + d->get_padding_left() + d->get_padding_right();
		int height = d->calculated_h + d->get_padding_top() + d->get_padding_bottom();

		if (dx + width > parent_width || d->break_line) {
			dx = 0;
			dy += max_h;
			max_h = 0;
		}

		int pos_x = d->get_right_pos();
		int pos_y = d->get_bottom_pos();

		if (d->float_right == false) {
			pos_x += dx;
		}
		if (d->float_bottom == false) {
			pos_y += dy;
		}

		pos_x += d->get_padding_left();
		pos_y += d->get_padding_top();

		set_positions(d, pos_x+x, pos_y+y);

		if (d->float_right == false && d->center_x == false) {
			dx += width;
		}

		if (d->float_bottom == false && d->center_y == false) {
			max_h = height > max_h ? height : max_h;
		}
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
		if (d != 0) {
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

	return 0;
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

	if (!((widget_x1 > box_x2) || (widget_x2 < box_x1) || (widget_y1 > box_y2) || (widget_y2 < box_y1))) {
		grade = 0;
	}
	else if (((dir_x < 0) && (widget_cx < cx)) || ((dir_x > 0) && (widget_cx > cx)) || ((dir_y < 0) && (widget_cy < cy)) || ((dir_y > 0) && (widget_cy > cy))) {
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
	parent(0),
	percent_x(false),
	percent_y(false),
	w(w),
	h(h),
	use_percent_padding_left(false),
	use_percent_padding_right(false),
	use_percent_padding_top(false),
	use_percent_padding_bottom(false),
	padding_left(0),
	padding_right(0),
	padding_top(0),
	padding_bottom(0),
	float_right(false),
	float_bottom(false),
	center_x(false),
	center_y(false),
	clear_float_x(false),
	clear_float_y(false),
	break_line(false),
	accepts_focus(false),
	calculated_x(-1),
	calculated_y(-1),
	calculated_w(-1),
	calculated_h(-1)
{
}

TGUI_Widget::TGUI_Widget(float percent_w, float percent_h) :
	parent(0),
	percent_x(true),
	percent_y(true),
	percent_w(percent_w),
	percent_h(percent_h),
	use_percent_padding_left(false),
	use_percent_padding_right(false),
	use_percent_padding_top(false),
	use_percent_padding_bottom(false),
	padding_left(0),
	padding_right(0),
	padding_top(0),
	padding_bottom(0),
	float_right(false),
	float_bottom(false),
	center_x(false),
	center_y(false),
	clear_float_x(false),
	clear_float_y(false),
	break_line(false),
	accepts_focus(false),
	calculated_x(-1),
	calculated_y(-1),
	calculated_w(-1),
	calculated_h(-1)
{
}

TGUI_Widget::TGUI_Widget(int w, float percent_h) :
	parent(0),
	percent_x(false),
	percent_y(true),
	percent_h(percent_h),
	w(w),
	use_percent_padding_left(false),
	use_percent_padding_right(false),
	use_percent_padding_top(false),
	use_percent_padding_bottom(false),
	padding_left(0),
	padding_right(0),
	padding_top(0),
	padding_bottom(0),
	float_right(false),
	float_bottom(false),
	center_x(false),
	center_y(false),
	clear_float_x(false),
	clear_float_y(false),
	break_line(false),
	accepts_focus(false),
	calculated_x(-1),
	calculated_y(-1),
	calculated_w(-1),
	calculated_h(-1)
{
}

TGUI_Widget::TGUI_Widget(float percent_w, int h) :
	parent(0),
	percent_x(true),
	percent_y(false),
	percent_w(percent_w),
	h(h),
	use_percent_padding_left(false),
	use_percent_padding_right(false),
	use_percent_padding_top(false),
	use_percent_padding_bottom(false),
	padding_left(0),
	padding_right(0),
	padding_top(0),
	padding_bottom(0),
	float_right(false),
	float_bottom(false),
	center_x(false),
	center_y(false),
	clear_float_x(false),
	clear_float_y(false),
	break_line(false),
	accepts_focus(false),
	calculated_x(-1),
	calculated_y(-1),
	calculated_w(-1),
	calculated_h(-1)
{
}

TGUI_Widget::~TGUI_Widget()
{
}

void TGUI_Widget::set_parent(TGUI_Widget *widget)
{
	parent = widget;
	parent->children.push_back(this);
}

void TGUI_Widget::set_padding_left(int padding)
{
	padding_left = padding;
	use_percent_padding_left = false;
}

void TGUI_Widget::set_padding_left(float percent_padding)
{
	percent_padding_left = percent_padding;
	use_percent_padding_left = true;
}

void TGUI_Widget::set_padding_right(int padding)
{
	padding_right = padding;
	use_percent_padding_right = false;
}

void TGUI_Widget::set_padding_right(float percent_padding)
{
	percent_padding_right = percent_padding;
	use_percent_padding_right = true;
}

void TGUI_Widget::set_padding_top(int padding)
{
	padding_top = padding;
	use_percent_padding_top = false;
}

void TGUI_Widget::set_padding_top(float percent_padding)
{
	percent_padding_top = percent_padding;
	use_percent_padding_top = true;
}

void TGUI_Widget::set_padding_bottom(int padding)
{
	padding_bottom = padding;
	use_percent_padding_bottom = false;
}

void TGUI_Widget::set_padding_bottom(float percent_padding)
{
	percent_padding_bottom = percent_padding;
	use_percent_padding_bottom = true;
}

void TGUI_Widget::set_padding(int padding)
{
	padding_left = padding_right = padding_top = padding_bottom = padding;
	use_percent_padding_left = use_percent_padding_right = use_percent_padding_top = use_percent_padding_bottom = false;
}

void TGUI_Widget::set_padding(float percent_padding)
{
	percent_padding_left = percent_padding_right = percent_padding_top = percent_padding_bottom = percent_padding;
	use_percent_padding_left = use_percent_padding_right = use_percent_padding_top = use_percent_padding_bottom = true;
}

void TGUI_Widget::set_float_right(bool float_right)
{
	this->float_right = float_right;
}

void TGUI_Widget::set_float_bottom(bool float_bottom)
{
	this->float_bottom = float_bottom;
}

void TGUI_Widget::set_center_x(bool center_x)
{
	this->center_x = center_x;
}

void TGUI_Widget::set_center_y(bool center_y)
{
	this->center_y = center_y;
}

void TGUI_Widget::set_clear_float_x(bool clear_float_x)
{
	this->clear_float_x = clear_float_x;
}

void TGUI_Widget::set_clear_float_y(bool clear_float_y)
{
	this->clear_float_y = clear_float_y;
}

void TGUI_Widget::set_break_line(bool break_line)
{
	this->break_line = break_line;
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
	if (calculated_w < 0) {
		return w;
	}
	return calculated_w;
}

int TGUI_Widget::get_height()
{
	if (calculated_h < 0) {
		return h;
	}
	return calculated_h;
}

int TGUI_Widget::get_padding_left()
{
	if (use_percent_padding_left) {
		int parent_width;
		tgui_get_size(parent, this, &parent_width, 0);
		return int(percent_padding_left * parent_width);
	}
	else {
		return padding_left;
	}
}

int TGUI_Widget::get_padding_right()
{
	if (use_percent_padding_right) {
		int parent_width;
		tgui_get_size(parent, this, &parent_width, 0);
		return int(percent_padding_right * parent_width);
	}
	else {
		return padding_right;
	}
}

int TGUI_Widget::get_padding_top()
{
	if (use_percent_padding_top) {
		int parent_height;
		tgui_get_size(parent, this, 0, &parent_height);
		return int(percent_padding_top * parent_height);
	}
	else {
		return padding_top;
	}
}

int TGUI_Widget::get_padding_bottom()
{
	if (use_percent_padding_bottom) {
		int parent_height;
		tgui_get_size(parent, this, 0, &parent_height);
		return int(percent_padding_bottom * parent_height);
	}
	else {
		return padding_bottom;
	}
}

int TGUI_Widget::get_right_pos()
{
	if (float_right == false && center_x == false) {
		return 0;
	}
	int parent_width;
	tgui_get_size(parent->parent, parent, &parent_width, 0);
	int width;
	tgui_get_size(parent, this, &width, 0);
	width += get_padding_left() + get_padding_right();
	int right = 0;
	int center_this = 0;
	int center_total[3] = { 0 }; // top middle bottom
	for (size_t i = 0; i < parent->children.size(); i++) {
		TGUI_Widget *d = parent->children[i];
		if (d->clear_float_x) {
			right = 0;
			if (d->center_y) {
				center_total[1] = 0;
				if (center_y) {
					center_this = 0;
				}
			}
			else if (d->float_bottom) {
				center_total[2] = 0;
				if (float_bottom) {
					center_this = 0;
				}
			}
			else {
				center_total[0] = 0;
				if (center_y == false && float_bottom == false) {
					center_this = 0;
				}
			}
		}
		if (d == this) {
			if (float_right) {
				break;
			}
			else {
				if (center_y) {
					center_this = center_total[1];
				}
				else if (float_bottom) {
					center_this = center_total[2];
				}
				else {
					center_this = center_total[0];
				}
			}
		}
		if (d->float_right) {
			int w2;
			tgui_get_size(parent, d, &w2, 0);
			w2 += d->get_padding_left() + d->get_padding_right();
			right += w2;
		}
		else if (d->center_x) {
			if (d == this || !((d->center_x && d->float_bottom) && (center_x && float_bottom))) {
				int w2;
				tgui_get_size(parent, d, &w2, 0);
				w2 += d->get_padding_left() + d->get_padding_right();
				if (d->center_y) {
					center_total[1] += w2;
				}
				else if (d->float_bottom) {
					center_total[2] += w2;
				}
				else if (!center_y) {
					center_total[0] += w2;
				}
			}
		}
	}

	if (center_x) {
		parent_width += parent->get_padding_left() + parent->get_padding_right();
		int parent_center = parent->get_padding_right() + (parent_width - parent->get_padding_left() - parent->get_padding_right()) / 2;
		int center_total_this;
		if (center_y) {
			center_total_this = center_total[1];
		}
		else if (float_bottom) {
			center_total_this = center_total[2];
		}
		else {
			center_total_this = center_total[0];
		}
		int widget_center = center_total_this / 2;
		int offset = parent->get_padding_left();
		return parent_width - parent_center - widget_center - offset + center_this;
	}

	return parent_width - (right + width);
}

int TGUI_Widget::get_bottom_pos()
{
	if (float_bottom == false && center_y == false) {
		return 0;
	}
	int parent_height;
	tgui_get_size(parent->parent, parent, 0, &parent_height);
	int height;
	tgui_get_size(parent, this, 0, &height);
	height += get_padding_top() + get_padding_bottom();
	if (center_y) {
		parent_height += parent->get_padding_top() + parent->get_padding_bottom();
		int parent_center = parent->get_padding_top() + (parent_height - parent->get_padding_top() - parent->get_padding_bottom()) / 2;
		int widget_center = height / 2;
		int offset = parent->get_padding_top();
		return parent_height - parent_center - widget_center - offset;
	}
	int bottom = 0;
	for (size_t i = 0; i < parent->children.size(); i++) {
		TGUI_Widget *d = parent->children[i];
		if (d->clear_float_y) {
			bottom = 0;
		}
		if (d == this) {
			break;
		}
		if (d->float_bottom) {
			int h2;
			tgui_get_size(parent, d, 0, &h2);
			h2 += d->get_padding_top() + d->get_padding_bottom();
			bottom += h2;
		}
	}

	return parent_height - (bottom + height);
}

void tgui_get_size(TGUI_Widget *parent, TGUI_Widget *widget, int *width, int *height)
{
	if (parent == 0) {
		if (width) {
			*width = widget->gui->w;
		}
		if (height) {
			*height = widget->gui->h;
		}
	}
	else {
		int w = parent->get_width();
		int h = parent->get_height();
		if (width) {
			if (widget->percent_x) {
				if (widget->percent_w < 0) {
					int total_w = 0;
					bool found_widget = false;
					for (size_t i = 0; i < parent->children.size(); i++) {
						TGUI_Widget *d = parent->children[i];
						if (d == widget) {
							found_widget = true;
						}
						int this_w = 0;
						if (d->percent_x == false || d->percent_w >= 0) {
							tgui_get_size(parent, d, &this_w, 0);
							this_w += d->get_padding_left() + d->get_padding_right();
						}
						if (d->float_right == false && d->center_x == false) {
							total_w += this_w;
						}
						if (total_w + this_w > w) {
							if (found_widget) {
								break;
							}
							total_w = 0;
						}
					}
					int remainder = w - total_w;
					if (remainder > 0) {
						*width = int(remainder * -widget->percent_w - (widget->get_padding_left() + widget->get_padding_right()));
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
					int max_h = 0;
					bool found_widget = false;
					for (size_t i = 0; i < parent->children.size(); i++) {
						TGUI_Widget *d = parent->children[i];
						if (d == widget) {
							found_widget = true;
						}
						int this_w;
						int this_h;
						tgui_get_size(parent, d, &this_w, &this_h);
						this_w += d->get_padding_left() + d->get_padding_right();
						this_h += d->get_padding_top() + d->get_padding_bottom();
						if (d->percent_y && d->percent_h < 0) {
							this_h = 0;
						}
						if (total_w + this_w > w) {
							max_h = this_h;
						}
						else if (this_h > max_h) {
							max_h = this_h;
						}
						if (d->float_right == false && d->center_x == false) {
							total_w += this_w;
						}
						if (total_w + this_w >= w) {
							if (found_widget) {
								break;
							}
							if (total_w + this_w > w) {
								total_w = this_w;
							}
							else {
								max_h = 0;
								total_w = 0;
							}
						}
					}
					int remainder = h - max_h;
					if (remainder > 0) {
						*height = int(remainder * -widget->percent_h - (widget->get_padding_top() + widget->get_padding_bottom()));
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
			event.mouse.x = (float)sdl_event->button.x;
			event.mouse.y = (float)sdl_event->button.y;
			break;
		case SDL_MOUSEBUTTONUP:
			event.type = TGUI_MOUSE_UP;
			event.mouse.button = sdl_event->button.button;
			event.mouse.x = (float)sdl_event->button.x;
			event.mouse.y = (float)sdl_event->button.y;
			break;
		case SDL_MOUSEMOTION:
			event.type = TGUI_MOUSE_AXIS;
			event.mouse.button = -1;
			event.mouse.x = (float)sdl_event->motion.x;
			event.mouse.y = (float)sdl_event->motion.y;
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

#ifdef TGUI_DEBUG_XXX
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
