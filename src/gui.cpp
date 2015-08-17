#include "gui.h"
#include "assert.h"
#include <stdio.h>


namespace Simple
{
    GLint g_gui_width = 1;
    GLint g_gui_height = 1;

    const unsigned int MAX_QUAD = 1000;
    GLint g_gui_num = 0;
    GLfloat g_gui_vertexs[MAX_QUAD * 4 * 3];
    GLfloat g_gui_colors[MAX_QUAD * 4 * 3];
    GLint g_gui_indices[MAX_QUAD * 4];

    std::vector<Window*> renderWins;
    Gui* glGui = NULL;

    inline void store_rect(int i, float x, float y, float w, float h)
    {
        /*
        x /= g_gui_width;
        y /= g_gui_height;
        w /= g_gui_width;
        h /= g_gui_height;
        */
        // 4 vertex
        // vertex 1
        int ii = i * 12;
        g_gui_vertexs[ii] = x;
        g_gui_vertexs[ii + 1] = y;
        g_gui_vertexs[ii + 2] = 0;
        // vertex 2
        g_gui_vertexs[ii + 3] = x + w;
        g_gui_vertexs[ii + 4] = y;
        g_gui_vertexs[ii + 5] = 0;
        // vertex 3
        g_gui_vertexs[ii + 6] = x + w;
        g_gui_vertexs[ii + 7] = y + h;
        g_gui_vertexs[ii + 8] = 0;
        // vertex 4
        g_gui_vertexs[ii + 9] = x;
        g_gui_vertexs[ii + 10] = y + h;
        g_gui_vertexs[ii + 11] = 0;
    }

    inline void stre_indice(int i)
    {
        int ii = i * 4;
        g_gui_indices[ii] = ii;
        g_gui_indices[ii + 1] = ii + 1;
        g_gui_indices[ii + 2] = ii + 2;
        g_gui_indices[ii + 3] = ii + 3;
    }

    inline void store_color(int i, float r, float g, float b)
    {
        int ii = i * 12;
        g_gui_colors[ii] = r;
        g_gui_colors[ii + 1] = g;
        g_gui_colors[ii + 2] = b;

        g_gui_colors[ii + 3] = r;
        g_gui_colors[ii + 4] = g;
        g_gui_colors[ii + 5] = b;

        g_gui_colors[ii + 6] = r;
        g_gui_colors[ii + 7] = g;
        g_gui_colors[ii + 8] = b;

        g_gui_colors[ii + 9] = r;
        g_gui_colors[ii + 10] = g;
        g_gui_colors[ii + 11] = b;
    }

    void gui_set_view(float w, float h)
    {
        g_gui_width = w;
        g_gui_height = h;
    }

    void gui_add_rect(float x, float y, float w, float h, float r, float g, float b)
    {
        assert(g_gui_num < MAX_QUAD);
        store_rect(g_gui_num, x, y, w, h);
        stre_indice(g_gui_num);
        store_color(g_gui_num, r, g, b);
        g_gui_num++;
    }

    void gui_update()
    {
        std::vector<Window*>::iterator i = renderWins.begin();
            for (; i != renderWins.end(); ++i)
                (*i)->draw();
    }

