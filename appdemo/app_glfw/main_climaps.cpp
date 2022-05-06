#if  0
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include<windows.h>
int WINAPI MessageBox(  HWND hWnd, 
    LPCTSTR lpText, 
    LPCTSTR lpCaption, 
    UINT uType);

// �Դ���ע��һ���ص�����(Callback Function)��
// ������ÿ�δ��ڴ�С��������ʱ�򱻵���
void framebuffer_size_callback(GLFWwindow* window, 
    int width, int height);

// ����������᷵����������Ƿ����ڱ����¡����ǽ�����һ��
// processInput�����������е�������뱣�����ࡣ 
void processInput(GLFWwindow *window);
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// ͨ��glfwSetwindowShouldCloseʹ�ð�WindowShouldClose
// ��������Ϊtrue�ķ����ر�GLFW�� 
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
    {
        if (MessageBox(NULL,  
            L"�����Ҫ�������� �ҿ������һ�����ӡ�",
            L"ѯ�ʣ�", MB_YESNO) == IDYES
            ) 
        {
            glfwSetWindowShouldClose(window, true);
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
    /* 
       ���֡�����С������Ҫһ��GLFWwindow��Ϊ���ĵ�һ��������
       �Լ�����������ʾ���ڵ���ά�ȡ�
       ÿ�����ڸı��С��GLFW�������������������Ӧ�Ĳ������㴦��
      */
    glViewport(0, 0, width, height);
    //�����������д�����������߶�
    std::cout << "��ǰ�Ĵ��ڴ�С��" 
        << "���:  " << width 
        << "    �߶�:  " << height << std::endl;
}

int main(int argc, char *argv[])
{
    glfwInit(); //��ʼ��GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);//���汾��
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);//�ΰ汾��
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//ʹ�ú���ģʽ

#ifdef __APPLE__
    // �����MAC����Ҫ��һ��
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, 
                        "HelloWorld", NULL, NULL); //��������

    if (window == NULL) {//���ʧ���˵Ļ�
        // ��������ʾ����ʧ��
        std::cout << "����̫ʧ���ˣ���û�ܴ������ڳɹ���" << std::endl; 
        // �˳�
        glfwTerminate(); 
        return -1;
    }
    glfwMakeContextCurrent(window);//����Window
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);//���ǻ���Ҫע���������������GLFW����ϣ��ÿ�����ڵ�����С��ʱ��������������

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "����̫ʧ���ˣ�GLADû�ܳ�ʼ���ɹ���" << std::endl;
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        //�������
        processInput(window);

        //����30���˼���������ڿ��������Ǹ�ʲô���ⲻ����rgba��!
        //�㿴Ŷ�����һ��ֵ��1��
        //˵��a�ں���
        //�����ǰ���Ǿ���argb(GDI+_32)�����ˡ�
        glClearColor(0.9f, 0.1f, 0.1f, 1.0f);
        //��Ⱦ�����ɫ
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate(); //����ѭ����ͽ������� .
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