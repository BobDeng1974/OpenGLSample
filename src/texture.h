#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <GL/gl.h>
#include <string.h>


// texture data
struct Texture
{
    GLubyte* data;  // ͼƬ������ 32 x 32��
	GLuint	bpp;    // ͼƬλ��
	GLuint	width;
	GLuint	height;
	GLuint	texID;  // gl ����ID
	GLuint	type;    // ͼƬ���� (GL_RGB, GL_RGBA)
};



unsigned int LoadGLTextures(const char* filename);
bool LoadTGA(Texture* , const char* );
bool ReadFileData(const char* filename, char* data, size_t &sz);

#pragma pack(push, 1)
struct BitmapFileHeader
{
    unsigned short type;          //   λͼ�ļ������ͣ�����ΪBM
    unsigned long size;           //   λͼ�ļ��Ĵ�С�����ֽ�Ϊ��λ
    unsigned short reserved1;     //   λͼ�ļ������֣�����Ϊ0
    unsigned short reserved2;     //   λͼ�ļ������֣�����Ϊ0
    unsigned long offbits;        //   λͼ���ݵ���ʼλ�ã��������λͼ
};

struct BitmapInfoHeader
{
    unsigned long size;          //   ���ṹ��ռ���ֽ���
    unsigned long width;         //   λͼ�Ŀ�ȣ�������Ϊ��λ
    unsigned long height;        //   λͼ�ĸ߶ȣ�������Ϊ��λ
    unsigned short planes;       //   Ŀ���豸�ļ��𣬱���Ϊ1
    unsigned short bitcount;     //   ÿ�����������λ����������1(˫ɫ),4(16ɫ)��8(256ɫ)��24(���ɫ)֮һ
    unsigned long compression;   //   λͼѹ�����ͣ�������   0(��ѹ��),1(BI_RLE8ѹ������)��2(BI_RLE4ѹ������)֮һ
    unsigned long sizeimage;     //   λͼ�Ĵ�С�����ֽ�Ϊ��λ
    unsigned long xpelspermeter; //   λͼˮƽ�ֱ��ʣ�ÿ��������
    unsigned long ypelspermeter; //   λͼ��ֱ�ֱ��ʣ�ÿ��������
    unsigned long clrused;       //   λͼʵ��ʹ�õ���ɫ���е���ɫ��
    unsigned long clrimportant;  //   λͼ��ʾ��������Ҫ����ɫ��
};

struct RGB { unsigned char red, green, blue; };
#pragma pack(pop)
// bmp �洢���ݵ�������bgr, opengl����ʹ����ɫ˳����rgb,������Ҫת��һ��
inline void SwapBGR2RGB(RGB& rgb)
{
    unsigned char t = rgb.red;
    rgb.red = rgb.blue;
    rgb.blue = t;
}

unsigned int LoadGLBitmap24(const char* filename);

#endif // __TEXTURE_H__