    void gui_render()
    {
        if (g_gui_num > 0)
        {
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, g_gui_vertexs);

            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(3, GL_FLOAT, 0, g_gui_colors);

            glDrawElements(GL_QUADS, g_gui_num * 4, GL_UNSIGNED_INT, g_gui_indices);

            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_COLOR_ARRAY);
        }
    }

    void gui_clear()
    {
        g_gui_num = 0;
    }

        //===========================================================
        //
        Window::Window()
        {
            mMouseDownWin = NULL;
        }
        //===========================================================
        //
        Window::Window(float x, float y, float w, float h)
        {
            mSize.w = w;
            mSize.h = h;
            mPosition.x = x;
            mPosition.y = y;
            mMouseDownWin = NULL;
        }
        //===========================================================
        //
        Window::~Window()
        {
            mMouseDownWin = NULL;
            std::vector<Window*>::iterator i = mChildren.begin();
            for (; i != mChildren.end(); ++i)
                delete (*i);
            mChildren.clear();
        }
        //===========================================================
        //
        void Window::removeChild(Window *win)
        {
            std::vector<Window*>::iterator i = mChildren.begin();
            for (; i != mChildren.end(); ++i)
            {
                if ((*i) == win)
                {
                    mChildren.erase(i);
                    break;
                }
            }
            if (mMouseDownWin == win)
                mMouseDownWin = NULL;
        }

        //===========================================================
        //
        bool Window::hitTest(float x, float y)
        {
            if (x < mPosition.x || x > mPosition.x + mSize.w)
                return false;
            if (y < mPosition.y || y > mPosition.y + mSize.h)
                return false;

            printf("hitTest true %f, %f\n", x, y);
            return true;
        }

        //===========================================================
        //
        int Window::mouseEvent(int button, int state, int x, int y)
        {
            if (button == GLUT_LEFT_BUTTON)
            {
                if (state == GLUT_DOWN)
                    mouseDown(x, y);
                else if (state == GLUT_UP)
                    mouseUp(x, y);
            }
            return 0;
        }
        //===========================================================
        //
        int Window::mouseDown(float x, float y)
        {
            if (!hitTest(x, y))
                return 0;

            std::vector<Window*>::iterator i = mChildren.begin();
            for (; i != mChildren.end(); ++i)
            {
                int ret = (*i)->mouseDown(x, y);
                if (ret > 0)
                {
                    mMouseDownWin = (*i);
                    return ret;
                }
            }

            mMouseDownWin = this;
            return 1;
        }
        //===========================================================
        //
        void Window::mouseMove(float x, float y)
        {
            if (mMouseDownWin)
                mMouseDownWin->mouseMove(x, y);
        }
        //===========================================================
        //
        void Window::mouseUp(float x, float y)
        {
            if (mMouseDownWin)
                mMouseDownWin->mouseUp(x, y);
        }
        //===========================================================
        //
        void Window::draw(float x, float y)
        {
            gui_add_rect(mPosition.x + x, mPosition.y + y, mSize.w, mSize.h, mColor.r, mColor.g, mColor.b);
            std::vector<Window*>::iterator i = mChildren.begin();
            for (; i != mChildren.end(); ++i)
                (*i)->draw(mPosition.x + x, mPosition.y + y);
        }

        //===========================================================
        //
        Button::Button(): Window(0, 0, 1, 1),mCallBack(NULL) {}
        //===========================================================
        //
        Button::Button(float x, float y, float w, float h): Window(x, y, w, h),mCallBack(NULL) {}

        //===========================================================
        //
        int Button::mouseDown(float x, float y)
        {
            int ret = Window::mouseDown(x, y);
            if (ret > 0 && mCallBack != NULL)
                mCallBack();
            return ret;
        }


        //===========================================================
        // 这个是很另类的做法需要注意的
        GLLabel::GLLabel():Window(0, 0, 1, 1){}
        //===========================================================
        //
        GLLabel::GLLabel(const std::string& str)
        : Window(0, 0, 1, 1), mString(str)
        {
        }

        //===========================================================
        // 这里无法叠在一起只有自己画了
        void GLLabel::draw(float x, float y)
        {
            int sz = mString.size();
            const char* dt = mString.c_str();
            glPushMatrix();
            glTranslatef(x, y, 0);
            glScalef(0.1, 0.1, 0.1);
            glColor3f(mColor.r, mColor.g, mColor.b);
            for (int i = 0; i < sz; ++i)
                glutStrokeCharacter(GLUT_STROKE_ROMAN, dt[i]);
            glPopMatrix();
        }
        //===========================================================
        //

        Slider::Slider()
        : Window(0, 0, 1, 1)
        {
            mBar = new Window(0, 0, mSize.w, 1);
            mBar->setColor(Color(100, 255, 255));
            addChild(mBar);
        }
        //===========================================================
        //
        Slider::Slider(float x, float y, float w, float h):Window(x, y, w, h)
        {
            mBar = new Window(0, 0, mSize.w, 1);
            mBar->setColor(Color(100, 255, 255));
            addChild(mBar);
        }

        //===========================================================
        //
        int Slider::mouseDown(float x, float y)
        {
            int ret = Window::mouseDown(x, y);
            if (ret > 0)
            {
                if (mMouseDownWin == mBar)
                {

                }
            }
            return ret;
        }
        //===========================================================
        //
        void Slider::mouseMove(float x, float y)
        {

        }
        //===========================================================
        //
        void Slider::mouseUp(float x, float y)
        {

        }
        //===========================================================
        //
        Gui* Gui::create()
        {
            assert(glGui == NULL);
            glGui = new Gui();
            return glGui;
        }
        //===========================================================
        //
        void Gui::destroy()
        {
            std::vector<Window*>::iterator i = renderWins.begin();
            for (; i != renderWins.end(); ++i)
                delete (*i);
            renderWins.clear();
            delete glGui;
            glGui = NULL;
        }

        void Gui::setView(float width, float height)
        {
            gui_set_view(width, height);
        }
        //===========================================================
        //
        void Gui::update()
        {
            std::vector<Window*>::iterator i = renderWins.begin();
            for (; i != renderWins.end(); ++i)
                (*i)->update(0.3f);
        }
        //===========================================================
        //
        Button* Gui::addButton(const std::string& name, float x, float y, float w, float h)
        {
            if (hasWindow(name))
                return NULL;

            Button *button = new Button(x, y, w, h);
            button->setName(name);
            renderWins.push_back(button);
            return button;
        }
        //===========================================================
        //
        GLLabel* Gui::addLabel(const std::string& name, const std::string& str)
        {
            if (hasWindow(name))
                return NULL;

            GLLabel *label = new GLLabel(str);
            label->setName(name);
            renderWins.push_back(label);
            return label;
        }
        //===========================================================
        //
        Slider* Gui::addSlider(const std::string& name, float x, float y, float w, float h)
        {
            if (hasWindow(name))
                return NULL;
            Slider *slider = new Slider(x, y, w, h);
            slider->setName(name);
            renderWins.push_back(slider);
            return slider;
        }
        //===========================================================
        //
        void Gui::removeWindow(Window *win)
        {
            std::vector<Window*>::iterator i = renderWins.begin();
            for (; i != renderWins.end(); ++i)
            {
                if ((*i) == win)
                {
                    renderWins.erase(i);
                    break;
                }
            }
        }
        //===========================================================
        //
        void Gui::removeWindowByName(const std::string& name)
        {
            std::vector<Window*>::iterator i = renderWins.begin();
            for (; i != renderWins.end(); ++i)
            {
                if ((*i)->getName() == name)
                {
                    renderWins.erase(i);
                    break;
                }
            }
        }
        //===========================================================
        //
        bool Gui::hasWindow(const std::string& name)
        {
            std::vector<Window*>::iterator i = renderWins.begin();
            for (; i != renderWins.end(); ++i)
            {
                if ((*i)->getName() == name)
                {
                    return true;
                }
            }
            return false;
        }

        int Gui::mouseEvent(int button, int state, int x, int y)
        {
            std::vector<Window*>::iterator i = renderWins.begin();
            for (; i != renderWins.end(); ++i)
            {
                if ((*i)->mouseEvent(button, state, x, y))
                    break;
            }
            return 0;
        }
        //===========================================================
        //
        void Gui::render()
        {
            gui_update();
            gui_render();
            gui_clear();
        }
}
