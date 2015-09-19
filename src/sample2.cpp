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

#include <cmath>

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

#define BUFFER_OFFSET(offset) ((GLvoid*) NULL + offset)

struct Vertex
{
    float pos[3];
    float texcoord[2];
    float normal[3];
    float tangent[4];
};

//-------------------------------------------------------------
class Math
{
public:
    static const float EPSILON;

    static bool closeEnough(float f1, float f2)
    {
        // Determines whether the two floating-point values f1 and f2 are
        // close enough together that they can be considered equal.
        return fabsf((f1 - f2) / ((f2 == 0.0f) ? 1.0f : f2)) < EPSILON;
    }
};
const float Math::EPSILON = 1e-6f;

//-------------------------------------------------------------
//
class Vector2
{
    friend Vector2 operator*(float lhs, const Vector2 &rhs);
    friend Vector2 operator-(const Vector2 &v);

public:
    float x, y;

    static float dot(const Vector2 &p, const Vector2 &q);

    Vector2() {}
    Vector2(float x_, float y_);
    ~Vector2() {}

    bool operator==(const Vector2 &rhs) const;
    bool operator!=(const Vector2 &rhs) const;

    Vector2 &operator+=(const Vector2 &rhs);
    Vector2 &operator-=(const Vector2 &rhs);
    Vector2 &operator*=(float scalar);
    Vector2 &operator/=(float scalar);

    Vector2 operator+(const Vector2 &rhs) const;
    Vector2 operator-(const Vector2 &rhs) const;
    Vector2 operator*(float scalar) const;
    Vector2 operator/(float scalar) const;

    float magnitude() const;
    float magnitudeSq() const;
    Vector2 inverse() const;
    void normalize();
    void set(float x_, float y_);
};

inline Vector2 operator*(float lhs, const Vector2 &rhs)
{
    return Vector2(lhs * rhs.x, lhs * rhs.y);
}

inline Vector2 operator-(const Vector2 &v)
{
    return Vector2(-v.x, -v.y);
}

inline float Vector2::dot(const Vector2 &p, const Vector2 &q)
{
    return (p.x * q.x) + (p.y * q.y);
}

inline Vector2::Vector2(float x_, float y_) : x(x_), y(y_) {}

inline bool Vector2::operator==(const Vector2 &rhs) const
{
    return Math::closeEnough(x, rhs.x) && Math::closeEnough(y, rhs.y);
}

inline bool Vector2::operator!=(const Vector2 &rhs) const
{
    return !(*this == rhs);
}

inline Vector2 &Vector2::operator+=(const Vector2 &rhs)
{
    x += rhs.x, y += rhs.y;
    return *this;
}

inline Vector2 &Vector2::operator-=(const Vector2 &rhs)
{
    x -= rhs.x, y -= rhs.y;
    return *this;
}

inline Vector2 &Vector2::operator*=(float scalar)
{
    x *= scalar, y *= scalar;
    return *this;
}

inline Vector2 &Vector2::operator/=(float scalar)
{
    x /= scalar, y /= scalar;
    return *this;
}

inline Vector2 Vector2::operator+(const Vector2 &rhs) const
{
    Vector2 tmp(*this);
    tmp += rhs;
    return tmp;
}

inline Vector2 Vector2::operator-(const Vector2 &rhs) const
{
    Vector2 tmp(*this);
    tmp -= rhs;
    return tmp;
}

inline Vector2 Vector2::operator*(float scalar) const
{
    return Vector2(x * scalar, y * scalar);
}

inline Vector2 Vector2::operator/(float scalar) const
{
    return Vector2(x / scalar, y / scalar);
}

inline float Vector2::magnitude() const
{
    return sqrtf((x * x) + (y * y));
}

inline float Vector2::magnitudeSq() const
{
    return (x * x) + (y * y);
}

inline Vector2 Vector2::inverse() const
{
    return Vector2(-x, -y);
}

inline void Vector2::normalize()
{
    float invMag = 1.0f / magnitude();
    x *= invMag, y *= invMag;
}

inline void Vector2::set(float x_, float y_)
{
    x = x_, y = y_;
}
//-------------------------------------------------------------
//  Vector3

class Vector3
{
    friend Vector3 operator*(float lhs, const Vector3 &rhs);
    friend Vector3 operator-(const Vector3 &v);

public:
    float x, y, z;

    static Vector3 cross(const Vector3 &p, const Vector3 &q);
    static float dot(const Vector3 &p, const Vector3 &q);

    Vector3() {}
    Vector3(float x_, float y_, float z_);
    ~Vector3() {}

