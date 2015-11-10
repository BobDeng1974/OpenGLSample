#include "texture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tag
struct TgaHeader
{
    GLubyte Header[12];
};

struct TgaImage
{
    GLubyte		header[6];
	GLuint		bytes;
	GLuint		sz;
	GLuint		temp;
	GLuint		type;
	GLuint		height;
	GLuint		width;
	GLuint		bpp;
};

TgaHeader tgaheader;
TgaImage tga;

//----------------------------------------------------------------------------
//
void out_error(const char* msg)
{
    printf("%s\n", msg);
}

GLubyte uTGAcompare[12] = {0,0,2, 0,0,0,0,0,0,0,0,0};
GLubyte cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};


bool LoadUncompressedTGA(Texture *, FILE *);
bool LoadCompressedTGA(Texture *, FILE *);

//----------------------------------------------------------------------------
//
unsigned int LoadGLTextures(const char* filename)
{
    Texture texture;
    if (LoadTGA(&texture, filename))
    {
        glGenTextures(1, &texture.texID);
        glBindTexture(GL_TEXTURE_2D, texture.texID);
        glTexImage2D(GL_TEXTURE_2D, 0, texture.bpp / 8, texture.width, texture.height, 0, texture.type, GL_UNSIGNED_BYTE, texture.data);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        if (texture.data)
            free(texture.data);
    }
    return texture.texID;
}

//----------------------------------------------------------------------------
//
bool LoadTGA(Texture* texture, const char* filename)
{
    bool ret = false;
    FILE *fn = fopen(filename, "rb");
    if (fn == NULL)
    {
        out_error("Could not open texture file");
        return false;
    }
    if(fread(&tgaheader, sizeof(TgaHeader), 1, fn) == 0)
    {
        out_error("Could not read file header");
        fclose(fn);
        return false;
    }
    if(memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)
    {
        ret = LoadUncompressedTGA(texture, fn);
    }
    else if (memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)
    {
        ret = LoadCompressedTGA(texture, fn);
    }
    else
    {
        out_error("TGA file be type 2 or type 10");
    }
    fclose(fn);
    return ret;
}

//----------------------------------------------------------------------------
//
bool LoadUncompressedTGA(Texture * texture, FILE * fn)
{
	if(fread(tga.header, sizeof(tga.header), 1, fn) == 0)
	{
	    out_error("Could not read info header");
        return false;
	}

	texture->width  = tga.header[1] * 256 + tga.header[0];
	texture->height = tga.header[3] * 256 + tga.header[2];
	texture->bpp	= tga.header[4];
	tga.width		= texture->width;
	tga.height		= texture->height;
	tga.bpp			= texture->bpp;

	if((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp !=32)))
	{
		out_error("Invalid texture information");
        return false;
	}

	if(texture->bpp == 24)
		texture->type	= GL_RGB;
	else
		texture->type	= GL_RGBA;

	tga.bytes	= (tga.bpp / 8);
	tga.sz		        = (tga.bytes * tga.width * tga.height);
	texture->data	    = (GLubyte *)malloc(tga.sz);

	if(texture->data == NULL)
	{
		out_error("Could not allocate memory for image");
		return false;
	}

	if(fread(texture->data, 1, tga.sz, fn) != tga.sz)
	{
		out_error("Could not read image data");
		if(texture->data != NULL)
		{
			free(texture->data);
		}
		return false;
	}
    // Byte Swapping Optimized By Steve Thomas
	for(GLuint cswap = 0; cswap < tga.sz; cswap += tga.bytes)
	{
		texture->data[cswap] ^= texture->data[cswap+2] ^=
		texture->data[cswap] ^= texture->data[cswap+2];
	}

	return true;
}

