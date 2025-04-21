#include <page.hpp>
#ifndef __GUI_UTILS_H
#define __GUI_UTILS_H


// void next_line();
// int line_height();
// bool vertical_overflow();
void setup_gui(M5EPD_Canvas *canvas, M5EPD_Canvas* selected_icon, M5EPD_Canvas* unselected_icon);
void draw_selection_cursor();
char* select_file(M5EPD_Canvas &canvas, Paginator &paginator, const char* title);
bool check_selection(M5EPD_Canvas &canvas, Paginator &paginator);
//char* wrap_one_line(M5EPD_Canvas &canvas,const char *block_c);
char* wrap_one_line(M5EPD_Canvas &canvas,const char *block_c, int max_line_width, int16_t (* width_callback)(const char*));
const char* word_wrap(const char *line_c);
//bool cursor_inside_canvas();
void gui_clear(M5EPD_Canvas &canvas);
void gui_draw(M5EPD_Canvas &canvas);
// int get_num_choices();
int get_current_choice();
//void add_choice_option(const char* name);
// void add_choice_option(M5EPD_Canvas &canvas, const char* name,int x, int y);
void clear_choices();
void set_indent(int indent);
void load_font(const char* fontFilename);

#endif