    bool operator==(const Vector3 &rhs) const;
    bool operator!=(const Vector3 &rhs) const;

    Vector3 &operator+=(const Vector3 &rhs);
    Vector3 &operator-=(const Vector3 &rhs);
    Vector3 &operator*=(float scalar);
    Vector3 &operator/=(float scalar);

    Vector3 operator+(const Vector3 &rhs) const;
    Vector3 operator-(const Vector3 &rhs) const;
    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;

    float magnitude() const;
    float magnitudeSq() const;
    Vector3 inverse() const;
    void normalize();
    void set(float x_, float y_, float z_);
};

inline Vector3 operator*(float lhs, const Vector3 &rhs)
{
    return Vector3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
}

inline Vector3 operator-(const Vector3 &v)
{
    return Vector3(-v.x, -v.y, -v.z);
}

inline Vector3 Vector3::cross(const Vector3 &p, const Vector3 &q)
{
    return Vector3((p.y * q.z) - (p.z * q.y),
        (p.z * q.x) - (p.x * q.z),
        (p.x * q.y) - (p.y * q.x));
}

inline float Vector3::dot(const Vector3 &p, const Vector3 &q)
{
    return (p.x * q.x) + (p.y * q.y) + (p.z * q.z);
}

inline Vector3::Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

inline Vector3 &Vector3::operator+=(const Vector3 &rhs)
{
    x += rhs.x, y += rhs.y, z += rhs.z;
    return *this;
}

inline bool Vector3::operator==(const Vector3 &rhs) const
{
    return Math::closeEnough(x, rhs.x) && Math::closeEnough(y, rhs.y)
        && Math::closeEnough(z, rhs.z);
}

inline bool Vector3::operator!=(const Vector3 &rhs) const
{
    return !(*this == rhs);
}

inline Vector3 &Vector3::operator-=(const Vector3 &rhs)
{
    x -= rhs.x, y -= rhs.y, z -= rhs.z;
    return *this;
}

inline Vector3 &Vector3::operator*=(float scalar)
{
    x *= scalar, y *= scalar, z *= scalar;
    return *this;
}

inline Vector3 &Vector3::operator/=(float scalar)
{
    x /= scalar, y /= scalar, z /= scalar;
    return *this;
}

inline Vector3 Vector3::operator+(const Vector3 &rhs) const
{
    Vector3 tmp(*this);
    tmp += rhs;
    return tmp;
}

inline Vector3 Vector3::operator-(const Vector3 &rhs) const
{
    Vector3 tmp(*this);
    tmp -= rhs;
    return tmp;
}

inline Vector3 Vector3::operator*(float scalar) const
{
    return Vector3(x * scalar, y * scalar, z * scalar);
}

inline Vector3 Vector3::operator/(float scalar) const
{
    return Vector3(x / scalar, y / scalar, z / scalar);
}

inline float Vector3::magnitude() const
{
    return sqrtf((x * x) + (y * y) + (z * z));
}

inline float Vector3::magnitudeSq() const
{
    return (x * x) + (y * y) + (z * z);
}

inline Vector3 Vector3::inverse() const
{
    return Vector3(-x, -y, -z);
}

inline void Vector3::normalize()
{
    float invMag = 1.0f / magnitude();
    x *= invMag, y *= invMag, z *= invMag;
}

inline void Vector3::set(float x_, float y_, float z_)
{
    x = x_, y = y_, z = z_;
}
//----------------------------------------------------------


