#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <GL/gl.h>
#include <string.h>


// texture data
struct Texture
{
    GLubyte* data;  // 图片内容是 32 x 32的
	GLuint	bpp;    // 图片位深
	GLuint	width;
	GLuint	height;
	GLuint	texID;  // gl 纹理ID
	GLuint	type;    // 图片类型 (GL_RGB, GL_RGBA)
};



unsigned int LoadGLTextures(const char* filename);
bool LoadTGA(Texture* , const char* );
bool ReadFileData(const char* filename, char* data, size_t &sz);

#pragma pack(push, 1)
struct BitmapFileHeader
{
    unsigned short type;          //   位图文件的类型，必须为BM
    unsigned long size;           //   位图文件的大小，以字节为单位
    unsigned short reserved1;     //   位图文件保留字，必须为0
    unsigned short reserved2;     //   位图文件保留字，必须为0
    unsigned long offbits;        //   位图数据的起始位置，以相对于位图
};

struct BitmapInfoHeader
{
    unsigned long size;          //   本结构所占用字节数
    unsigned long width;         //   位图的宽度，以像素为单位
    unsigned long height;        //   位图的高度，以像素为单位
    unsigned short planes;       //   目标设备的级别，必须为1
    unsigned short bitcount;     //   每个像素所需的位数，必须是1(双色),4(16色)，8(256色)或24(真彩色)之一
    unsigned long compression;   //   位图压缩类型，必须是   0(不压缩),1(BI_RLE8压缩类型)或2(BI_RLE4压缩类型)之一
    unsigned long sizeimage;     //   位图的大小，以字节为单位
    unsigned long xpelspermeter; //   位图水平分辨率，每米像素数
    unsigned long ypelspermeter; //   位图垂直分辨率，每米像素数
    unsigned long clrused;       //   位图实际使用的颜色表中的颜色数
    unsigned long clrimportant;  //   位图显示过程中重要的颜色数
};

struct RGB { unsigned char red, green, blue; };
#pragma pack(pop)
// bmp 存储数据的数序是bgr, opengl绘制使用颜色顺序是rgb,所有需要转换一下
inline void SwapBGR2RGB(RGB& rgb)
{
    unsigned char t = rgb.red;
    rgb.red = rgb.blue;
    rgb.blue = t;
}

unsigned int LoadGLBitmap24(const char* filename);

#endif // __TEXTURE_H__
