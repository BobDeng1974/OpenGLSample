/**
整体要实现的东东:
简单的OpenGL程序使用glfw来实现
*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//---------------------------------------------------------
// 全局变量
double g_mouse_sx = 0, g_mouse_sy = 0; // 鼠标按下的位置
double g_roll_x = 0, g_roll_y = 0; // 鼠标拖动的偏移

bool g_is_press = false;    // 鼠标是否点击

#pragma pack(push, 1)
struct BitmapFileHeader
{
    unsigned short type;          //   位图文件的类型，必须为BM
    unsigned long size;           //   位图文件的大小，以字节为单位
    unsigned short reserved1;     //   位图文件保留字，必须为0
    unsigned short reserved2;     //   位图文件保留字，必须为0
    unsigned long offbits;        //   位图数据的起始位置，以相对于位图
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
// bmp 存储数据的数序是bgr, opengl绘制使用颜色顺序是rgb,所有需要转换一下
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

inline GLchar* SampleReadFileData(const char* filename, GLsizei &sz)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
        return NULL;
    fseek(fp, 0L, SEEK_END);
    sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    GLchar* data = new GLchar[sz];
    sz = fread((void*)data, 1, sz, fp);
    fclose(fp);
    return data;
}

GLuint  SampleBuildShader(const char* filename, GLenum type)
{
    GLsizei sz = 0;
    GLint succ;
    const GLchar* vsSource = SampleReadFileData(filename, sz);
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

GLuint SampleBuildProgram(const char*vsfn, const char* frfn)
{
    GLint linked;
    GLuint vsshader = SampleBuildShader(vsfn, GL_VERTEX_SHADER);
    GLuint frshader = SampleBuildShader(frfn, GL_FRAGMENT_SHADER);
    GLuint program = glCreateProgram();
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

    GLuint tex = LoadBitmap24("Data/demo.bmp");     // 加载一张bmp纹理
    GLuint btn = LoadBitmap24("Data/button.bmp");



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

    const GLfloat vers[] = {
        -0.5,-0.5, 0.5,  0.5,-0.5, 0.5,  0.5,0.5, 0.5, -0.5,0.5, 0.5,   // 正面 4个顶点
         0.5,-0.5,-0.5, -0.5,-0.5,-0.5, -0.5,0.5,-0.5,  0.5,0.5,-0.5,   // 背面 4个顶点

        -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,-0.5, -0.5, 0.5,-0.5,   // 上面 4个顶点
        -0.5,-0.5,-0.5, 0.5,-0.5,-0.5, 0.5,-0.5, 0.5, -0.5,-0.5, 0.5,   // 下面 4个顶点

        -0.5,-0.5,-0.5, -0.5,-0.5, 0.5, -0.5,0.5, 0.5, -0.5,0.5,-0.5,   // 左面 4个顶点
         0.5,-0.5, 0.5,  0.5,-0.5,-0.5,  0.5,0.5,-0.5,  0.5,0.5, 0.5,   // 右面 4个顶点
    };

    const GLfloat cors[] = {
        1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,
    };

    const GLfloat nors[] = {
        0,0,1, 0,0,1, 0,0,1, 0,0,1,
        0,0,-1, 0,0,-1, 0,0,-1, 0,0,-1,
        0,1,0, 0,1,0, 0,1,0, 0,1,0,
        0,-1,0, 0,-1,0, 0,-1,0, 0,-1,0,
        -1,0,0, -1,0,0, -1,0,0, -1,0,0,
    };

    const GLfloat uvs[] = {
        0,0, 1,0, 1,1, 0,1,
        0,0, 1,0, 1,1, 0,1,
        0,0, 1,0, 1,1, 0,1,
        0,0, 1,0, 1,1, 0,1,
        0,0, 1,0, 1,1, 0,1,
        0,0, 1,0, 1,1, 0,1,
    };

    // 绘制三角形面的顺序是,前后,左右,上下
    const GLint indices[] = {
        0,1,2, 0,2,3,
        4,5,6, 4,6,7,

        8,9,10, 8,10,11,
        12,13,14, 12,14,15,

        16,17,18, 16,18,19,
        20,21,22, 20,22,23,
    };

    const GLfloat btn_vers[] = {-53.5,-24, 0,  53.5,-24, 0,  53.5,24, 0, -53.5,24, 0};
    const GLfloat btn_cors[] = {1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f};
    const GLfloat btn_nors[] = { 0,0,1, 0,0,1, 0,0,1, 0,0,1,};
    const GLfloat btn_uvs[] = {0,0, 1,0, 1,1, 0,1};
    const GLint btn_indices[] = {0,1,2, 0,2,3,};

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
        // 3D场景部分
        //gluLookAt(0.0f,0.0f,-3.0f, 0.0f,0.0f,0.0f, 0.0f,1.0f,0.0);
        glBindTexture(GL_TEXTURE_2D, tex);      // 绑定纹理
        glPushMatrix();
        //glTranslatef(0, 0, 2);
        glRotatef(rx + g_roll_y, 1.0f, 0.0f, 0.0f);
        glRotatef(ry + g_roll_x, 0.0f, 1.0f, 0.0f);

            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, vers);

            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(3, GL_FLOAT, 0, cors);

            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT, 0, nors);

            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, 0, uvs);

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, indices);
        glPopMatrix();


        //======================================================================
        // GUI部分
        glPushMatrix();
        glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glPolygonMode(GL_FRONT, GL_FILL);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

        float mHalfWidth = width / 2;
        float mHalfHeight = height / 2;
		glOrtho(-mHalfWidth, mHalfWidth, -mHalfHeight, mHalfHeight, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_FOG);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, btn);
		//------------------------------
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, btn_vers);

            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(3, GL_FLOAT, 0, btn_cors);

            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT, 0, btn_nors);

            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, 0, btn_uvs);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, btn_indices);
		//------------------------------
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopAttrib();
		glPopClientAttrib();


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}


