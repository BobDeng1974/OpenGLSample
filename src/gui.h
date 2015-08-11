#ifndef GUI_H
#define GUI_H

#include <GL/glut.h>
#include <vector>
#include <string>
using namespace std;

namespace Simple
{

    // base function
    void gui_set_view(float w, float h);
    void gui_add_rect(float x, float y, float w, float h, float r = 1.0f, float g = 1.0f, float b = 1.0f);
    void gui_update();
    void gui_render();
    void gui_clear();

    // 最基础的UI,这里不使用纹理来进行直接使用颜色
    class Vector2d
    {
    public:
        float x, y, z;
        Vector2d():x(0), y(0), z(0){}
        Vector2d(float x, float y, float z):x(x), y(y), z(z){}
        Vector2d operator=(const Vector2d& r)
        {
            return Vector2d(r.x, r.y, r.z);
        }
        Vector2d operator+(const Vector2d& r)
        {
            return Vector2d(x + r.x, y + r.y, z + r.z);
        }
        Vector2d operator-(const Vector2d& r)
        {
            return Vector2d(x - r.x, y - r.y, z - r.z);
        }
        Vector2d& operator+=(const Vector2d& r)
        {
            x += r.x;
            y += r.y;
            z += r.z;
            return *this;
        }
    };

    class Size
    {
    public:
        Size():w(0),h(0){}
        Size(float w, float h): w(w), h(h) {}
        float w, h;
    };

    class Color
    {
    public:
        Color(): r(255), g(255), b(255) {}
        Color(float r, float g, float b): r(r), g(g), b(b){}
        float r, g, b;
    };

    typedef void (*Function0)();
    typedef void (*Function1)(float value);

    class Window
    {
    public:
        Window();
        Window(float x = 0, float y = 0, float w = 1, float h = 1);
        virtual ~Window();

        void setName(const std::string& name) { mName = name; }
        const std::string getName(void) { return mName; }

        void setPosition(const Vector2d &v) { mPosition = v; }
        void setSize(const Size &v) { mSize = v; }
        void setColor(const Color &c) { mColor = c; }
        void addChild(Window *win) { mChildren.push_back(win); }

        const Vector2d& getPositoin(void)   {return mPosition; }
        const Size& getSize(void)       { return mSize; }
        const Color& getColor(void) { return mColor; }
        void removeChild(Window *win);

        virtual void update(float dt) {}
        virtual int mouseDown(float x, float y);
        virtual void mouseMove(float x, float y);
        virtual void mosueUp(float x, float y);

        virtual void draw(float x = 0, float y = 0);
    protected:

        Color mColor;
        Size mSize;
        Vector2d mPosition;
        std::string mName;
        std::vector<Window*> mChildren;

        Window* mMouseDownWin;
    };

    class Button : public Window
    {
    public:
        Button();
        Button(float x, float y, float w, float h);

        void setMouseDownHandle(Function0 func) { mCallBack = func; }

        virtual int mouseDown(float x, float y);
        virtual void mouseMove(float x, float y) {}
        virtual void mosueUp(float x, float y) {}

    protected:
        Function0 mCallBack;
    };

    class GLLabel : public Window
    {
    public:
        GLLabel();
        GLLabel(const std::string& str);
        virtual void draw(float x = 0, float y = 0);
    protected:
        std::string mString;
    };

    class Slider : public Window
    {
    public:
        Slider();
        Slider(float x, float y, float w, float h);

        void setValue(float v) { mValue = v; }
        float getValue(void) { return mValue; }

        void setValueChangeHandle(Function1 func) { mValueChangeHandle = func; }
        virtual int mouseDown(float x, float y);
        virtual void mouseMove(float x, float y);
        virtual void mosueUp(float x, float y);
    protected:
        float mValue;
        Function1 mValueChangeHandle;
        Window* mBar;
        Vector2d mBegin;
    };

    class Gui
    {
    public:
        static Gui* create();
        static void destroy();

        void update();
        void render();

        Button* addButton(const std::string& name, float x, float y, float w, float h);
        GLLabel* addLabel(const std::string& name, const std::string& str);
        Slider* addSlider(const std::string& name, float x, float y, float w, float h);

        void removeWindow(Window *win);
        bool hasWindow(const std::string& name);
        void removeWindowByName(const std::string& name);
    };
}


#endif // GUI_H
