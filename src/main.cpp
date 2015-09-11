
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

#include "texture.h"
#include "gui.h"
#include "fntloader.h"

using namespace Simple;

//=========================================================
double xpos = 0, ypos = 0;

// 顶点坐标
const GLfloat vers[] = {
    -0.5,-0.5,0.5, 0.5,-0.5,0.5, 0.5,0.5,0.5, -0.5,0.5,0.5, // 正面 4个顶点
    0.5,-0.5,-0.5, -0.5,-0.5,-0.5, -0.5,0.5,-0.5, 0.5,0.5,-0.5, // 背面 4个顶点

    -0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,-0.5, -0.5,0.5,-0.5, // 上面 4个顶点
    -0.5,-0.5,-0.5, 0.5,-0.5,-0.5, 0.5,-0.5,0.5, -0.5,-0.5,0.5, // 下面 4个顶点

    -0.5,-0.5,-0.5, -0.5,-0.5,0.5, -0.5,0.5,0.5, -0.5,0.5,-0.5, // 左面 4个顶点
    0.5,-0.5,0.5, 0.5,-0.5,-0.5, 0.5,0.5,-0.5, 0.5,0.5,0.5, // 右面 4个顶点
};

// 法线
const GLfloat nors[] = {
    0,0,1, 0,0,1, 0,0,1, 0,0,1,
    0,0,-1, 0,0,-1, 0,0,-1, 0,0,-1,
    0,1,0, 0,1,0, 0,1,0, 0,1,0,
    0,-1,0, 0,-1,0, 0,-1,0, 0,-1,0,
    -1,0,0, -1,0,0, -1,0,0, -1,0,0,
};

// 纹理坐标
const GLfloat texcoos[] = {
    0,0, 1,0, 1,1, 0,1,
    0,0, 1,0, 1,1, 0,1,
    0,0, 1,0, 1,1, 0,1,
    0,0, 1,0, 1,1, 0,1,
    0,0, 1,0, 1,1, 0,1,
    0,0, 1,0, 1,1, 0,1,
};

// 顶点颜色
const GLfloat cors[] = {
    1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,
    1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,
    1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,
    1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,
    1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,
    1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,
};

// 顶点索引
const GLint indices[] = {
    0,1,2, 0,2,3,
    4,5,6, 4,6,7,

    8,9,10, 8,10,11,
    12,13,14, 12,14,15,

    16,17,18, 16,18,19,
    20,21,22, 20,22,23,
};

//----------------------------------------------------------------------------
//
GLchar* ReadShaderData(const char* filename, GLsizei &sz)
{
    FILE *fn = fopen(filename, "rb");
    if (!fn)
        return NULL;
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
GLuint  buildShader(const char* filename, GLenum type)
{
    GLsizei sz = 0;
    GLint succ;
    const GLchar* vsSource = ReadShaderData(filename, sz);
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
GLuint buildProgram(const char*vsfn, const char* frfn)
{
    GLint linked;
    GLuint program = glCreateProgram();
    GLuint vsshader = buildShader(vsfn, GL_VERTEX_SHADER);
    GLuint frshader = buildShader(frfn, GL_FRAGMENT_SHADER);
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


//----------------------------------------------------------------------------
//
static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

//----------------------------------------------------------------------------
//
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

//----------------------------------------------------------------------------
//
static void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {

    }
}

//----------------------------------------------------------------------------
//
static void cursorpos_callback(GLFWwindow *window, double x, double y)
{
    xpos = x;
    ypos = y;
}

//----------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    create_graphic();
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    glfwInit();
    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    //glfwWindowHint(GLFW_SAMPLES, 0);    // 0x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // 设置OPENGL版本3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetCursorPosCallback(window, cursorpos_callback);

    // 读取UI纹理和fnt纹理
    GLuint fntText = LoadGLTextures("Data/fnt_arial.tga");
    FntFile fntFile;
    fntFile.loadFntFile("Data/fnt_arial.fnt");


    GLuint tex1 = LoadGLTextures("Data/demo.tga");
    GLuint program = buildProgram("Data/normalmap.vert", "Data/normalmap.frag"); // 使用shader, 把r<->b交换颜色
    GLint t1 = glGetUniformLocation(program, "tex");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glUniform1i(t1, 0);;//对应纹理第一层

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
    //glClearDepth(1.0f);
    //glDepthFunc(GL_LEQUAL);
    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // UI
        gp->render();
        // Model


        // 两个球体,主要是材质变换


        // 两个有无发现贴图对比


        gluLookAt(0.0f,0.0f,-3.0f, 0.0f,0.0f,0.0f, 0.0f,1.0f,0.0);

        //float r  = 50 * (float)glfwGetTime();
        glPushMatrix();
            glRotatef(45, 1.0f, 0.0f, 0.0f);
            glRotatef(45, 0.0f, 1.0f, 0.0f);

            glBindTexture(GL_TEXTURE_2D, tex1);
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, vers);

            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT, 0, nors);

            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, 0, texcoos);

            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(3, GL_FLOAT, 0, cors);

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, indices);

        glPopMatrix();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    destroy_graphic();

    return EXIT_SUCCESS;
}


#include "fntloader.h"
#include "ms3dloader.h"

int main2(int argc, char *argv[])
{
#if 0

    using namespace ms3d_space;
    ms3d_model_t* model = create_ms3d_model();
    load_ms3d_file(model, "Data/tin_open_lay.ms3d");
    dump_ms3d_file(model, "Data/tin_open_lay.log");
    delete_ms3d_model(model);
#endif // 0

#if 1
    using namespace fnt_space;
    FntFile file;
    //file.dump();
    file.loadFntFile("Data/fnt_arial.fnt");
    int k = 0;
#endif // 1

}

