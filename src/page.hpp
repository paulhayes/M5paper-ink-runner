class Page 
{
    public:
    static const int MAX_COPY=32;
    private:
    char *copySections[MAX_COPY];
    Option options[32];
    int copyLengths;
    int numCopySections;
    int numOptions;
    

    void addCopy(char *copy,int length);
    void addOption(char *option, int minY, int maxY);
    void getCopy(int index);
    void getOption(int index);
};

struct Option {
    char* text;
    int minY;
    int maxY;
};