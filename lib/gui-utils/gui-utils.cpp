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


void draw_selection_cursor(Paginator &paginator,M5EPD_Canvas &selected_icon, M5EPD_Canvas &unselected_icon, int &selected_choice)
{

    int numChoices = paginator.currentPage().getNumSelectionAreas();
    for(int i=0;i<numChoices;i++){
        M5EPD_Canvas* icon = (i==selected_choice)? &selected_icon: &unselected_icon;
        SelectionArea choice = paginator.currentPage().getSelectionArea(i);
        icon->pushCanvas(choice.minX-icon->width(),choice.minY,UPDATE_MODE_DU4);
        Serial.print(choice.minX);
        Serial.print(",");
        Serial.print(choice.minY);
        Serial.print(" ");
        Serial.println(i==selected_choice);
    }
}

bool check_selection(Paginator &paginator, M5EPD_Canvas &canvas, M5EPD_Canvas &selected_icon, M5EPD_Canvas &unselected_icon, int &current_choice)
{
    int num_choice_pos = paginator.currentPage().getNumSelectionAreas();
        
    M5.TP.update();
    if(M5.TP.available() && !M5.TP.isFingerUp()){
        
        tp_finger_t finger = M5.TP.readFinger(0);
        // Serial.print("x=");
        // Serial.print(finger.x);
        // Serial.print("y=");
        // Serial.print(finger.y);
        // Serial.println("");
        
        for(int i=0;i<num_choice_pos;i++){
            SelectionArea selectionArea = paginator.currentPage().getSelectionArea(i);
            bool inY = finger.y>selectionArea.minY && finger.y<selectionArea.maxY;
            if(inY){
                current_choice=i;
                draw_selection_cursor(paginator,selected_icon,unselected_icon,current_choice);
                while(!M5.TP.isFingerUp()){
                    finger = M5.TP.readFinger(0);
                    delay(5);
                    M5.TP.update();
                }
                inY = finger.y>selectionArea.minY && finger.y<selectionArea.maxY;
                if(inY){
                    Serial.println("finger up inside of bounds");
                    return true;
                }
                Serial.println("finger up outside of bounds");
                return false;
            }
        }
    }
    if( M5.BtnL.wasPressed() ){
        Serial.println('-');
        current_choice--;
        if(current_choice<0){
            if(paginator.previousPage()){
                paginator.renderPage();
            }
            current_choice=-1;
            //current_choice=num_choice_pos-1;
        }
        draw_selection_cursor(paginator,selected_icon,unselected_icon,current_choice);
    }
    if(M5.BtnR.wasPressed()){
        Serial.println('+');
        current_choice++;
        if(current_choice>=num_choice_pos){
            current_choice=num_choice_pos;
            if(paginator.nextPage()){
                paginator.renderPage();
            }
            //current_choice=0;
        }
        draw_selection_cursor(paginator,selected_icon,unselected_icon,current_choice);
    }    
    if(M5.BtnP.wasPressed() && current_choice>=0){
        Serial.println("button down");
        return true;
    }
    return false;
}

char* select_file(M5EPD_Canvas &canvas, Paginator &paginator, const char* titlePaginator, M5EPD_Canvas &selected_icon, M5EPD_Canvas &unselected_icon)
{
    canvas.clear();
    char* title = (char*)malloc(strlen(titlePaginator)+1);
    strcpy(title,titlePaginator);
    paginator.addCopy(title);
    File root = SPIFFS.open("/");
    File file;
    char *files[10];
    int num_files=0;
    while(file=root.openNextFile()){
        const char* name=file.name();
        int len=strlen(name);
        if( strcmp(name+len-4,".bin")==0 ){
            Serial.print("file: ");
            Serial.println(name);
            char* fileName = files[num_files]=(char*)malloc(len+1);
            strcpy(fileName,name);
            int y=100+num_files*2*canvas.fontHeight();
            paginator.addChoice(num_files,name);
            num_files++;
            Serial.println("Next file");
        }
        file.close();
    }
    heap_caps_check_integrity_all(true);
    
    int selected_choice = 0;
    paginator.renderPage();
    
    draw_selection_cursor(paginator,selected_icon,unselected_icon, selected_choice);
    M5.update();

    while(!check_selection(paginator,canvas,selected_icon,unselected_icon, selected_choice)){
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

    Serial.print("wrapping first line of ");
    Serial.println(block_c);
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
    while(nextSpace!=NULL && (uintptr_t)nextSpace<(uintptr_t)endOfLine) {
        if(nextNewline!=NULL && (uintptr_t)nextNewline<(uintptr_t)nextSpace){
            nextSpace = nextNewline;
            replacedChar='\n';
            nextSpace[0]='\0';
            endOfLine=nextNewline;
            nextLine=nextNewline+1;
            break;
        }
        else {
            replacedChar=' ';
        }
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
                endOfLine=prevSpace+1;
                nextLine = endOfLine;
                break;
            }
        }
        else {
        }
        prevSpace = nextSpace;
        nextSpace = strchr(nextSpace+1,' ');        
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

void setup_gui(M5EPD_Canvas *canvas, M5EPD_Canvas* selected_icon, M5EPD_Canvas* unselected_icon)
{
    const int textSize=2;
    canvas->createCanvas(540, 960);
    canvas->setTextSize(textSize);
    canvas->setTextFont(2);
    int icon_w=canvas->textWidth(">");
    int icon_h=canvas->fontHeight();
    selected_icon->createCanvas(icon_w,icon_h);
    unselected_icon->createCanvas(icon_w,icon_h);
    canvas->setTextColor(15);
    canvas->drawString(">",0,0);
    copyRect(canvas,selected_icon,icon_w,icon_h);
    canvas->setTextColor(0);
    canvas->drawString(">",0,0);
    canvas->setTextColor(15);
    copyRect(canvas,unselected_icon,icon_w,icon_h);
}

void gui_draw(M5EPD_Canvas &canvas)
{
    canvas.pushCanvas(0, 0, UPDATE_MODE_DU);
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