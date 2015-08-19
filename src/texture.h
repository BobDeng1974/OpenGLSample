#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <GL/glut.h>

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


bool LoadTGA(Texture* , const char* );


#endif // __TEXTURE_H__
