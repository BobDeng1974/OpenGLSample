#include "gui.h"
#include "assert.h"
#include <stdio.h>


namespace Simple
{
    GLint g_gui_width = 1;
    GLint g_gui_height = 1;
    GLfloat g_gui_ar = 1;
    GLfloat g_gui_half_width = 1;
    GLfloat g_gui_half_height = 1;

    const unsigned int MAX_QUAD = 1000;
    GLint g_gui_num = 0;
    GLfloat g_gui_vertexs[MAX_QUAD * 4 * 3];
    GLfloat g_gui_colors[MAX_QUAD * 4 * 3];
    GLint g_gui_indices[MAX_QUAD * 4];

    std::vector<Window*> renderWins;

    //===========================================================
    //
    inline int check_to_x(int x)
    {
        return x - g_gui_half_width;
    }

    //===========================================================
    //
    inline int check_to_y(int y)
    {
        return g_gui_half_height - y;
    }

    //===========================================================
    //
    inline void store_rect(int i, float x, float y, float w, float h)
    {
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

    //===========================================================
    //
    inline void stre_indice(int i)
    {
        int ii = i * 4;
        g_gui_indices[ii] = ii;
        g_gui_indices[ii + 1] = ii + 1;
        g_gui_indices[ii + 2] = ii + 2;
        g_gui_indices[ii + 3] = ii + 3;
    }

    //===========================================================
    //
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

    //===========================================================
    //
    void rdSetView(float w, float h)
    {
        g_gui_width = w;
        g_gui_height = h;
        g_gui_ar = (float) w / (float) h;
        g_gui_half_width = w / 2.0;
        g_gui_half_height = h / 2.0;
    }

    //===========================================================
    //
    void rdAddRectangle(float x, float y, float w, float h, float r, float g, float b)
    {
        assert(g_gui_num < MAX_QUAD);
        store_rect(g_gui_num, x, y, w, h);
        stre_indice(g_gui_num);
        store_color(g_gui_num, r, g, b);
        g_gui_num++;
    }

    //===========================================================
    //
    void rdUpdate()
    {
        std::vector<Window*>::iterator i = renderWins.begin();
            for (; i != renderWins.end(); ++i)
                (*i)->draw();
    }

    //===========================================================
    //
    void rdBegin()
    {
        //save current attributes
		glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glPolygonMode(GL_FRONT, GL_FILL);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		//glOrtho(-g_gui_half_width * g_gui_ar, g_gui_half_width * g_gui_ar, -g_gui_half_height, g_gui_half_height, -1, 1);
		glOrtho(-g_gui_half_width, g_gui_half_width, -g_gui_half_height, g_gui_half_height, -1, 1);

		//glOrtho(-1, 1, -1, 1, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_FOG);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);

		//glFrontFace(GL_CW);
		//glCullFace(GL_BACK);
		//glEnable(GL_CULL_FACE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_TEXTURE_2D);
    }

    //===========================================================
    //
    void rdRender()
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
        g_gui_num = 0;
    }

    //===========================================================
    //
    void rdEnd()
    {
        glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		//restore former attributes
		glPopAttrib();
		glPopClientAttrib();
    }

    //===========================================================
    //
    void rdFrameQueue()
    {
        rdUpdate();
        rdBegin();
        rdRender();
        rdEnd();
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
        mrdSize.w = w;
        mrdSize.h = h;
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
        if (x < mPosition.x || x > mPosition.x + mrdSize.w)
            return false;
        if (y < mPosition.y || y > mPosition.y + mrdSize.h)
            return false;

        printf("hitTest true %f, %f\n", x, y);
        return true;
    }

    //===========================================================
    //
    int Window::mouseEvent(int button, int state, int x, int y)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (state == GLFW_PRESS)
                mouseDown(x, y);
            else if (state == GLFW_RELEASE)
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
        rdAddRectangle(mPosition.x + x, mPosition.y + y, mrdSize.w, mrdSize.h, mColor.r, mColor.g, mColor.b);
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
    Label::Label():Window(0, 0, 1, 1){}
    //===========================================================
    //
    Label::Label(const std::string& str)
    : Window(0, 0, 1, 1), mString(str)
    {
    }

    //===========================================================
    // 这里无法叠在一起只有自己画了
    void Label::draw(float x, float y)
    {
        int sz = mString.size();
        const char* dt = mString.c_str();
        glPushMatrix();
        glTranslatef(x, y, 0);
        glScalef(0.1, 0.1, 0.1);
        glColor3f(mColor.r, mColor.g, mColor.b);
        //for (int i = 0; i < sz; ++i)
            //glutStrokeCharacter(GLUT_STROKE_ROMAN, dt[i]);
        glPopMatrix();
    }
    //===========================================================
    //

    Slider::Slider()
    : Window(0, 0, 1, 1)
    {
        mBar = new Window(0, 0, mrdSize.w, 1);
        mBar->setColor(rdColor(100, 255, 255));
        addChild(mBar);
    }
    //===========================================================
    //
    Slider::Slider(float x, float y, float w, float h):Window(x, y, w, h)
    {
        mBar = new Window(0, 0, mrdSize.w, 1);
        mBar->setColor(rdColor(100, 255, 255));
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

}