//----------------------------------------------------------------------------
//
bool LoadCompressedTGA(Texture * texture, FILE * fn)
{
	if(fread(tga.header, sizeof(tga.header), 1, fn) == 0)
	{
		out_error("Could not read info header");
		return false;
	}

	texture->width  = tga.header[1] * 256 + tga.header[0];
	texture->height = tga.header[3] * 256 + tga.header[2];
	texture->bpp	= tga.header[4];
	tga.width		= texture->width;
	tga.height		= texture->height;
	tga.bpp			= texture->bpp;

	if((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp !=32)))
	{
		out_error("Invalid texture information");
		return false;
	}

	if(texture->bpp == 24)
		texture->type	= GL_RGB;
	else
		texture->type	= GL_RGBA;

	tga.bytes	= (tga.bpp / 8);
	tga.sz	= (tga.bytes * tga.width * tga.height);
	texture->data	= (GLubyte *)malloc(tga.sz);

	if(texture->data == NULL)
	{
		out_error("Could not allocate memory for image");
		return false;
	}

	GLuint pixelcount	= tga.height * tga.width;
	GLuint currentpixel	= 0;
	GLuint currentbyte	= 0;
	GLubyte * colorbuffer = (GLubyte *)malloc(tga.bytes);

	do
	{
		GLubyte chunkheader = 0;
		if(fread(&chunkheader, sizeof(GLubyte), 1, fn) == 0)
		{
			out_error("Could not read RLE header");
			if(texture->data != NULL)
				free(texture->data);
			return false;
		}

		if(chunkheader < 128)
		{
			chunkheader++;
			for(short counter = 0; counter < chunkheader; counter++)
			{
				if(fread(colorbuffer, 1, tga.bytes, fn) != tga.bytes)
				{
					out_error("Could not read image data");
					if(colorbuffer != NULL)
						free(colorbuffer);

					if(texture->data != NULL)
						free(texture->data);

					return false;
				}
				texture->data[currentbyte		] = colorbuffer[2];
				texture->data[currentbyte + 1	] = colorbuffer[1];
				texture->data[currentbyte + 2	] = colorbuffer[0];

				if(tga.bytes == 4)
					texture->data[currentbyte + 3] = colorbuffer[3];

				currentbyte += tga.bytes;
				currentpixel++;

				if(currentpixel > pixelcount)
				{
					out_error("Too many pixels read");
					if(colorbuffer != NULL)
						free(colorbuffer);

					if(texture->data != NULL)
						free(texture->data);

					return false;
				}
			}
		}
		else
		{
			chunkheader -= 127;
			if(fread(colorbuffer, 1, tga.bytes, fn) != tga.bytes)
			{
				out_error("Could not read from file");
				if(colorbuffer != NULL)
					free(colorbuffer);

				if(texture->data != NULL)
					free(texture->data);

				return false;
			}

			for(short counter = 0; counter < chunkheader; counter++)
			{
				texture->data[currentbyte		] = colorbuffer[2];
				texture->data[currentbyte + 1	] = colorbuffer[1];
				texture->data[currentbyte + 2	] = colorbuffer[0];

				if(tga.bytes == 4)
					texture->data[currentbyte + 3] = colorbuffer[3];

				currentbyte += tga.bytes;
				currentpixel++;

				if(currentpixel > pixelcount)
				{
					out_error("Too many pixels read");
					if(colorbuffer != NULL)
						free(colorbuffer);

					if(texture->data != NULL)
						free(texture->data);
					return false;
				}
			}
		}
	}

	while(currentpixel < pixelcount);
	return true;
}

bool ReadFileData(const char* filename, char* data, size_t &sz)
{
    bool ret = false;
    FILE *fn = fopen(filename, "rb");
    if (fn == NULL)
    {
        out_error(" cant open file");
        return ret;
    }
    sz = fread(data, 1024, 1, fn);
    fclose(fn);
    return ret;
}

unsigned int LoadGLBitmap24(const char* filename)
{
    unsigned int tex = 0;
    FILE* fp = fopen(filename, "rb");
    if (!fp)
        return 0;

    BitmapFileHeader fileheader;
    BitmapInfoHeader infohead;

    fread(&fileheader, sizeof(BitmapFileHeader), 1, fp);
    fread(&infohead, sizeof(BitmapInfoHeader), 1, fp);

    int width = infohead.width;
    int height = infohead.height;
    int bitcount = infohead.bitcount;

    if (bitcount == 24)
    {
        int num = width * height;
        fseek(fp, fileheader.offbits, SEEK_SET) ;

        RGB* img = new RGB[num];
        fread(img, sizeof(RGB) * num, 1, fp);

        for (int i = 0; i < num; ++i)
            SwapBGR2RGB(img[i]);

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Linear Min Filter
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Linear Mag Filter
        glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &img->red);

        delete[] img;
    }

    fclose(fp);
    return tex;
}

