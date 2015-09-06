#ifndef __GUI_H__
#define __GUI_H__

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <assert.h>

using namespace std;
/**
UI 这块，暂时这样写
1. 纹理方面使用两张，一张是fnt文字,一张是UI使用的纹理
2. 实现button, slider, label 三种UI

*/

namespace Simple
{
    typedef float vec3_t[3];
    typedef float vec2_t[2];

    struct graphic_buffer_t
    {
        unsigned int tex;
        vec3_t* vertexs;
        vec2_t* uvs;
        vec3_t* normals;
        vec3_t* colors;

        unsigned int num_trangles; // num of trangles
        unsigned int* indexs;
    };

    graphic_buffer_t* create_rd_buffer();
    void gpc_bind_texture(unsigned int texturId);
    unsigned int gpc_get_texture_id();
    unsigned int gpc_get_index();
    void gpc_push_vertex(float x, float y, float z);
    void gpc_push_uv(float u, float v);
    void gpc_push_color(float r, float g, float b);
    void gpc_push_normal(float x, float y, float z);
    void gpc_push_trangles_index(int v0, int v1, int v2);
    void gpc_push_next();
    void gpc_end_buff(graphic_buffer_t* t);

    void gpc_render(graphic_buffer_t* t);
    void delete_gpc_buffer(graphic_buffer_t* t);


    class Graphic
    {
    public:
        Graphic();
        ~Graphic();

        void setView(float w, float h);
        void bindtextrue(unsigned int tex);
        unsigned int getVertexIndex();
        void vertex(float x, float y, float z);
        void uv(float u, float v);
        void color(float r, float g, float b);
        void normal(float x, float y, float z);
        void index(int v0, int v1, int v2);
        void next_vertex();

        void finish(); // last one call this
        void render();
    protected:
        void beginDraw();
        void draw();
        void endDraw();
    private:
        vector<graphic_buffer_t*> mGraphics;
        float mWidth,mHeight;
    };

    extern Graphic *gp;

    // 最基础的UI,这里不使用纹理来进行直接使用颜色
    class rdPoint
    {
    public:
        float x, y, z;
        explicit rdPoint():x(0), y(0), z(0){}
        rdPoint(float x, float y, float z):x(x), y(y), z(z){}
        rdPoint operator=(const rdPoint& r) { return rdPoint(r.x, r.y, r.z); }
        rdPoint operator+(const rdPoint& r) { return rdPoint(x + r.x, y + r.y, z + r.z); }
        rdPoint operator-(const rdPoint& r) { return rdPoint(x - r.x, y - r.y, z - r.z); }
        rdPoint& operator+=(const rdPoint& r)
        {
            x += r.x;
            y += r.y;
            z += r.z;
            return *this;
        }
    };

    class rdSize
    {
    public:
        rdSize():w(0),h(0){}
        rdSize(float w, float h): w(w), h(h) {}
        float w, h;
    };

    class rdColor
    {
    public:
        explicit rdColor(): r(1.0f), g(1.0f), b(1.0f) {}
        rdColor(float r, float g, float b): r(r), g(g), b(b){}
        float r, g, b;
    };

    class rdRect
    {
    public:
        explicit rdRect(): x(0.0f), y(0.0f), w(1.0f), h(1.0f){}
        rdRect(float x, float y, float w, float h):x(x), y(y), w(w), h(h){}
        float x, y, w, h;
    };

    class rdTexture
    {
    public:
        explicit rdTexture(): tex(0), u0(0.0f), u1(0.0f), v0(0.0f), v1(0.0f){}
        rdTexture(unsigned int t, float ua, float ub, float va, float vb):tex(t), u0(ua), v0(va), u1(ub), v1(vb){}
        float u0, v0, u1, v1;
        unsigned int tex;
    };

    typedef void (*Function0)();
    typedef void (*Function1)(float value);

    //-------------------------------------------
    class Window
    {
    public:
        explicit Window();
        Window(float x = 0, float y = 0, float w = 1, float h = 1);
        virtual ~Window();

        void setName(const std::string& name) { mName = name; }
        const std::string getName(void) { return mName; }

        void setTexture(const rdTexture& tex) { mTexture = tex; }
        void setPosition(const rdPoint &v) { mPosition = v; }
        void setrdSize(const rdSize &v) { mrdSize = v; }
        void setColor(const rdColor &c) { mColor = c; }
        void addChild(Window *win) { mChildren.push_back(win); }

        const rdPoint& getPositoin(void)   {return mPosition; }
        const rdSize& getSize(void)       { return mrdSize; }
        const rdColor& getColor(void) { return mColor; }
        void removeChild(Window *win);

        bool hitTest(float x, float y);

        virtual void update(float dt) {}
        virtual int mouseEvent(int button, int state, int x, int y);

        virtual int mouseDown(float x, float y);
        virtual void mouseMove(float x, float y);
        virtual void mouseUp(float x, float y);

        virtual void draw(float x = 0, float y = 0);
    protected:

        rdColor mColor;
        rdSize mrdSize;
        rdPoint mPosition;
        std::string mName;
        std::vector<Window*> mChildren;

        rdTexture mTexture;

        Window* mMouseDownWin;
    };
    //-------------------------------------------
    class Button : public Window
    {
    public:
        explicit Button();
        Button(float x, float y, float w, float h);

        void setMouseDownHandle(Function0 func) { mCallBack = func; }

        virtual int mouseDown(float x, float y);
        virtual void mouseMove(float x, float y) {}
        virtual void mouseUp(float x, float y) {}

    protected:
        Function0 mCallBack;
    };
    //-------------------------------------------
    class Label : public Window
    {
    public:
        explicit Label();
        Label(const std::string& str);
        virtual void draw(float x = 0, float y = 0);
    protected:
        std::string mString;
    };
    //-------------------------------------------
    class Slider : public Window
    {
    public:
        explicit Slider();
        Slider(float x, float y, float w, float h);

        void setValue(float v) { mValue = v; }
        float getValue(void) { return mValue; }

        Window* getBar() { return mBar; }

        void setValueChangeHandle(Function1 func) { mValueChangeHandle = func; }
        virtual int mouseDown(float x, float y);
        virtual void mouseMove(float x, float y);
        virtual void mouseUp(float x, float y);
    protected:
        float mValue;
        Function1 mValueChangeHandle;
        Window* mBar;
        rdPoint mBegin;
        rdPoint mBarBegin;
    };
}


#endif // __GUI_H__
