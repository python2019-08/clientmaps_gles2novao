
 
#if MBGL_USE_GLES2
#define GLFW_INCLUDE_ES2
#endif // MBGL_USE_GLES2
#include <glad/glad.h> 

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>



#include <ctime>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <utility>
#include "util/logtool.hpp"
#include "md_map.h"  

#include "glfw_view.hpp"


using namespace clientmap;

void glfwError(int error, const char *description) 
{
    Log::Error( "GLFW error (%i): %s", error, description);
    assert(false);
}

GLFWView::GLFWView(bool fullscreen_ )
    : _fullscreen(fullscreen_) 
{
 
}

GLFWView::~GLFWView() {
    done();
}
   
int GLFWView::init()
{
    glfwSetErrorCallback(glfwError);

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    if (!glfwInit()) {
        Log::Error("failed to initialize glfw");
        exit(1);
    }

    GLFWmonitor *monitor = nullptr;
    if (_fullscreen) {
        monitor = glfwGetPrimaryMonitor();
        auto videoMode = glfwGetVideoMode(monitor);
        _width = videoMode->width;
        _height = videoMode->height;
    }
      
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0); 
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);//主版本号
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);//次版本号
 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 16);

    _window = glfwCreateWindow(_width, _height, 
                        "CliMaps:", monitor, nullptr);
    if (!_window) {
        glfwTerminate();
        Log::Error("failed to initialize window");
        exit(1);
    }

    glfwSetWindowUserPointer(_window, this);
    glfwSetCursorPosCallback(_window, onMouseMove);
    glfwSetMouseButtonCallback(_window, onMouseClick);
    glfwSetWindowSizeCallback(_window, onWindowResize);
    glfwSetFramebufferSizeCallback(_window, onFramebufferResize);
    glfwSetScrollCallback(_window, onScroll);
    glfwSetKeyCallback(_window, onKey);
    glfwSetWindowFocusCallback(_window, onWindowFocus);

    glfwGetWindowSize(_window, &_width, &_height);

    glfwMakeContextCurrent(_window);//绑定了Window
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "GLAD没能初始化成功。" << std::endl;
        return -1;
    }
    //------------------------------------------------
    // create CMdEngine object and initialize it
    //------------------------------------------------
    m_mdMap = std::make_unique<CMDMap>(this);
    std::string rootDir = "../../";
    m_mdMap->init(rootDir, _width,  _height);
    //------------------------------------------------
    printf("\n");
    printf("================================================================================\n");
    printf("\n");  
    printf("- Press `A` to move left\n");
    printf("- Press `D` to move right\n");
    printf("- Press `W` to move up\n");
    printf("- Press `S` to move down\n");
    printf("- Press `Q` to counterclockwise rotation\n");
    printf("- Press `R` to clockwise rotation\n");
    printf("- Press `Pageup`   to pitch +\n");
    printf("- Press `Pagedown` to pitch -\n");
    printf("- Press `Home` to teturn to original state\n");
    printf("- Press `Esc` to quit\n");
    printf("\n");
    printf("-  mouse drag to move map\n");
    printf("-  mouse wheel to scale map\n");
    printf("\n");
    printf("================================================================================\n");
    printf("\n");
}



void GLFWView::done()
{
    glfwDestroyWindow(_window);
    
    printf("----");

    glfwTerminate();
}

void GLFWView::setShouldClose() {
    glfwSetWindowShouldClose(_window, true);
    glfwPostEmptyEvent();
}

void GLFWView::setWindowTitle(const std::string& title)
{
    glfwSetWindowTitle(_window, (std::string{ "CliMaps: " } +title).c_str());
}


void GLFWView::onKey(GLFWwindow *window, 
    int key, 
    int /*scancode*/, 
    int action, 
    int mods) 
{
    auto *view = reinterpret_cast<GLFWView *>(
        glfwGetWindowUserPointer(window)
        );
    //Log::Debug("GLFWView::onKey,action=%d", action);
    if (action == GLFW_RELEASE) 
    { 
        switch (key) 
        {
            case GLFW_KEY_ESCAPE: {
                glfwSetWindowShouldClose(window, true);
            } break;  
            case GLFW_KEY_A: {
                glm::vec2 delta(-1, 0);
                view->m_mdMap->moveBy(delta);
            } break;
            case GLFW_KEY_D: {
                glm::vec2 delta(1, 0);
                view->m_mdMap->moveBy(delta);
            } break;
            case GLFW_KEY_W: {
                glm::vec2 delta(0, -1);
                view->m_mdMap->moveBy(delta);
            } break; 
            case GLFW_KEY_S: {
                glm::vec2 delta(0, 1);
                view->m_mdMap->moveBy(delta);
            } break; 
            case GLFW_KEY_Q: {
                view->m_mdMap->rotateBy(-1);
            } break;
            case GLFW_KEY_R: {
                view->m_mdMap->rotateBy(1);
            } break;    
            case GLFW_KEY_PAGE_UP: {
                view->m_mdMap->pitchBy(-1);
            } break;
            case GLFW_KEY_PAGE_DOWN: {
                view->m_mdMap->pitchBy(1);
            } break; 
            case GLFW_KEY_HOME: {
                glm::vec2  ct(view->_width*0.5f, view->_height*0.5f);
                view->m_mdMap->jumpTo(
                    CameraOptions().withCenter(ct)
                    .withZoom(3).withPitch(0).withRoll(0)  );
            } break;
        }
    }

    if (action == GLFW_RELEASE || action == GLFW_REPEAT) 
    {
        switch (key) {
        case GLFW_KEY_W: {

        } break;
        case GLFW_KEY_1: {

        } break;
        case GLFW_KEY_2: {

        } break;  
        case GLFW_KEY_M: {

        } break;
        }
    }
}
      