void    CalcTangentVector(const float pos1[3], const float pos2[3],
                          const float pos3[3], const float texCoord1[2],
                          const float texCoord2[2], const float texCoord3[2],
                          const float normal[3], float tangent[4]);


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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

        glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &img->red);
        glBindTexture(GL_TEXTURE_2D, 0);
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
    GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_position[] = { 0.0f, 3.0f, -5.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);  // 设置环境光颜色
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);  // 设置漫反射的颜色
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);  // 设置镜面反射的颜色
   // glLightfv(GL_LIGHT0, GL_POSITION, light_position);  // 设置灯的位置

    // 设置材质的颜色
    GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat high_shininess[] = { 100.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);   // 设置环境光颜色
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);   // 设置漫反射的颜色
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);  // 设置镜面反射的颜色
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);// 镜面指数 该值越小，表示材质越粗糙，点光源发射的光线照射到上面，也可以产生较大的亮点

    //----------------------------- 设置 shader ----------------------------
    GLuint colormaptex = LoadBitmap24("Data/colormap.bmp");
    GLuint normalmaptex = LoadBitmap24("Data/normalmap.bmp");
    GLuint program = SampleBuildProgram("Data/normalmap.vert", "Data/normalmap.frag");

    //----------------------------- 设置 shader ----------------------------
    Vertex d_cube[24] =
    {
        // Positive Z Face.
        { -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f},
        {  1.0f, -1.0f,  1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f},
        {  1.0f,  1.0f,  1.0f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f},
        { -1.0f,  1.0f,  1.0f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f},

        // Negative Z Face.
        {  1.0f, -1.0f, -1.0f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f},
        { -1.0f, -1.0f, -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f},
        { -1.0f,  1.0f, -1.0f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f},
        {  1.0f,  1.0f, -1.0f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f},

        // Positive Y Face.
        { -1.0f,  1.0f,  1.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f},
        {  1.0f,  1.0f,  1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f},
        {  1.0f,  1.0f, -1.0f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f},
        { -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f},

        // Negative Y Face.
        { -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f},
        {  1.0f, -1.0f, -1.0f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f},
        {  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f},
        { -1.0f, -1.0f,  1.0f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f},

        // Positive X Face.
        {  1.0f, -1.0f,  1.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f},
        {  1.0f, -1.0f, -1.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f},
        {  1.0f,  1.0f, -1.0f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f},
        {  1.0f,  1.0f,  1.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f},

        // Negative X Face.
        { -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f},
        { -1.0f, -1.0f,  1.0f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f},
        { -1.0f,  1.0f,  1.0f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f},
        { -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f}
    };

    Vertex *pVertex1 = 0;
    Vertex *pVertex2 = 0;
    Vertex *pVertex3 = 0;
    Vertex *pVertex4 = 0;
    float tangent[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    for (int i = 0; i < 24; i += 4)
    {
        pVertex1 = &d_cube[i];
        pVertex2 = &d_cube[i + 1];
        pVertex3 = &d_cube[i + 2];
        pVertex4 = &d_cube[i + 3];

        CalcTangentVector(
            pVertex1->pos, pVertex2->pos, pVertex4->pos,
            pVertex1->texcoord, pVertex2->texcoord, pVertex4->texcoord,
            pVertex1->normal, tangent);

        pVertex1->tangent[0] = tangent[0];
        pVertex1->tangent[1] = tangent[1];
        pVertex1->tangent[2] = tangent[2];
        pVertex1->tangent[3] = tangent[3];

        pVertex2->tangent[0] = tangent[0];
        pVertex2->tangent[1] = tangent[1];
        pVertex2->tangent[2] = tangent[2];
        pVertex2->tangent[3] = tangent[3];

        pVertex3->tangent[0] = tangent[0];
        pVertex3->tangent[1] = tangent[1];
        pVertex3->tangent[2] = tangent[2];
        pVertex3->tangent[3] = tangent[3];

        pVertex4->tangent[0] = tangent[0];
        pVertex4->tangent[1] = tangent[1];
        pVertex4->tangent[2] = tangent[2];
        pVertex4->tangent[3] = tangent[3];
    }

    // Store the cube's geometry in a Vertex Buffer Object.
    GLuint d_vertexBuffer;
    glGenBuffers(1, &d_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, d_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(d_cube), d_cube, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

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
        gluLookAt(0.0f,0.0f,-6.0f, 0.0f,0.0f,0.0f, 0.0f,1.0f,0.0);

        glPushMatrix();
        glRotatef(rx, 1.0f, 0.0f, 0.0f);
        glRotatef(ry, 0.0f, 1.0f, 0.0f);

        rx = rx > 360 ? 0 : (rx + 0.1f);
        ry = ry > 360 ? 0 : (ry + 0.1f);

        glPushAttrib(GL_LIGHTING_BIT);

        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glUseProgram(program);
        glUniform1i(glGetUniformLocation(program, "colorMap"), 0);
        glUniform1i(glGetUniformLocation(program, "normalMap"), 1);

        glActiveTexture(GL_TEXTURE1);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, normalmaptex);

        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, colormaptex);

        //===============================================
        // draw
        glBindBuffer(GL_ARRAY_BUFFER, d_vertexBuffer);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));

        glClientActiveTexture(GL_TEXTURE0);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(sizeof(float) * 3));

        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(sizeof(float) * 5));

        glClientActiveTexture(GL_TEXTURE1);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(4, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(sizeof(float) * 8));

        glDrawArrays(GL_QUADS, 0, sizeof(d_cube) / sizeof(d_cube[0]));

        glPopMatrix();

        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glClientActiveTexture(GL_TEXTURE0);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);

        glUseProgram(0);
        glPopAttrib();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}

