#include <M5EPD.h>
#include "gui-utils.h"


// int padding = 5;
// int current_indent = padding;
// int cursor_x = padding;
// int cursor_y = 0;

// void next_line()
// {
//     cursor_y += canvas.fontHeight();
//     cursor_x = current_indent;
// }

// bool vertical_overflow()
// {
//     return cursor_y>canvas.height();
// }

// int line_height()
// {
//     return canvas.fontHeight();
// }


void draw_selection_cursor(GuiElements &gui_elements, int selected_choice)
{
    
    int numChoices = gui_elements.paginator.currentPage().getNumSelectionAreas();
    SelectionArea *previous_choice = NULL;
    SelectionArea *current_choice = NULL;
    if(selected_choice==gui_elements.cursorSelctionIndex){
        return;
    }
    for(int i=0;i<numChoices;i++){
        // M5EPD_Canvas* icon = (i==selected_choice)? &gui_elements.selected_icon: &gui_elements.unselected_icon;
        SelectionArea *choice = &gui_elements.paginator.currentPage().getSelectionArea(i);
        if(i==gui_elements.cursorSelctionIndex){
            previous_choice=choice;
        }
        if(i==selected_choice){
            current_choice=choice;
        }
        // Serial.print("Choice ");
        // Serial.print(i);
        // Serial.print(": ");
        // Serial.print(choice->minX);
        // Serial.print(",");
        // Serial.print(choice->minY);
        // Serial.print(" ");
        // Serial.print(choice->maxX);
        // Serial.print(",");
        // Serial.print(choice->maxY);
        // Serial.print(" ");
        // Serial.print(i==selected_choice?"SELECTED ":" ");
        // Serial.println(i==gui_elements.cursorSelctionIndex?"PREVIOUS":"");
    }

    if(current_choice!=NULL){
        //Serial.println("Drawing cursor");
        gui_elements.selected_icon.pushCanvas(current_choice->minX-gui_elements.selected_icon.width(),current_choice->minY+gui_elements.top_bar.height(),UPDATE_MODE_DU4);
    }
    if(previous_choice!=NULL){
        //Serial.println("Removing old cursor");
        gui_elements.unselected_icon.pushCanvas(previous_choice->minX-gui_elements.unselected_icon.width(),previous_choice->minY+gui_elements.top_bar.height(),UPDATE_MODE_DU4);
    }

    gui_elements.cursorSelctionIndex = selected_choice;
    // Serial.print(selected_choice);
    // Serial.print(" ");
    // Serial.println(gui_elements.cursorSelctionIndex);


}

bool check_selection(GuiElements &gui_elements, int &current_choice)
{
    int num_choices = gui_elements.paginator.currentPage().getNumSelectionAreas();
    bool choice_made = false;
    bool redraw_selection = false;
    M5.TP.update();
    if(M5.TP.available() && M5.TP.getFingerNum()>0 ){
        tp_finger_t finger = M5.TP.readFinger(0);
        bool fingerUp = M5.TP.isFingerUp();
        // Serial.print(M5.TP.getFingerNum());
        // Serial.print(" x=");
        // Serial.print(finger.x);
        // Serial.print("y=");
        // Serial.print(finger.y);
        // Serial.print(" ");
        // Serial.print(fingerUp);
        // Serial.println("");
        
        for(int i=0;i<num_choices;i++){
            SelectionArea selectionArea = gui_elements.paginator.currentPage().getSelectionArea(i);
            selectionArea.minY+=gui_elements.top_bar.height();
            selectionArea.maxY+=gui_elements.top_bar.height();
            bool inY = finger.y>selectionArea.minY && finger.y<selectionArea.maxY;
            if(fingerUp && inY && current_choice==i){
                Serial.printf("E%d\n",i);
                choice_made = true;
                redraw_selection=true;
                M5.TP.flush();
            }
            else if(inY && current_choice!=i){
                Serial.printf("S%d\n",i);
                current_choice=i;
                redraw_selection=true;
                // inY = finger.y>selectionArea.minY && finger.y<selectionArea.maxY;
                // if(inY){
                //     Serial.println("finger up inside of bounds");
                //     choice_made = true;
                // }
                // Serial.println("finger up outside of bounds");
            }
        }
        
    }
    else if( M5.BtnL.wasPressed() ){
        Serial.println("up");
        M5.TP.flush();
        redraw_selection=true;
        current_choice--;
        if(current_choice<0){
            current_choice=-1;
            if(gui_elements.paginator.previousPage()){
                gui_elements.paginator.renderPage();
                num_choices = gui_elements.paginator.currentPage().getNumSelectionAreas();
                current_choice=num_choices-1;
            }
        }
//        draw_selection_cursor(paginator,selected_icon,unselected_icon,current_choice);
    }
    else if(M5.BtnR.wasPressed()){
        M5.TP.flush();
        Serial.println("down");
        redraw_selection=true;
        current_choice++;
        if(current_choice>=num_choices){
            current_choice=num_choices-1;
            if(gui_elements.paginator.nextPage()){
                gui_elements.paginator.renderPage();
                num_choices = gui_elements.paginator.currentPage().getNumSelectionAreas();
                current_choice=0;
            }
        }
    }    
    else if( M5.BtnP.wasPressed() ){
        Serial.println("button down");
        choice_made=true;
    }
    if(redraw_selection){
        menu_bar_draw(gui_elements);
        draw_selection_cursor(gui_elements,current_choice);        
    }
    return choice_made;
}

