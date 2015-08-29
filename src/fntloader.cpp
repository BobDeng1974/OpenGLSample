#include "fntloader.h"

namespace fnt_space
{

FntFile::FntFile(){}
FntFile::~FntFile()
{
    for (vector<fnt_page_t*>::iterator i = pages.begin(); i != pages.end(); ++i)
        delete (*i);
    for (map<int, fnt_char_t*>::iterator i = id2char.begin(); i != id2char.end(); ++i)
        delete i->second;
    for (vector<fnt_kerning_t*>::iterator i = kernings.begin(); i != kernings.end(); ++i)
        delete (*i);
}

bool FntFile::loadFntFile(const char* filename)
{
    FILE* fp = fopen(filename, "r");
    if (!fp)
        return false;

    const int MAX_SZ = 512;
    char buff[MAX_SZ];
    while(!feof(fp))
    {
        if (fgets(buff, MAX_SZ, fp) != NULL)
        {
            if (memcmp(buff, "info", 4) == 0)
            {
                sscanf(buff, "info face=\"%[0-9a-zA-Z_.]\" size=%d bold=% italic=%d charset=\"%[0-9a-zA-Z_.]\" unicode=%d stretchH=%d smooth=%d aa=%d padding=%d,%d,%d,%d spacing=%d,%d outline=%d",
                       info.face, &info.size, &info.bold, &info.italic, info.charset,);
                //sscanf(buff, "page id=%d file=\"%[0-9a-zA-Z_.]\"", &info, s1);
            }
            else if (memcmp(buff, "common", 4) == 0)
            {

            }
            else if (memcmp(buff, "page", 4) == 0)
            {

            }
            else if (memcmp(buff, "chars", 5) == 0)
            {
                // TODO
            }
            else if (memcmp(buff, "char ", 5) == 0)
            {

            }
            else if (memcmp(buff, "kernings", 8) == 0)
            {

            }
            else if (memcmp(buff, "kerning ", 8) == 0)
            {

            }
        }
    }
}

void FntFile::dump()
{
}

fnt_char_t* FntFile::findFntChar(int id)
{
    map<int, fnt_char_t*>::iterator i = id2char.find(id);
    if (i != id2char.end())
        return i->second;
    return NULL;
}


};
