#ifndef __GUI_UTILS_H
#define __GUI_UTILS_H

#include <page.hpp>
#include "gui-utils.h"

struct GuiElements 
{
    Paginator &paginator;
    M5EPD_Canvas &canvas;
    M5EPD_Canvas &top_bar;
    M5EPD_Canvas &selected_icon;
    M5EPD_Canvas &unselected_icon;
};

typedef std::function<int16_t(const char*)> widthCallbackFunc;
// void next_line();
// int line_height();
// bool vertical_overflow();
void setup_gui(GuiElements gui_elements); //M5EPD_Canvas *canvas, M5EPD_Canvas *top_bar, M5EPD_Canvas* selected_icon, M5EPD_Canvas* unselected_icon
void draw_selection_cursor(GuiElements gui_elements, int selected_choice);
char* select_file(GuiElements gui_elements, const char* title);
bool check_selection(GuiElements gui_elements, int &current_choice);
//char* wrap_one_line(M5EPD_Canvas &canvas,const char *block_c);
char* wrap_one_line(char *&block_c, int max_line_width, widthCallbackFunc width_callback);
const char* word_wrap(const char *line_c);
//bool cursor_inside_canvas();
//void gui_clear(M5EPD_Canvas &canvas);
void menu_bar_draw(M5EPD_Canvas &canvas, Paginator &paginator);
// int get_num_choices();
//void add_choice_option(const char* name);
// void add_choice_option(M5EPD_Canvas &canvas, const char* name,int x, int y);
//void set_indent(int indent);
void load_font(const char* fontFilename);

#endif