char* select_file(GuiElements gui_elements, const char* titlePaginator)
{
    gui_elements.canvas.clear();
    char* title = (char*)malloc(strlen(titlePaginator)+1);
    strcpy(title,titlePaginator);
    gui_elements.paginator.addCopy(title);
    File root = SD.open("/");
    File file;
    char *files[10];
    int num_files=0;
    while(file=root.openNextFile()){
        const char* name=file.name();
        int len=strlen(name);
        if( strcmp(name+len-4,".bin")==0 ){
            // Serial.print("file: ");
            // Serial.println(name);
            char* fileName = files[num_files]=(char*)malloc(len+1);
            strcpy(fileName,name);
            int y=100+num_files*2*gui_elements.canvas.fontHeight();
            gui_elements.paginator.addChoice(num_files,name);
            num_files++;
            // Serial.println("Next file");
        }
        file.close();
    }
    //heap_caps_check_integrity_all(true);
    
    int selected_choice = 0;
    gui_elements.paginator.renderPage();
    
    draw_selection_cursor(gui_elements, selected_choice);
    M5.update();

    while(!check_selection(gui_elements,selected_choice)){
        M5.update();
        delay(10);
    }
    root.close();
    Serial.print("Option ");
    Serial.print(selected_choice);
    Serial.print(" ");
    Serial.print(files[selected_choice]);
    Serial.println(" selected");
    const char* filename = files[selected_choice];
    int filename_len = strlen(filename);
    char* story_filename = (char*)malloc(filename_len+2);
    story_filename[0]='/';
    strcpy(story_filename+1,filename);

    for(int i=num_files-1;i>=num_files;i--){
        free((void*)files[i]);
    }

    heap_caps_check_integrity_all(true);

    return story_filename;
}

const char *strnchr(const char* ptr, int ch, size_t size) {
    
    while (size > 0) {
      size--;
      if (ptr[size] == (char) ch) {
        return ptr+size;
      } 
      if (ptr[size] == '\0') {
        break;
      } 
    }
    return NULL;
  }



