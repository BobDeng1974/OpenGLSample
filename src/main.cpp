#include <GL/glut.h>

#include "texture.h"

// 顶点坐标
const GLfloat vers[] = {
    -0.5,-0.5,0.5, 0.5,-0.5,0.5, 0.5,0.5,0.5, -0.5,0.5,0.5, // 正面 4个顶点
    0.5,-0.5,0.5, -0.5,-0.5,0.5, -0.5,0.5,0.5, 0.5,0.5,0.5, // 背面 4个顶点

    -0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,-0.5, -0.5,0.5,-0.5, // 上面 4个顶点
    -0.5,0.5,-0.5,-0.5,0.5,-0.5, 0.5,0.5,0.5, -0.5,0.5,0.5, // 下面 4个顶点

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

// 设置灯光的颜色
const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

// 设置材质的颜色
const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

Texture texture;
GLfloat r = 0.0f;

int LoadGLTextures()
{
    int Status = 0;
    if (LoadTGA(&texture, "Data/ms.tga"))
    {
        glGenTextures(1, &texture.texID);
        glBindTexture(GL_TEXTURE_2D, texture.texID);
        glTexImage2D(GL_TEXTURE_2D, 0, texture.bpp / 8, texture.width, texture.height, 0, texture.type, GL_UNSIGNED_BYTE, texture.data);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        if (texture.data)
            free(texture.data);
    }
    return Status;
}
//----------------------------------------------------------------------------
//
static void init()
{
    //LoadGLTextures();
    glClearColor(0,0,0,1);      // 清理屏幕为黑色
    glEnable(GL_CULL_FACE);     // 启用面剔除
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);    // 启用深度测试
    /*
    glEnable(GL_LIGHT0);        // 启用灯光0
    glEnable(GL_NORMALIZE);     // 启用法线
    glEnable(GL_COLOR_MATERIAL);    // 启用材质模式
    glEnable(GL_LIGHTING);      // 打开灯光
    */
    //
    glEnable(GL_TEXTURE_2D);
    //glShadeModel(GL_SMOOTH);
    //glClearDepth(1.0f);
    //glDepthFunc(GL_LEQUAL);
    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    /*
    // 设置灯光
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);  // 设置环境光颜色
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);  // 设置漫反射的颜色
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);  // 设置镜面反射的颜色
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);  // 设置灯的位置

    // 设置材质
    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);   // 设置环境光颜色
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);   // 设置漫反射的颜色
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);   // 设置镜面反射的颜色
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);                   // 镜面指数 该值越小，表示材质越粗糙，点光源发射的光线照射到上面，也可以产生较大的亮点
    */
}

void test_draw()
{

    const GLfloat vers[] = {
         0.0f,0.0f,0.0f,
         25.0f,0.0f,0.0f,
         30.0f,30.0f,0.0f,
         0.0f,25.0f,0.0f,
    };
    const GLfloat cors[] = {
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
    };
    const GLint indices[] = {
        0,1,2,3
    };

    glPushMatrix();
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, vers);
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(3, GL_FLOAT, 0, cors);
        glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, indices);
    glPopMatrix();
}
//----------------------------------------------------------------------------
//
static void reshape(int width, int height)
{
    const float ar = (float) width / (float) height;
    glViewport(0, 0, width, height);                // 把图像按照指定宽,高显示到屏幕上

    glMatrixMode(GL_PROJECTION);                    // 选择透视矩阵
    glLoadIdentity();                               // 重置矩阵
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);      // 设置透视矩阵

    glMatrixMode(GL_MODELVIEW);                     // 选择模型矩阵
    glLoadIdentity();
}

//----------------------------------------------------------------------------
//
static void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0f ,0.0f, -3.0f);
    test_draw();
    r += 0.05f;
    glPushMatrix();
        //glRotatef(r, 1.0f, 0.0f, 0.0f);
        //glRotatef(r, 0.0f, 1.0f, 0.0f);

        //glBindTexture(GL_TEXTURE_2D, texture.texID);
        glColor3d(1,1,1);
        glBegin(GL_QUADS);										// Draw Our Quad
			//glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f,  1.0f, 0.0f);
			//glTexCoord2f(1.0f, 1.0f);
			glVertex3f( 1.0f,  1.0f, 0.0f);
			//glTexCoord2f(1.0f, 0.0f);
			glVertex3f( 1.0f, -1.0f, 0.0f);
			//glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, 0.0f);
		glEnd();


/*
        glEnableClientState(GL_NORMAL_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, vers);

        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, nors);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, texcoos);

        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(3, GL_FLOAT, 0, cors);

        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, indices);
*/
    glPopMatrix();
    glutSwapBuffers();      // 交换缓冲区
}

//----------------------------------------------------------------------------
//
static void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27 :
        case 'q':
            exit(0);
            break;
    }
    glutPostRedisplay();    // 进行重绘
}

//----------------------------------------------------------------------------
//
static void idle(void)
{
    glutPostRedisplay();
}

//----------------------------------------------------------------------------
//
static void mouse(int button, int state, int x, int y)
{

}

//----------------------------------------------------------------------------
//
static void timer(int dt)
{
    glutPostRedisplay();
    glutTimerFunc(20, timer, 0);
}

//----------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("GLUT");
    init();
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    glutMouseFunc(mouse);
    glutTimerFunc(20, timer, 0);
    glutMainLoop();
    return EXIT_SUCCESS;
}
/*
#include "gui.h"
#include <stdlib.h>
#include <stdio.h>

Simple::Gui* gui = NULL;
int g_width = 0;
int g_height = 0;
float g_wh = 0.0f;

// 设置灯光的颜色
const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

// 设置材质的颜色
const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

static void initUi()
{
    // create ui
    gui->addButton("btn1", 0, 0, 100, 30);
}

static void init()
{
    glClearColor(0,0,0,1);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
}

static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;
    gui->setView(width, height);

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);    // 透视变换
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

    glMatrixMode(GL_MODELVIEW);     // 模型变换
    glLoadIdentity();
}

void test_draw()
{
    glColor3d(1,0,0);
    const GLfloat vers[] = {
         0.0f,0.0f,0.0f,
         25.0f,0.0f,0.0f,
         30.0f,30.0f,0.0f,
         0.0f,25.0f,0.0f,
    };
    const GLfloat cors[] = {
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
    };
    const GLint indices[] = {
        0,1,2,3
    };

    glPushMatrix();
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, vers);
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(3, GL_FLOAT, 0, cors);
        glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, indices);
    glPopMatrix();
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glColor3d(1,0,0);

    gui->update();
    gui->render();
    // 3d


    glutSwapBuffers();
}

static void timer(int dt)
{
    glutPostRedisplay();
    glutTimerFunc(20, timer, 0);
}

static void mouse(int button, int state, int x, int y)
{
    gui->mouseEvent(button, state, x, y);
}

static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27 :
        case 'q':
            exit(0);
            break;
    }

    glutPostRedisplay();
}

static void idle(void)
{
    glutPostRedisplay();
}

int main(int argc, char *argv[])
{
    gui = Simple::Gui::create();

    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("GLUT Shapes");

    init();
    initUi();

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);
    glutMouseFunc(mouse);
    glutTimerFunc(20, timer, 0);

    glutMainLoop();

    Simple::Gui::destroy();

    return EXIT_SUCCESS;
}
*/
