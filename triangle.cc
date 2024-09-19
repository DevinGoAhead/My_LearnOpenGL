#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <iostream>

// 错误回调函数
static void error_callback(int error, const char *description)
{
	-fprintf(stderr, "ERROR: %s\n", description);
}

// 视口尺寸自动调整回调函数
static void frame_buffersize_callback(GLFWwindow *pWindow, int windWidth, int windLength)
{
	glViewport(0, 0, windWidth, windLength);
}

// 视口尺寸自动调整回调函数
static void key_callback(GLFWwindow *pWindow, int key, int scancode, int action, int modes)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
		glfwSetWindowShouldClose(pWindow, GL_TRUE);
}

int main()
{
	/*注册 错误回调函数*/
	glfwSetErrorCallback(error_callback);

	/*初始化GLFW*/
	if (!glfwInit()) // 如果初始化失败
		exit(EXIT_FAILURE);

	/*设定openGL 兼容版本及配置*/
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/*创建窗口*/
	// //v1 这样的方式不显示标题栏、边框
	// GLFWmonitor *priMonitor = glfwGetPrimaryMonitor();		   // 返回指向主显示器的指针
	// const GLFWvidmode *vidMode = glfwGetVideoMode(priMonitor); // 返回 priMonitor 的视频模式

	// GLFWwindow *pWindow = glfwCreateWindow(vidMode->width, vidMode->height,
	// 									   "Triangle-Single Color", priMonitor, NULL);

	//v2 
	GLFWwindow *pWindow = glfwCreateWindow(800, 600, "Triangle-Single Color", NULL, NULL);
	glfwMaximizeWindow(pWindow);//手动最大化
	if (!pWindow) // 如果创建显示器失败
	{
		glfwTerminate(); // 终止GLFW
		exit(EXIT_FAILURE);
	}

	/*设置当前pWindow 为当前上下文*/
	glfwMakeContextCurrent(pWindow);

	/* 初始化GLAD*/
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "faild to init GLAD" << std::endl;
		glfwTerminate(); // 终止GLFW
		exit(EXIT_FAILURE);
	}

	/*视口*/
	int windWidth, windLength;
	glfwGetFramebufferSize(pWindow, &windWidth, &windLength); // 窗口内容区域尺寸
	glViewport(0, 0, windWidth, windLength);

	glfwSetFramebufferSizeCallback(pWindow, frame_buffersize_callback); // 注册视口尺寸自动调整回调函数

	/*注册处理键盘输入回调函数*/
	glfwSetKeyCallback(pWindow, key_callback);

	/*主循环*/
	glfwSwapInterval(1); // 渲染完成 1 帧，交换一次前后缓冲
	while (!glfwWindowShouldClose(pWindow))
	{
		glfwPollEvents();
		glClearColor(0.3f, 0.5f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT); // 仅清除颜色缓冲区
		glfwSwapBuffers(pWindow);
	}

	/*资源清理*/
	// glfwTerminate();//终止GLFW。放在这里会出问题，应为 glfwDestroyWindow 还会使用 GLFW 库
	glfwDestroyWindow(pWindow); // 销毁窗口
	glfwTerminate();			// 终止GLFW
	exit(EXIT_SUCCESS);

	return 0;
}