char* wrap_one_line(char *&ref_current_line, int max_line_width, widthCallbackFunc width_callback)
{
    // char* block_c = (char*)malloc(strlen(ref_current_line+1));
    // strcpy(block_c,ref_current_line);
    char* block_c = ref_current_line;

    // Serial.print("wrapping first line of ");
    // Serial.println(block_c);
    char *endOfLine=block_c+strlen(block_c);
    char* nextNewline = strchr(block_c, '\n');
    char *nextSpace = strchr(block_c,' ');
    char *insertedNull = NULL;
    char *nextLine;
    int lineLength=0;
    char replacedChar=' ';
    int width=0;

    char *prevSpace = NULL;
    if(nextSpace==NULL){
        nextLine = endOfLine;
    }
    while(nextSpace!=NULL && (uintptr_t)nextSpace<=(uintptr_t)endOfLine) {

        if(nextNewline!=NULL && (uintptr_t)nextNewline<(uintptr_t)nextSpace){
            nextSpace = nextNewline;
            endOfLine=nextNewline;
            nextLine=nextNewline+1;
        }
        
        replacedChar=nextSpace[0];
        nextSpace[0]='\0';
        insertedNull=nextSpace;
        width = width_callback(block_c);
        nextSpace[0]=replacedChar;
        if(width>max_line_width){
            if(prevSpace==NULL){
                break;   
            }
            else {
                prevSpace[0]='\0';
                nextLine = prevSpace+1;
                break;
            }
        }
        else if(replacedChar=='\n'){
            nextLine=nextSpace+1;
            nextSpace[0]='\0';
            break;
        }
        if(nextSpace==endOfLine){
            nextLine=nextSpace;
            break;
        }
        prevSpace = nextSpace;
        nextSpace = strchr(nextSpace+1,' '); 
        if(nextSpace==NULL){
            nextSpace=endOfLine;
        }
    }
    int len = strlen(block_c);
    ref_current_line = (char*)malloc(len+1);
    strcpy(ref_current_line,block_c);
    //free(block_c);
    return nextLine;
}

// const char* word_wrap_old(const char *line_c)
// {
//     //Serial.println("word_wrap start");
//     int len = strlen(line_c);
//     char *all = (char *)malloc(len + 1);
//     const char *current = all;
//     char *line = (char*)malloc(len+1);
//     strcpy(all, line_c);
//     const char *remaining = current;
//     int i = 0;
//     do
//     {
//         strcpy(line,current);
//         remaining = current + strlen(current);
//         int search_len = strlen(current);
//         if(cursor_y>(canvas.height()-canvas.fontHeight())){
//             return remaining;
//         }
//         while ((canvas.width() - (padding + cursor_x)) < canvas.textWidth(line))
//         {
//             //Serial.print("Finding last space in :");
//             //Serial.println(current);
//             const char *last = strnchr(current, ' ', search_len);
//             if (last == NULL)
//             {
//                 last = current + search_len - 1;
//                 //Serial.println("substract character");
//             }
//             else {
//                 //Serial.println("substract word");
//             }            
//             search_len = last-current;
//             //strncpy(line,current,search_len);
//             line[search_len]='\0';
//             remaining = last + 1;
//             // Serial.print("Testing:");
//             // Serial.println(line);
//         }
        
//         if (current != NULL && strlen(current) > 0)
//         {
//             // Serial.print("At ");
//             // Serial.print(cursorX);
//             // Serial.print(",");
//             // Serial.print(cursorY);
//             // Serial.print(" ");
//             // Serial.println(line);
//             canvas.drawString(line, cursor_x, cursor_y);
//         }
//         next_line();
//         current = remaining;
//         // Serial.print("S ");
//         // Serial.print((int)remaining);
//         // Serial.print(" ");
//         // Serial.print((int)(all + len));
//         // Serial.println(" ");
        
//         //Serial.println("next line");
//     } while (remaining < all + len);
//     free(all);
//     return NULL;
//     //Serial.println("word_wrap end");
// }

// bool cursor_inside_canvas()
// {
//     return cursor_y<(canvas.height()-canvas.fontHeight());
// }

void copyRect(M5EPD_Canvas *src, M5EPD_Canvas *dst, int w, int h)
{
    for(int x=0;x<w;x++){
        for(int y=0;y<h;y++){
            dst->drawPixel( x,y,src->readPixel(x,y) );
        }
    }
}