void GLFWView::onScroll(GLFWwindow *window, 
                        double /*xOffset*/, 
                        double yOffset) 
{
    //Log::Debug("GLFWView::onScroll");
    auto *view = reinterpret_cast<GLFWView *>(
        glfwGetWindowUserPointer(window)
        );
    double delta = yOffset * 40;

    bool isWheel = delta != 0 && std::fmod(delta, 4.000244140625) == 0;

    double absDelta = delta < 0 ? -delta : delta;
    double scale = 2.0 / (1.0 + std::exp(-absDelta / 100.0));

    // Make the scroll wheel a bit slower.
    if (!isWheel) {
        scale = (scale - 1.0) / 2.0 + 1.0;
    }

    // Zooming out.
    if (delta < 0 && scale != 0) {
        scale = 1.0 / scale;
    }
    double sign = yOffset > 0 ? 1 :- 1;
    view->m_mdMap->zoomBy(scale *sign);
 
}

void GLFWView::onMouseClick(GLFWwindow *window, 
    int button, 
    int action, 
    int modifiers) 
{
    //Log::Debug("GLFWView::onMouseClick");
    auto *view = reinterpret_cast<GLFWView *>(
        glfwGetWindowUserPointer(window)  );

    if (button == GLFW_MOUSE_BUTTON_RIGHT ||
        (button == GLFW_MOUSE_BUTTON_LEFT && modifiers & GLFW_MOD_CONTROL)) 
    {
        view->_rotating = action == GLFW_PRESS;
        //view->m_mdMap->setGestureInProgress(view->_rotating);
    } 
    else if (button == GLFW_MOUSE_BUTTON_LEFT && (modifiers & GLFW_MOD_SHIFT)) 
    {
        view->_pitching = action == GLFW_PRESS;
        //view->m_mdMap->setGestureInProgress(view->_pitching);
    } 
    else if (button == GLFW_MOUSE_BUTTON_LEFT) 
    {
        view->_tracking = action == GLFW_PRESS;
        //view->m_mdMap->setGestureInProgress(view->_tracking);

        //if (action == GLFW_RELEASE) 
        //{
        //    double now = glfwGetTime();
        //    if (now - view->_lastClick < 0.4 /* ms */) {
        //        if (modifiers & GLFW_MOD_SHIFT) {
        //            view->map->scaleBy(0.5);
        //        } else {
        //            view->map->scaleBy(2.0 );
        //        }
        //    }
        //    view->_lastClick = now;
        //}
    }
}

void GLFWView::onMouseMove(GLFWwindow *window, double x, double y) 
{
    //Log::Debug("GLFWView::onMouseMove");
    auto* view = reinterpret_cast<GLFWView *>(
                              glfwGetWindowUserPointer(window)  );
    if (view->_tracking) 
    {
        const double dx = x - view->_lastX;
        const double dy = y - view->_lastY;
        if (dx || dy) {
            view->m_mdMap->moveBy(glm::vec2{ dx, -dy });
        }
    } else if (view->_rotating) {
        //view->m_mdMap->rotateBy({ view->_lastX, view->_lastY }, { x, y });
    } else if (view->_pitching) {
        const double dy = y - view->_lastY;
        if (dy) {
            //view->m_mdMap->pitchBy(dy / 2);
        }
    }
    view->_lastX = x;
    view->_lastY = y;
 
   
    view->triggerDraw(); 
}

void GLFWView::onWindowFocus(GLFWwindow *window, int focused) 
{
    //Log::Debug("GLFWView::onWindowFocus");
    if (focused == GLFW_FALSE) { // Focus lost.
        auto *view = reinterpret_cast<GLFWView *>(
            glfwGetWindowUserPointer(window));
        //view->rendererFrontend->getRenderer()->reduceMemoryUse();
    }
}


void GLFWView::triggerDraw() {
    glfwPostEmptyEvent();
}
  

void GLFWView::onWindowResize(GLFWwindow *window, int width, int height)
{
    //Log::Debug("GLFWView::onWindowResize");
    auto *view = reinterpret_cast<GLFWView *>(glfwGetWindowUserPointer(window));
    view->_width = width;
    view->_height = height;
    view->m_mdMap->setViewSize( view->_width, view->_height );
}

void GLFWView::onFramebufferResize(GLFWwindow *window, 
    int width, 
    int height
    )
{
    //Log::Debug("GLFWView::onFramebufferResize");
    auto *view = reinterpret_cast<GLFWView *>(glfwGetWindowUserPointer(window));
    //view->backend->setSize({ static_cast<uint32_t>(_width), 
    //                         static_cast<uint32_t>(_height) 
    //                         });

    // This is only triggered when the framebuffer is resized,
    // but not the window. It can
    // happen when you move the window between screens with a 
    // different pixel ratio.
    // We are forcing a repaint my invalidating the view, 
    // which triggers a rerender with the
    // new framebuffer dimensions.
    view->triggerDraw();
}


void GLFWView::run()
{ 
    while (!glfwWindowShouldClose(_window)) 
    { 
        m_mdMap->drawFrame();

        glfwPollEvents();
        glfwSwapBuffers(_window);
    }
     
}


 