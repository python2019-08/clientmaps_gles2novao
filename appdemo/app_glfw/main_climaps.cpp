#if  0
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include<windows.h>
int WINAPI MessageBox(  HWND hWnd, 
    LPCTSTR lpText, 
    LPCTSTR lpCaption, 
    UINT uType);

// 对窗口注册一个回调函数(Callback Function)，
// 它会在每次窗口大小被调整的时候被调用
void framebuffer_size_callback(GLFWwindow* window, 
    int width, int height);

// 这个函数将会返回这个按键是否正在被按下。我们将创建一个
// processInput函数来让所有的输入代码保持整洁。 
void processInput(GLFWwindow *window);
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 通过glfwSetwindowShouldClose使用把WindowShouldClose
// 属性设置为true的方法关闭GLFW。 
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
    {
        if (MessageBox(NULL,  
            L"你真的要抛弃我吗 我可是你第一个孩子。",
            L"询问：", MB_YESNO) == IDYES
            ) 
        {
            glfwSetWindowShouldClose(window, true);
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
    /* 
       这个帧缓冲大小函数需要一个GLFWwindow作为它的第一个参数，
       以及两个整数表示窗口的新维度。
       每当窗口改变大小，GLFW会调用这个函数并填充相应的参数供你处理。
      */
    glViewport(0, 0, width, height);
    //尝试在命令行窗口输出宽度与高度
    std::cout << "当前的窗口大小：" 
        << "宽度:  " << width 
        << "    高度:  " << height << std::endl;
}

int main(int argc, char *argv[])
{
    glfwInit(); //初始化GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);//主版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);//次版本号
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//使用核心模式

#ifdef __APPLE__
    // 如果是MAC，需要这一行
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, 
                        "HelloWorld", NULL, NULL); //创建窗口

    if (window == NULL) {//如果失败了的话
        // 命令行提示创建失败
        std::cout << "真是太失败了，你没能创建窗口成功。" << std::endl; 
        // 退出
        glfwTerminate(); 
        return -1;
    }
    glfwMakeContextCurrent(window);//绑定了Window
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);//我们还需要注册这个函数，告诉GLFW我们希望每当窗口调整大小的时候调用这个函数：

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "真是太失败了，GLAD没能初始化成功。" << std::endl;
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        //输入控制
        processInput(window);

        //经过30秒的思考，我终于看懂了这是个什么鬼：这不就是rgba嘛!
        //你看哦，最后一个值是1！
        //说明a在后面
        //如果在前面那就是argb(GDI+_32)那种了。
        glClearColor(0.9f, 0.1f, 0.1f, 1.0f);
        //渲染这个颜色
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate(); //跳出循环后就结束进程 .
    return 0;
}
#else
#include "glfw_view.hpp"

int main(int argc, char *argv[])
{
    clientmap::GLFWView  glfwView(false);

    glfwView.init();

    glfwView.run();
}

#endif