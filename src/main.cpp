#include <GL/glut.h>

#include "gui.h"
#include <stdlib.h>
#include <stdio.h>

Simple::Gui* gui = NULL;
int g_width = 0;
int g_height = 0;
float g_wh = 0.0f;

// ���õƹ����ɫ
const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

// ���ò��ʵ���ɫ
const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

/** \brief  ����Ļ���ת������������
 *
 * \param x int
 * \param y int
 * \param wx GLdouble&
 * \param wy GLdouble&
 * \param wz GLdouble&
 * \return int
 *
 */
int screen2world(int x, int y, GLdouble &wx, GLdouble &wy, GLdouble &wz)
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];

    GLfloat winx, winy, winz;
    GLdouble mx, my, mz;
    glTranslated(0, 0, 2.1);

    glPushMatrix();
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
    glPopMatrix();

    winx = x;
    winy = (float)viewport[3] - (float)y;
    glReadPixels(winx, (int)winy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winz);
    gluUnProject(winx, winy, winz, modelview, projection, viewport, &mx, &my, &mz);
    wx = mx;
    wy = my;
    wz = mz;
    printf("==>x:%f,y:%f,z:%f\n", mx, my, mz);
    return 1;
}

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
    g_width = width;
    g_height = height;

    const float ar = (float) width / (float) height;

    g_wh = ar;

    printf("%d,%d\n", width, height);
    gui->setView(width, height);

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);    // ͸�ӱ任
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

    glMatrixMode(GL_MODELVIEW);     // ģ�ͱ任
    glLoadIdentity();
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

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glColor3d(1,0,0);

    // 2D
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float hw = g_width / 2;
    float hh = g_height / 2;

    glOrtho(-hw * g_wh,hw * g_wh,-hh,hh,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gui->update();
    gui->render();

    // 3d
    glMatrixMode(GL_PROJECTION);    // ͸�ӱ任
    glLoadIdentity();
    glFrustum(-g_wh, g_wh, -1.0, 1.0, 2.0, 100.0);
    glMatrixMode(GL_MODELVIEW);     // ģ�ͱ任
    glLoadIdentity();




    glutSwapBuffers();
}

static void timer(int dt)
{
    glutPostRedisplay();
    glutTimerFunc(20, timer, 0);
}

static void mouse(int button, int state, int x, int y)
{
    GLdouble wx, wy, wz;
    screen2world(x, y, wx, wy, wz);
    gui->mouseEvent(button, state, wx, wy);
    printf("%d,%d ==> %f, %f, %f\n", x, y, wx, wy, wz);
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
