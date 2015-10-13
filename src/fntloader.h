#ifndef __FNT_LOADER_H__
#define __FNT_LOADER_H__

#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<map>
#include<vector>
#include<string.h>

using namespace std;

namespace fnt_space
{

struct fnt_info_t
{
    char face[32];
    char size;
    char bold;
    char italic;
    char charset[32];
    char unicode;
    short stretchH;
    char smooth;
    char aa;
    char padding[4];
    char spacing[2];
    char outline;
};

struct fnt_common_t
{
    char lineHeight;
    char base;
    unsigned short scaleW;
    unsigned short scaleH;
    char pages;
    char packed;
    char alphaChnl;
    char redChnl;
    char greenChnl;
    char blueChnl;
};

struct fnt_page_t
{
    char id;
    char file[32];
};

struct fnt_char_t
{
    int id;
    short x;
    short y;
    short width;
    short height;
    short xoffset;
    short yoffset;
    short xadvance;
    char page;
    short chnl;
};

struct fnt_kerning_t
{
    short first;
    short second;
    short amount;
};


class FntFile
{
private:
    fnt_info_t info;
    fnt_common_t common;
    vector<fnt_page_t*> pages;
    map<int, fnt_char_t*> id2char;
    vector<fnt_kerning_t*> kernings;

public:
    FntFile();
    ~FntFile();

    bool loadFntFile(const char* filename);
    fnt_char_t* findFntChar(int id);


    fnt_common_t getCommon() { return common; }
    void dump();
};


};

#endif // __FNT_LOADER_H__
