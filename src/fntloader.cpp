#include "fntloader.h"

namespace fnt_space
{

FntFile::FntFile() {}
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
                sscanf(buff, "info face=\"%[0-9a-zA-Z_.]\" size=%d bold=%d italic=%d charset=%[0-9a-zA-Z_.\"] \
                       unicode=%d stretchH=%d smooth=%d aa=%d padding=%d,%d,%d,%d spacing=%d,%d outline=%d",
                       info.face, &info.size, &info.bold, &info.italic, info.charset,
                       &info.unicode, &info.stretchH, &info.smooth, &info.aa,
                       &info.padding[0], &info.padding[1], &info.padding[2], &info.padding[3],
                       &info.spacing[0], &info.spacing[1], &info.outline);
            }
            else if (memcmp(buff, "common", 4) == 0)
            {
                sscanf(buff , "common lineHeight=%d base=%d scaleW=%d scaleH=%d pages=%d packed=%d alphaChnl=%d redChnl=%d greenChnl=%d blueChnl=%d",
                       &common.lineHeight, &common.base, &common.scaleW,
                       &common.scaleH, &common.pages,
                       &common.packed, &common.alphaChnl, &common.redChnl,
                       &common.greenChnl, &common.blueChnl);
            }
            else if (memcmp(buff, "page", 4) == 0)
            {
                fnt_page_t* t = new fnt_page_t;
                sscanf(buff , "page id=0 file=\"%[0-9a-zA-Z_.]\"", &t->id, t->file);
                pages.push_back(t);
            }
            else if (memcmp(buff, "chars", 5) == 0)
            {
                // TODO
            }
            else if (memcmp(buff, "char ", 5) == 0)
            {
                fnt_char_t* t = new fnt_char_t;
                sscanf(buff ,"char id=%d   x=%d   y=%d    width=%d     height=%d     xoffset=%d    yoffset=%d    xadvance=%d     page=%d  chnl=%d",
                       &t->id, &t->x, &t->y, &t->width, &t->height, &t->xoffset, &t->yoffset, &t->xadvance, &t->page, &t->chnl);
                id2char.insert(std::make_pair<int,fnt_char_t*>(t->id, t));
            }
            else if (memcmp(buff, "kernings", 8) == 0)
            {
                // TODO
            }
            else if (memcmp(buff, "kerning ", 8) == 0)
            {
                fnt_kerning_t* t = new fnt_kerning_t;
                sscanf(buff ,"kerning first=%d  second=%f  amount=%f", &t->first, &t->second, &t->amount);
                kernings.push_back(t);
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
