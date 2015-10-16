
#include "stringcode.h"



int utf8ToUnicode(const char *s, int *bytes)
{
    if ((0x80 & s[0]) == 0)//一个字节的UTF-8
    {
        *bytes = 1;
        return s[0];
    }
    int a, b;
    if ((0xE0 & s[0]) == 0xC0)//两字节的UTF-8
    {
        *bytes = 2;
        a = ( 0x1F & s[0] ) << 6;
        b = 0x3F & s[1];
        return a + b;
    }
    int c;
    if ((0xF0 & s[0]) == 0xE0)//三字节的UTF-8
    {
        *bytes = 3;
        a = ( 0x0F & s[0] ) << 12;
        b = ( 0x3F & s[1] ) << 6;
        c = 0x3F & s[2];
        return a + b + c;
    }
    return 0;
}

int converToUnicode(const char* s, size_t sz, IntVector* v)
{
    size_t e = 0;
    const char* rs = s;
    int nd = 0;
    while (e < sz)
    {
        int c = utf8ToUnicode(rs, &nd);
        v->push_back(c);
        e += nd;
        rs = s + e;
    }
    return 0;
}
