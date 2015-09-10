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

#endif // __TEXTURE_H__