void CalcTangentVector(const float pos1[3], const float pos2[3],
                       const float pos3[3], const float texCoord1[2],
                       const float texCoord2[2], const float texCoord3[2],
                       const float normal[3], float tangent[4])
{
    // Given the 3 vertices (position and texture coordinates) of a triangle
    // calculate and return the triangle's tangent vector.

    // Create 2 vectors in object space.
    //
    // edge1 is the vector from vertex positions pos1 to pos2.
    // edge2 is the vector from vertex positions pos1 to pos3.
    Vector3 edge1(pos2[0] - pos1[0], pos2[1] - pos1[1], pos2[2] - pos1[2]);
    Vector3 edge2(pos3[0] - pos1[0], pos3[1] - pos1[1], pos3[2] - pos1[2]);

    edge1.normalize();
    edge2.normalize();

    // Create 2 vectors in tangent (texture) space that point in the same
    // direction as edge1 and edge2 (in object space).
    //
    // texEdge1 is the vector from texture coordinates texCoord1 to texCoord2.
    // texEdge2 is the vector from texture coordinates texCoord1 to texCoord3.
    Vector2 texEdge1(texCoord2[0] - texCoord1[0], texCoord2[1] - texCoord1[1]);
    Vector2 texEdge2(texCoord3[0] - texCoord1[0], texCoord3[1] - texCoord1[1]);

    texEdge1.normalize();
    texEdge2.normalize();

    // These 2 sets of vectors form the following system of equations:
    //
    //  edge1 = (texEdge1.x * tangent) + (texEdge1.y * bitangent)
    //  edge2 = (texEdge2.x * tangent) + (texEdge2.y * bitangent)
    //
    // Using matrix notation this system looks like:
    //
    //  [ edge1 ]     [ texEdge1.x  texEdge1.y ]  [ tangent   ]
    //  [       ]  =  [                        ]  [           ]
    //  [ edge2 ]     [ texEdge2.x  texEdge2.y ]  [ bitangent ]
    //
    // The solution is:
    //
    //  [ tangent   ]        1     [ texEdge2.y  -texEdge1.y ]  [ edge1 ]
    //  [           ]  =  -------  [                         ]  [       ]
    //  [ bitangent ]      det A   [-texEdge2.x   texEdge1.x ]  [ edge2 ]
    //
    //  where:
    //        [ texEdge1.x  texEdge1.y ]
    //    A = [                        ]
    //        [ texEdge2.x  texEdge2.y ]
    //
    //    det A = (texEdge1.x * texEdge2.y) - (texEdge1.y * texEdge2.x)
    //
    // From this solution the tangent space basis vectors are:
    //
    //    tangent = (1 / det A) * ( texEdge2.y * edge1 - texEdge1.y * edge2)
    //  bitangent = (1 / det A) * (-texEdge2.x * edge1 + texEdge1.x * edge2)
    //     normal = cross(tangent, bitangent)

    Vector3 t;
    Vector3 b;
    Vector3 n(normal[0], normal[1], normal[2]);

    float det = (texEdge1.x * texEdge2.y) - (texEdge1.y * texEdge2.x);

    if (Math::closeEnough(det, 0.0f))
    {
        t.set(1.0f, 0.0f, 0.0f);
        b.set(0.0f, 1.0f, 0.0f);
    }
    else
    {
        det = 1.0f / det;

        t.x = (texEdge2.y * edge1.x - texEdge1.y * edge2.x) * det;
        t.y = (texEdge2.y * edge1.y - texEdge1.y * edge2.y) * det;
        t.z = (texEdge2.y * edge1.z - texEdge1.y * edge2.z) * det;

        b.x = (-texEdge2.x * edge1.x + texEdge1.x * edge2.x) * det;
        b.y = (-texEdge2.x * edge1.y + texEdge1.x * edge2.y) * det;
        b.z = (-texEdge2.x * edge1.z + texEdge1.x * edge2.z) * det;

        t.normalize();
        b.normalize();
    }

    // Calculate the handedness of the local tangent space.
    // The bitangent vector is the cross product between the triangle face
    // normal vector and the calculated tangent vector. The resulting bitangent
    // vector should be the same as the bitangent vector calculated from the
    // set of linear equations above. If they point in different directions
    // then we need to invert the cross product calculated bitangent vector. We
    // store this scalar multiplier in the tangent vector's 'w' component so
    // that the correct bitangent vector can be generated in the normal mapping
    // shader's vertex shader.

    Vector3 bitangent = Vector3::cross(n, t);
    float handedness = (Vector3::dot(bitangent, b) < 0.0f) ? -1.0f : 1.0f;

    tangent[0] = t.x;
    tangent[1] = t.y;
    tangent[2] = t.z;
    tangent[3] = handedness;
}

