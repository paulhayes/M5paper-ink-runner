#include "gui-utils.h"
#include <string.h>
#include <unity.h>
#include <Arduino.h>
//#include <cstring>
//#include <ostream>

int test_length(const char *str)
{
    return 10*strlen(str);
}



void test_wordwrap(char *paragraph,const char **lines,int expectedNumLines)
{    
    char *current_line = paragraph;
    int max_line_width = 200;
    int numLines=0;
    
    char buf[64];
    //sprintf(buf,"Argh %d %d",(uint32_t)paragraph,(uint32_t)current_line);  
    //TEST_ASSERT_EQUAL_INT_MESSAGE(3,4,buf);

    while(current_line!=NULL && current_line[0]!='\0'){
        char *last_line = current_line;
        current_line = wrap_one_line(last_line, max_line_width, test_length);
        sprintf(buf,"Incorrect line, line_index=%d length=%d offset=%d",numLines,strlen(last_line),(uint32_t)current_line-(uint32_t)paragraph);
        if(numLines<6){
            //TEST_ASSERT_EQUAL_STRING_MESSAGE(lines[numLines],last_line,buf);
            TEST_ASSERT_EQUAL_STRING_MESSAGE(lines[numLines],last_line,buf);
            
        }
        numLines++;
        if(numLines>32){
            break;
        }
    }


    TEST_ASSERT_EQUAL(expectedNumLines,numLines);

}

void test_wrap_one_line()
{
    char paragraph[] = "Mary had a little lamb it's fleece was white as snow. And everywhere that Mary went, the lamb was sure to go";
    const char *lines[] = {
        "Mary had a little",
        "lamb it's fleece was",
        "white as snow. And",
        "everywhere that Mary",
        "went, the lamb was",
        "sure to go"
    };
    test_wordwrap(paragraph,lines,6);
}

void test_wrap_newlines_line()
{
    char paragraph[] = "Mary had a little lamb it's fleece was white as snow.\nAnd everywhere that Mary went, the lamb was sure to go";
    const char *lines[] = {
        "Mary had a little",
        "lamb it's fleece was",
        "white as snow.", 
        "And everywhere that",
        "Mary went, the lamb",
        "was sure to go"
    };
    test_wordwrap(paragraph,lines,6);
}


void test_wrap_short()
{
    char paragraph[] = "Mary had a";
    const char *lines[] = {
        "Mary had a"        
    };
    test_wordwrap(paragraph,lines,1);
}

void test_wrap_empty()
{
    char paragraph[] = "\n";
    const char *lines[] = {
        "\n"        
    };
    test_wordwrap(paragraph,lines,1);
}

void test_wrap_short_withnewline()
{
    char paragraph[] = "Mary had a little lamb\nit's fleece was white";
    const char *lines[] = {
        "Mary had a little",
        "lamb",
        "it's fleece was",
        "white"        
    };
    test_wordwrap(paragraph,lines,4);
}

void test_wrap_shortish()
{
    char paragraph[] = "Mary had a little lamb";
    const char *lines[] = {
        "Mary had a little",
        "lamb"
    };
    test_wordwrap(paragraph,lines,2);
}

void trivialTest(void) {
    TEST_ASSERT_EQUAL(4, 3+1);
}

void setup()
{
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(trivialTest);
    RUN_TEST(test_wrap_one_line);
    RUN_TEST(test_wrap_short);
    RUN_TEST(test_wrap_newlines_line);
    RUN_TEST(test_wrap_empty);
    RUN_TEST(test_wrap_short_withnewline);
    RUN_TEST(test_wrap_shortish);
    UNITY_END();
}

void loop(){
    
}