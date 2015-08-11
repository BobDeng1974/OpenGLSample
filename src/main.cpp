#include <GL/glut.h>

#include "gui.h"
#include <stdlib.h>
#include <stdio.h>

Simple::Gui* gui = NULL;

static int slices = 16;
static int stacks = 16;

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

/** \brief  将屏幕左边转换成世界坐标
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

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    winx = x;
    winy = (float)viewport[3] - (float)y;
    glReadPixels(x, (int)winy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winz);
    gluUnProject(winx, winy, winz, modelview, projection, viewport, &wx, &wy, &wz);
    return 1;
}

/* GLUT callback Handlers */
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

    // create ui
    //gui = Simple::Gui::create();
    //gui->addButton("btn1", 0, 0, 100, 20);
    //Simple::pushRect(0, 0, 300, 200, Simple::Color(100, 100, 100));
}

static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;

    gui_set_view(width, height);

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);    // 透视变换
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

    glMatrixMode(GL_MODELVIEW);     // 模型变换
    glLoadIdentity() ;
}

void test_draw()
{
    const GLfloat vers[][3] = {
         0.0f,0.0f,0.0f,
         25.0f,0.0f,0.0f,
         30.0f,30.0f,0.0f,
         0.0f,25.0f,0.0f,
    };
    const GLfloat cors[][3] = {
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
    };
    const GLint indices[][4] = {
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
    glTranslated(0, 0, -6);

    test_draw();

    // TODO
    gui->update();
    gui->render();

    glutSwapBuffers();
}

static void timer(int dt)
{
    glutPostRedisplay();
    glutTimerFunc(20, timer, 0);
}

static void mouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
    {
        GLdouble wx, wy, wz;
        screen2world(x, y, wx, wy, wz);
        printf("%f, %f, %f\n", wx, wy, wz);
    }
}

static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27 :
        case 'q':
            exit(0);
            break;

        case '+':
            slices++;
            stacks++;
            break;

        case '-':
            if (slices>3 && stacks>3)
            {
                slices--;
                stacks--;
            }
            break;
    }

    glutPostRedisplay();
}

static void idle(void)
{
    glutPostRedisplay();
}


/* Program entry point */

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("GLUT Shapes");

    init();

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
