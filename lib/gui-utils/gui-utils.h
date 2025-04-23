#include <page.hpp>
#ifndef __GUI_UTILS_H
#define __GUI_UTILS_H

typedef std::function<int16_t(const char*)> widthCallbackFunc;
// void next_line();
// int line_height();
// bool vertical_overflow();
void setup_gui(M5EPD_Canvas *canvas, M5EPD_Canvas* selected_icon, M5EPD_Canvas* unselected_icon);
void draw_selection_cursor(Paginator &paginator,M5EPD_Canvas &selected_icon, M5EPD_Canvas &unselected_icon);
char* select_file(M5EPD_Canvas &canvas, Paginator &paginator, const char* title,M5EPD_Canvas &selected_icon, M5EPD_Canvas &unselected_icon);
bool check_selection(Paginator &paginator, M5EPD_Canvas &canvas, M5EPD_Canvas &selected_icon, M5EPD_Canvas &unselected_icon);
//char* wrap_one_line(M5EPD_Canvas &canvas,const char *block_c);
char* wrap_one_line(const char *block_c, int max_line_width, widthCallbackFunc width_callback);
const char* word_wrap(const char *line_c);
//bool cursor_inside_canvas();
//void gui_clear(M5EPD_Canvas &canvas);
void gui_draw(M5EPD_Canvas &canvas);
// int get_num_choices();
int get_current_choice();
//void add_choice_option(const char* name);
// void add_choice_option(M5EPD_Canvas &canvas, const char* name,int x, int y);
void clear_choices();
//void set_indent(int indent);
void load_font(const char* fontFilename);

#endif