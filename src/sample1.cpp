
/**

����Ҫʵ�ֵĶ���:
0. �򵥵�OpenGL����ʹ��glfw��ʵ��
1. ��GUI
2. �򵥹�������ms3d, �����������Ĺ�������



*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

struct BitmapHeader
{
    unsigned short type;
    unsigned int size;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned short offbits;

};

struct BitmapInfoHeader
{
    unsigned int size;
    int width;
    int height;
    unsigned short planes;
    unsigned short bitcount;
    unsigned int compression;
    unsigned int sizeimage;
    int xpelspermeter;
    int ypelspermeter;
    unsigned int clrused;
    unsigned int clrimportant;
};

unsigned int LoadBitmap(const char* filename)
{
    FILE* fp = fopen(filename, "rb");
    if (!fp)
        return 0;

    BitmapHeader header;
    BitmapInfoHeader infoheader;

    fread(&header, sizeof(BitmapHeader), 1, fp);
    fread(&infoheader, sizeof(BitmapInfoHeader), 1, fp);



    fclose(fp);
    return 0;
}

//----------------------------------------------------------------------------
//
/*
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
*/
//----------------------------------------------------------------------------
//
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

static void error_callback(int error, const char* description){fputs(description, stderr);}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

static void mouse_callback(GLFWwindow* window, int button, int action, int mods){}
static void cursorpos_callback(GLFWwindow *window, double x, double y){}

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
    //LoadGLTextures();

    LoadBitmap("Data/np.bmp");
    glClearColor(0,0,0,1);      // ������ĻΪ��ɫ
    glEnable(GL_CULL_FACE);     // �������޳�
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);    // ������Ȳ���

    glEnable(GL_LIGHT0);        // ���õƹ�0
    glEnable(GL_NORMALIZE);     // ���÷���
    glEnable(GL_COLOR_MATERIAL);// ���ò���ģʽ
    glEnable(GL_LIGHTING);      // �򿪵ƹ�

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER ,0.9);//0.5���Ի����κ���0~1֮�����
    glShadeModel(GL_SMOOTH);

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

    //glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float ratio = (float) width / (float) height;
        glViewport(0, 0, width, height);                // ��ͼ����ָ����,����ʾ����Ļ��
        glMatrixMode(GL_PROJECTION);                    // ѡ��͸�Ӿ���
        glLoadIdentity();                               // ���þ���
        glFrustum(-ratio, ratio, -1.0, 1.0, 2.0, 100.0);// ����͸�Ӿ���
        glMatrixMode(GL_MODELVIEW);                     // ѡ��ģ�;���
        glLoadIdentity();

        // render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glTranslatef(0.0f ,0.0f, -3.0f);

        const GLfloat vers[] = { 0.0f,0.0f,0.0f, 25.0f,0.0f,0.0f, 30.0f,30.0f,0.0f, 0.0f,25.0f,0.0f, };
        const GLfloat cors[] = { 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, };
        const GLint indices[] = {  0,1,2,3 };

        glPushMatrix();
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, vers);
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(3, GL_FLOAT, 0, cors);
            glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, indices);
        glPopMatrix();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}


