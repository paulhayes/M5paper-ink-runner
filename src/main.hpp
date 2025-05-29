#ifndef MAIN_HPP 
#define MAIN_HPP

#include <M5EPD_Canvas.h>
#include <M5EPD.h>
#include "page.hpp"
#include "gui-utils.h"

void select_story();

M5EPD_Canvas canvas(&M5.EPD);
M5EPD_Canvas top_bar(&M5.EPD);
M5EPD_Canvas selected_icon(&M5.EPD);
M5EPD_Canvas unselected_icon(&M5.EPD);

char* story_filename;
Paginator paginator = Paginator(canvas);

GuiElements gui_elements = {paginator,canvas,top_bar,selected_icon,unselected_icon};


#endif