#ifndef PAGE_HPP 
#define PAGE_HPP
struct CopyBlock {
    char* text;
    int minY;
    int maxY;
    int minX;
    char selectionIndex;
};

class Page 
{
    private:
    static const int MAX_COPY=32; 
    CopyBlock copySections[MAX_COPY];
    //CopyBlock options[32];
    int copyLengths;
    int numCopySections;
    int numOptions;
    int currentY;
    public:
       
    void addCopy(char *copy, int minY, int maxY, int selectionIndex);
    CopyBlock getCopy(int index);
    //CopyBlock getOption(int index);
};


class Paginator 
{
    private:
    static const int MAX_PAGES = 32;
    Page *pages[MAX_PAGES];
    Page* getLastPage();
    Page* addPage();
    public:
    int numPages;
    int cursorX;
    int cursorY;
    int indent;
    const char* wordWrap(const char *line_c);
    void addCopy(char *copy);
    void addSelectableCopy(char *copy);
    void clear();
};

#endif