#ifndef __GUI_UTILS_H
#define __GUI_UTILS_H


void next_line();
void draw_selection_cursor();
char* select_file(const char* title);
bool check_selection();
char* wrap_one_line(const char *block_c);
char* wrap_one_line(const char *block_c, int max_line_width, int (* width_callback)(const char*));
const char* word_wrap(const char *line_c);
bool cursor_inside_canvas();
void setup_gui();
void gui_clear();
void gui_draw();
int get_num_choices();
int get_current_choice();
void add_choice_option(const char* name);
void add_choice_option(const char* name,int x, int y);
void clear_choices();
void set_indent(int indent);
void load_font(const char* fontFilename);
bool vertical_overflow();
#endif