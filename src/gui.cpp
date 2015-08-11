#include "gui.h"
#include "assert.h"


namespace Simple
{

        GLRender *glRender = NULL;
        Gui      *glGui = NULL;

        std::vector<Window*> renderWins;

        const unsigned int MAX_TRIANGLE = 1000;
        GLsizei g_count = 0;
        GLfloat g_vers[MAX_TRIANGLE * 4 * 3];
        GLbyte g_cors[MAX_TRIANGLE * 3];
        GLint g_indices[MAX_TRIANGLE * 4];

        void storeTriangle(int i, float x, float y, float w, float h)
        {
           // 4个顶点
           // 顶点1
            g_vers[i * 12] = x;
            g_vers[i * 12 + 1] = y;
            g_vers[i * 12 + 2] = 0;
            // 顶点2
            g_vers[i * 12 + 3] = x + w;
            g_vers[i * 12 + 4] = y;
            g_vers[i * 12 + 5] = 0;
            // 顶点3
            g_vers[i * 12 + 6] = x;
            g_vers[i * 12 + 7] = y + h;
            g_vers[i * 12 + 8] = 0;
            // 顶点4
            g_vers[i * 12 + 9] = x + w;
            g_vers[i * 12 + 10] = y + h;
            g_vers[i * 12 + 11] = 0;

            g_indices[i * 4] = i * 12;
            g_indices[i * 4 + 1] = i * 12 + 1;
            g_indices[i * 4 + 2] = i * 12 + 2;
            g_indices[i * 4 + 3] = i * 12 + 3;
        }

        void storeColor(int i, Color c)
        {
            g_cors[i * 3] = c.r;
            g_cors[i * 3 + 1] = c.g;
            g_cors[i * 3 + 2] = c.b;
        }

        void pushRect(float x, float y, float w, float h, Color c)
        {
            storeColor(g_count, c);
            storeTriangle(g_count, x, y, w, h);
            g_count++;
        }

        void do_render()
        {
            if (g_count > 0)
            {
                glEnableClientState(GL_VERTEX_ARRAY);
                glEnableClientState(GL_COLOR_ARRAY);

                glVertexPointer(3, GL_FLOAT, 0, g_vers);
                glColorPointer(3, GL_BYTE, 0, g_cors);
                glDrawElements(GL_TRIANGLE_STRIP, g_count, GL_UNSIGNED_INT, g_indices);
            }
        }

        //===========================================================
        // render
        GLRender::GLRender(){}
        //===========================================================
        //
        void GLRender::pushWindow(float x, float y, float w, float h, Color c)
        {
            storeColor(g_count, c);
            storeTriangle(g_count, x, y, w, h);
            g_count++;
        }
        //===========================================================
        //
        void GLRender::render()
        {
            g_count = 0;
            std::vector<Window*>::iterator i = renderWins.begin();
            for (; i != renderWins.end(); ++i)
                (*i)->draw();

            // 进行整体绘制
            if (g_count > 0)
            {
                glEnableClientState(GL_VERTEX_ARRAY);
                glEnableClientState(GL_COLOR_ARRAY);

                glVertexPointer(3, GL_FLOAT, 0, g_vers);
                glColorPointer(3, GL_BYTE, 0, g_cors);
                glDrawElements(GL_TRIANGLE_STRIP, g_count, GL_UNSIGNED_INT, g_indices);
            }
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
        int Window::mouseDown(float x, float y)
        {
            if (x < mPosition.x || x > mPosition.x + mSize.w)
                return 0;
            if (y < mPosition.y || y > mPosition.y + mSize.h)
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
        void Window::mosueUp(float x, float y)
        {
            if (mMouseDownWin)
                mMouseDownWin->mosueUp(x, y);
        }
        //===========================================================
        //
        void Window::draw(float x, float y)
        {
            glRender->pushWindow(mPosition.x + x, mPosition.y + y, mSize.w, mSize.h, mColor);
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
        void Slider::mosueUp(float x, float y)
        {

        }
        //===========================================================
        //
        Gui* Gui::create()
        {
            assert(glRender == NULL);
            assert(glGui == NULL);
            glRender = new GLRender();
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
            delete glRender;
            glRender = NULL;
            delete glGui;
            glGui = NULL;
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
        //===========================================================
        //
        void Gui::render()
        {
            assert(glRender);
            glRender->render();
        }
}
