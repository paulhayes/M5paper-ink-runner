#include "page.hpp"
#include "gui-utils.h"
#include <cstring>

void Page::addCopy(char *copy, int minY, int maxY, int selectionIndex)
{

    
    copySections[numCopySections] = {copy};
    numCopySections++;
}

// void Page::addOption(char *option, int minY, int maxY)
// {
//     this->copySections[this->numCopySections] = {option,minY,maxY};
//     numCopySections++;
// }

CopyBlock Page::getCopy(int index)
{
    return this->copySections[index];
}

void Paginator::addCopy(char *copy)
{
    getLastPage();
}

const char* Paginator::wordWrap(const char *line_c){
    while(line_c!=NULL){
        line_c = one_line(line_c);
        // this->cursor_y+=canvas.fontHeight();
        // if(cursor_y>canvas.height()){
        //     break;
        // }
        
    }
    return line_c;
}

Page* Paginator::getLastPage()
{
    return pages[this->numPages-1];
}

Page* Paginator::addPage()
{
    if(numPages>this->MAX_PAGES){
        return nullptr;
    }
    pages[this->numPages] = new Page();
    this->numPages++;
    return this->getLastPage();
}


void Paginator::addSelectableCopy(char *copy)
{

}

void Paginator::clear()
{
    for(int i=0;i<numPages;i++){
        delete(this->pages[i]);
    }
    this->numPages=0;
    cursorX=0;
    cursorY=0;
}
