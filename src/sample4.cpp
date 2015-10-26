/**
整体要实现的东东:
简单的OpenGL程序使用glfw来实现

简单GUI的实现原理


*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "objread.h"

//---------------------------------------------------------
// 全局变量
double g_mouse_sx = 0, g_mouse_sy = 0; // 鼠标按下的位置
double g_roll_x = 0, g_roll_y = 0; // 鼠标拖动的偏移

bool g_is_press = false;    // 鼠标是否点击


static void error_callback(int error, const char* description){fputs(description, stderr); fputs("\n", stderr);}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

static void cursorpos_callback(GLFWwindow* win, double x, double y)
{
    if (g_is_press)
    {
        double xpos, ypos;
        glfwGetCursorPos(win, &xpos, &ypos);
        g_roll_y = g_mouse_sy - ypos;
        g_roll_x = xpos - g_mouse_sx;
    }
}

static void mousebutton_callback(GLFWwindow* win, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            g_is_press = true;
            glfwGetCursorPos(win, &g_mouse_sx, &g_mouse_sy);
        }
        else if (action == GLFW_RELEASE)
        {
            double xpos, ypos;
            glfwGetCursorPos(win, &xpos, &ypos);
            g_roll_y = g_mouse_sy - ypos;
            g_roll_x = xpos - g_mouse_sx;
            g_is_press = false;
        }
    }
}

void draw_button(float x, float y, float w, float h)
{

}

int main(int argc, char *argv[])
{
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
    glfwSetCursorPosCallback(window, cursorpos_callback);
    glfwSetMouseButtonCallback(window, mousebutton_callback);


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
    const GLfloat light_position[] = { 0.0f, 3.0f, -5.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);  // 设置环境光颜色
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);  // 设置漫反射的颜色
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);  // 设置镜面反射的颜色
    //glLightfv(GL_LIGHT0, GL_POSITION, light_position);  // 设置灯的位置

    // 设置材质的颜色
    const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
    const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
    const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat high_shininess[] = { 100.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);   // 设置环境光颜色
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);   // 设置漫反射的颜色
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);  // 设置镜面反射的颜色
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);// 镜面指数 该值越小，表示材质越粗糙，点光源发射的光线照射到上面，也可以产生较大的亮点

    obj_data_t* t = obj_create();
    obj_read("Data/panel.obj","Data/panel.mtl",t);
    obj_render_t* rt = obj_create_render(t);

    float rx = 0.0f;
    float ry = 0.0f;

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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清理颜色和深度缓存
        //glTranslatef(0.0f ,0.0f, -3.0f);
        gluLookAt(0.0f,0.0f,-3.0f, 0.0f,0.0f,1.0f, 0.0f,1.0f,0.0);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);

        //======================================================================

        obj_render(rt);
        //======================================================================
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    obj_destory_data(t);
    obj_destory_render(rt);


    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}


