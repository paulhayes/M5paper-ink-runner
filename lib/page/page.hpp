#include <M5EPD_Canvas.h>
#ifndef PAGE_HPP 
#define PAGE_HPP
struct CopyBlock {
    char* text;
    int x;
    int y;
};

struct SelectionArea {
    int choiceIndex;
    int minX;
    int maxX;
    int minY;
    int maxY;   
};

class Page 
{
    private:
    static const int MAX_COPY=32; 
    CopyBlock copySections[MAX_COPY];
    SelectionArea choices[MAX_COPY];
    //CopyBlock options[32];
    int copyLengths;
    int numCopySections;
    int numSelectionAreas;
    int currentY;
    public:
    ~Page();
    void addLine(char *copy, int x, int y);
    void printCopy();
    SelectionArea& getSelectionArea(int i);
    int getNumSelectionAreas();
    void addSelectionArea(int choiceIndex, int minX, int maxX, int minY, int maxY);    
    CopyBlock getCopy(int index);
    void render(M5EPD_Canvas canvas);
    
    //CopyBlock getOption(int index);
};


class Paginator 
{
    private:
    static const int MAX_PAGES = 32;
    Page *pages[MAX_PAGES];
    M5EPD_Canvas &m_canvas;
    
    Page* getLastPage();
    Page* addPage();
    void wordWrap(const char *line_c);
    public:
    int numPages;
    int cursorX;
    int cursorY;
    int currentPageIndex = 0;
    int padding = 5;
    int indent = padding;
    Paginator(M5EPD_Canvas &canvas):m_canvas(canvas)
    {
    }
    Page &currentPage();
    void addCopy(const char *copy);
    void addChoice(int choiceIndex, const char *copy);
    void addLineBreak();
    void clear();
    bool hasChoices();
    void renderPage();
    bool nextPage();
    bool previousPage();
};

#endif