/**
整体要实现的东东:
0. 简单的OpenGL程序使用glfw来实现
1. 简单GUI
2. 简单骨骼动画ms3d, 《半条命》的骨骼动画
*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#pragma pack(push, 1)
struct BitmapFileHeader
{
    unsigned short type;          //   位图文件的类型，必须为BM
    unsigned long size;           //   位图文件的大小，以字节为单位
    unsigned short reserved1;     //   位图文件保留字，必须为0
    unsigned short reserved2;     //   位图文件保留字，必须为0
    unsigned long offbits;       //   位图数据的起始位置，以相对于位图
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

// 因为bmp图片的颜色顺序是b,g,r 所有要转化成r,g,b 交换r,b
inline void SwapBGR2RGB(RGB& rgb)
{
    unsigned char t = rgb.red;
    rgb.red = rgb.blue;
    rgb.blue = t;
}

unsigned int LoadBitmap24(const char* filename)
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

        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Linear Min Filter
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Linear Mag Filter
        glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &img->red);

        delete[] img;
    }

    fclose(fp);
    return tex;
}

GLchar* ReadShaderData1(const char* filename, GLsizei &sz)
{
    FILE *fn = fopen(filename, "rb");
    if (fn == NULL)
    {
        printf(" cant open file \n");
        return NULL;
    }
    fseek(fn, 0L, SEEK_END);
    sz = ftell(fn);
    fseek(fn, 0L, SEEK_SET);
    GLchar* data = (GLchar*)malloc(sz);
    sz = fread((void*)data, 1, sz, fn);
    fclose(fn);
    return data;
}

//----------------------------------------------------------------------------
//
GLuint  buildShader1(const char* filename, GLenum type)
{
    GLsizei sz = 0;
    GLint succ;
    GLchar* vsSource = ReadShaderData1(filename, sz);
    GLuint  vs = glCreateShader(type);
    glShaderSource(vs, 1, &vsSource, &sz);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &succ);
    if (succ == GL_FALSE)
    {
        GLchar err[256] = {0};
        GLsizei err_sz, len;
        glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &err_sz);
        glGetShaderInfoLog(vs, err_sz, &len, err);
        printf("%s\n", err);
    }

    return vs;
}

//----------------------------------------------------------------------------
//
GLuint buildProgram1(const char*vsfn, const char* frfn)
{
    GLint linked;
    GLuint program = glCreateProgram();
    GLuint vsshader = buildShader1(vsfn, GL_VERTEX_SHADER);
    GLuint frshader = buildShader1(frfn, GL_FRAGMENT_SHADER);
    glAttachShader(program, vsshader);
    glAttachShader(program, frshader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (linked == GL_FALSE)
    {
        GLchar err[256] = {0};
        GLsizei err_sz, len;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &err_sz);
        glGetProgramInfoLog(program, err_sz, &len, err);
        printf("%s\n", err);
    }
    glUseProgram(program);
    return program;
}

static void mouse_callback(GLFWwindow* window, int button, int action, int mods){}
static void cursorpos_callback(GLFWwindow *window, double x, double y){}
static void error_callback(int error, const char* description){fputs(description, stderr);}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}


int main(int argc, char *argv[])
{
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    glfwInit();
    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetCursorPosCallback(window, cursorpos_callback);

    //buildProgram("Data/shader.vert", "Data/shader.frag");
    GLuint tex = LoadBitmap24("Data/demo.bmp");
    glClearColor(0,0,0,1);      // 清理屏幕为黑色
    glEnable(GL_CULL_FACE);     // 启用面剔除
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);    // 启用深度测试

    glEnable(GL_LIGHT0);        // 启用灯光0
    glEnable(GL_NORMALIZE);     // 启用法线
    glEnable(GL_COLOR_MATERIAL);// 启用材质模式
    glEnable(GL_LIGHTING);      // 打开灯光

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER ,0.9);//0.5可以换成任何在0~1之间的数
    glShadeModel(GL_SMOOTH);

    // 设置灯光的颜色
    const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
    const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);  // 设置环境光颜色
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);  // 设置漫反射的颜色
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);  // 设置镜面反射的颜色
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);  // 设置灯的位置

    // 设置材质的颜色
    const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
    const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
    const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat high_shininess[] = { 100.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);   // 设置环境光颜色
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);   // 设置漫反射的颜色
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);  // 设置镜面反射的颜色
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);// 镜面指数 该值越小，表示材质越粗糙，点光源发射的光线照射到上面，也可以产生较大的亮点

    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float ratio = (float) width / (float) height;
        glViewport(0, 0, width, height);                // 把图像按照指定宽,高显示到屏幕上
        glMatrixMode(GL_PROJECTION);                    // 选择透视矩阵
        glLoadIdentity();                               // 重置矩阵
        glFrustum(-ratio, ratio, -1.0, 1.0, 2.0, 100.0);// 设置透视矩阵
        glMatrixMode(GL_MODELVIEW);                     // 选择模型矩阵
        glLoadIdentity();

        // render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glTranslatef(0.0f ,0.0f, -3.0f);

        const GLfloat vers[] = { 0.0f,0.0f,0.0f, 1.0f,0.0f,0.0f, 1.0f,1.0f,0.0f, 0.0f,1.0f,0.0f, };
        const GLfloat cors[] = { 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, };
        const GLfloat nors[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};
        const GLfloat uvs[] = {0.0f,0.0f, 1.0f,0.0f, 1.0f,1.0f, 0.0f,1.0f};
        const GLint indices[] = {  0,1,2, 0,2,3 };

        glBindTexture(GL_TEXTURE_2D, tex);
        glPushMatrix();
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, vers);

            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(3, GL_FLOAT, 0, cors);

            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT, 0, nors);

            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, 0, uvs);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);
        glPopMatrix();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}