void setup_gui(GuiElements gui_elements)
{
    //M5EPD_Canvas *canvas, M5EPD_Canvas *top_bar, M5EPD_Canvas* selected_icon, M5EPD_Canvas* unselected_icon
    const int top_bar_height = 32;
    const int textSize=2;
    gui_elements.canvas.createCanvas(540, 960-top_bar_height);
    gui_elements.canvas.setTextSize(textSize);
    gui_elements.canvas.setTextFont(2);
    int icon_w=gui_elements.canvas.textWidth(">");
    int icon_h=gui_elements.canvas.fontHeight();
    gui_elements.selected_icon.createCanvas(icon_w,icon_h);
    gui_elements.unselected_icon.createCanvas(icon_w,icon_h);
    gui_elements.canvas.setTextColor(15);
    gui_elements.canvas.drawString(">",0,0);
    copyRect(&gui_elements.canvas,&gui_elements.selected_icon,icon_w,icon_h);
    gui_elements.canvas.setTextColor(0);
    gui_elements.canvas.drawString(">",0,0);
    gui_elements.canvas.setTextColor(15);
    copyRect(&gui_elements.canvas,&gui_elements.unselected_icon,icon_w,icon_h);

    gui_elements.top_bar.createCanvas(540,top_bar_height);
    gui_elements.top_bar.setTextFont(1);

    gui_elements.paginator.canvasOffsetY = gui_elements.top_bar.height();
}

void draw_battery(M5EPD_Canvas &canvas,uint32_t battery,int x, int y)
{
    uint16_t color = 0x0000;
    uint16_t bgcolor = 0xffff;
    int h = 18;
    int w = h*2;
    int p=2;
    int battery_top = 6;

    canvas.fillRect(x-4,y+h/2-battery_top/2,4,battery_top,color);
    canvas.fillRect(x,y,w,h,color);
    canvas.fillRect(x+p,y+p,w*battery/100-2*p,h-2*p,bgcolor);
}

void menu_bar_draw(GuiElements gui_elements)
{
    int32_t batteryPercent = 100*min(M5.getBatteryVoltage(),(uint32_t)4200)/4200;
    char title[] =  "inkeink reader";

    TopBarState state = {
        batteryPercent,
        gui_elements.paginator.currentPageIndex+1
    };
    state.setTitle(title);

    Serial.printf("Battery Voltage:%d\n",M5.getBatteryVoltage());

    if(state==gui_elements.top_bar_state){
        return;
    }

    uint16_t color = 0x0000;
    uint16_t bgcolor = 0xffff;
    gui_elements.top_bar.clear();
    gui_elements.top_bar.setTextFont(2);
    gui_elements.top_bar.fillCanvas(bgcolor);
    gui_elements.top_bar.drawLine(0,30,gui_elements.top_bar.width(),30,color);

    draw_battery(gui_elements.top_bar,batteryPercent,120,8);

    gui_elements.top_bar.setTextColor(color,bgcolor);
    gui_elements.top_bar.drawString("inkeink reader",5,5);
    char pageInfo[30];
    sprintf(pageInfo,"%d/%d",gui_elements.paginator.currentPageIndex+1,gui_elements.paginator.numPages);
    //int width = canvas.textWidth(pageInfo);
    gui_elements.top_bar.setTextDatum(TR_DATUM);
    gui_elements.top_bar.drawString(pageInfo,gui_elements.canvas.width()-5,5);
    gui_elements.top_bar.setTextDatum(TL_DATUM);
    gui_elements.top_bar.pushCanvas(0, 0, UPDATE_MODE_DU);
    
}

// void add_choice_option(const char* name)
// {
//     // choice_positions[i] = cursor_y;
//     // word_wrap(text);
//     add_choice_option(name,cursor_x,cursor_y);
// }

// void add_choice_option(M5EPD_Canvas &canvas, const char* text,int x, int y)
// {
//     choice_positions[num_choice_pos]=y;
//     //canvas.drawString(name,100,y);
//     cursor_x=x;
//     cursor_y=y;
//     canvas.write()
//     num_choice_pos++;
// }


// void set_indent(int indent)
// {
//     current_indent = indent;
// }

void load_font(M5EPD_Canvas *canvas, const char* fontFilename)
{
    canvas->loadFont(fontFilename,SPIFFS);
    canvas->setTextColor(15);
    canvas->createRender(3);
    canvas->createRender(32,256);
}