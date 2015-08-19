#include <GL/glew.h>
#include <GL/glut.h>

#include "texture.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ��������
const GLfloat vers[] = {
    -0.5,-0.5,0.5, 0.5,-0.5,0.5, 0.5,0.5,0.5, -0.5,0.5,0.5, // ���� 4������
    0.5,-0.5,0.5, -0.5,-0.5,0.5, -0.5,0.5,0.5, 0.5,0.5,0.5, // ���� 4������

    -0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,-0.5, -0.5,0.5,-0.5, // ���� 4������
    -0.5,0.5,-0.5,-0.5,0.5,-0.5, 0.5,0.5,0.5, -0.5,0.5,0.5, // ���� 4������

    -0.5,-0.5,-0.5, -0.5,-0.5,0.5, -0.5,0.5,0.5, -0.5,0.5,-0.5, // ���� 4������
    0.5,-0.5,0.5, 0.5,-0.5,-0.5, 0.5,0.5,-0.5, 0.5,0.5,0.5, // ���� 4������
};

// ����
const GLfloat nors[] = {
    0,0,1, 0,0,1, 0,0,1, 0,0,1,
    0,0,-1, 0,0,-1, 0,0,-1, 0,0,-1,
    0,1,0, 0,1,0, 0,1,0, 0,1,0,
    0,-1,0, 0,-1,0, 0,-1,0, 0,-1,0,
    -1,0,0, -1,0,0, -1,0,0, -1,0,0,
};

// ��������
const GLfloat texcoos[] = {
    0,0, 1,0, 1,1, 0,1,
    0,0, 1,0, 1,1, 0,1,
    0,0, 1,0, 1,1, 0,1,
    0,0, 1,0, 1,1, 0,1,
    0,0, 1,0, 1,1, 0,1,
    0,0, 1,0, 1,1, 0,1,
};

// ������ɫ
const GLfloat cors[] = {
    1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,
    1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,
    1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,
    1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,
    1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,
    1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,
};

// ��������
const GLint indices[] = {
    0,1,2, 0,2,3,
    4,5,6, 4,6,7,
    8,9,10, 8,10,11,
    12,13,14, 12,14,15,

    16,17,18, 16,18,19,
    20,21,22, 20,22,23,
};

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

GLchar* ReadShaderData(const char* filename, GLsizei &sz)
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

GLuint  buildShader(const char* filename, GLenum type)
{
    GLsizei sz = 0;
    GLint succ;
    GLchar* vsSource = ReadShaderData(filename, sz);
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
static void init()
{
    LoadGLTextures();
    glClearColor(0,0,0,1);      // ������ĻΪ��ɫ
    glEnable(GL_CULL_FACE);     // �������޳�
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);    // ������Ȳ���

    glEnable(GL_LIGHT0);        // ���õƹ�0
    glEnable(GL_NORMALIZE);     // ���÷���
    glEnable(GL_COLOR_MATERIAL);// ���ò���ģʽ
    glEnable(GL_LIGHTING);      // �򿪵ƹ�

    //
    glEnable(GL_TEXTURE_2D);
    //glShadeModel(GL_SMOOTH);
    //glClearDepth(1.0f);
    //glDepthFunc(GL_LEQUAL);
    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // ���õƹ�
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);  // ���û�������ɫ
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);  // �������������ɫ
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);  // ���þ��淴�����ɫ
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);  // ���õƵ�λ��

    // ���ò���
    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);   // ���û�������ɫ
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);   // �������������ɫ
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);  // ���þ��淴�����ɫ
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);// ����ָ�� ��ֵԽС����ʾ����Խ�ֲڣ����Դ����Ĺ������䵽���棬Ҳ���Բ����ϴ������

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
    glViewport(0, 0, width, height);                // ��ͼ����ָ����,����ʾ����Ļ��

    glMatrixMode(GL_PROJECTION);                    // ѡ��͸�Ӿ���
    glLoadIdentity();                               // ���þ���
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);      // ����͸�Ӿ���

    glMatrixMode(GL_MODELVIEW);                     // ѡ��ģ�;���
    glLoadIdentity();
}

//----------------------------------------------------------------------------
//
static void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0f ,0.0f, -3.0f);
    //test_draw();
    r += 0.05f;
    glPushMatrix();
        glRotatef(r, 1.0f, 0.0f, 0.0f);
        glRotatef(r, 0.0f, 1.0f, 0.0f);

        glBindTexture(GL_TEXTURE_2D, texture.texID);
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
    glutSwapBuffers();      // ����������
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
    glutPostRedisplay();    // �����ػ�
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
    glMatrixMode(GL_PROJECTION);    // ͸�ӱ任
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

    glMatrixMode(GL_MODELVIEW);     // ģ�ͱ任
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
