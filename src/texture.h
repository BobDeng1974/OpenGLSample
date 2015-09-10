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

#endif // __TEXTURE_H__
