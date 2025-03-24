#ifndef __GUI_UTILS_H
#define __GUI_UTILS_H


void next_line();
void draw_selection_cursor();
char* select_file();
bool check_selection();
void word_wrap(const char *line_c);
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
void set_font(const char* fontFilename);

#endif