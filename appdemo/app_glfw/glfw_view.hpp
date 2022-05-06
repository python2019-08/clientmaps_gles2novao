#pragma once
 
#include <utility>
#include <string>
#include <memory>
#include "map/drawdevice.h"

struct GLFWwindow;
class GLFWBackend;
class GLFWRendererFrontend;
class SnapshotObserver;
class CMdEngine;
 
namespace clientmap {
class CMDMap;


class GLFWView: public IDrawDevice {
public:
    GLFWView(bool fullscreen );
    ~GLFWView();
     
    int init();
    void done();
    void setShouldClose();

    void setWindowTitle(const std::string&);

    void run();
 

    // 实现 接口md::IRenderDevice的虚函数
    bool activate() override {
        return true;
    }
    void deactivate()override {}
    void triggerDraw()override;
protected: 

private:
    // Window callbacks
    static void onKey(GLFWwindow *window, 
        int key, 
        int scancode, 
        int action, 
        int mods);
    static void onScroll(GLFWwindow *window, 
        double xoffset, 
        double yoffset);
    static void onWindowResize(GLFWwindow *window, 
        int width, 
        int height);
    static void onFramebufferResize(GLFWwindow *window, 
        int width, 
        int height);
    static void onMouseClick(GLFWwindow *window, 
        int button, 
        int action, 
        int modifiers);
    static void onMouseMove(GLFWwindow *window, 
        double x, 
        double y);
    static void onWindowFocus(GLFWwindow *window, 
        int focused);     
private:
    bool _fullscreen = false; 

    int _width = 1024;
    int _height = 768; 

    bool _tracking = false;
    bool _rotating = false;
    bool _pitching = false;

    double _lastX = 0;
    double _lastY = 0;
    double _lastClick = -1;

    GLFWwindow* _window = nullptr;
    std::unique_ptr<CMDMap>  m_mdMap;
};
}//END OF namespace clientmap 