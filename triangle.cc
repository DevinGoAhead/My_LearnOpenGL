#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <windows.h>

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>


// 错误回调函数
static void error_callback(int error, const char *description)
{
	fprintf(stderr, "ERROR: %d | %s\n", error, description);
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

// 检查编译状态
void isCompileSuccess(GLuint shader)
{
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success); // 获取编译状态

	if (!success) // 如果编译失败
	{
		char log[512];
		glGetShaderInfoLog(shader, sizeof(log) - 1, NULL, log);
		std::cout << "ERROR::SHADER::COMPILE_FAILED\n"
				  << std::endl;
		exit(EXIT_FAILURE);
	}
}

void isLinkSuccess(GLuint program)
{
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success); // 获取编译状态

	if (!success) // 如果编译失败
	{
		char log[512];
		glGetProgramInfoLog(program, sizeof(log) - 1, NULL, log);
		std::cout << "ERROR::SHADER::LINK_FAILED\n"
				  << std::endl;
		exit(EXIT_FAILURE);
	}
}

// v2 生成一个随机数种子
GLfloat RandFrom0to1()
	{
		srand(time(NULL) | GetCurrentProcessId());//生成一个随机数种子
		GLfloat time = glfwGetTime();
		Sleep(100);//这里为了让下一个随机数种子有时间产生一定的变化
		return (sin((int)time | rand()) / 2 + 0.5); // sin() / 2  [-0.5, +0.5] 
	}

// 顶点着色器源码
// v2 不需要 顶点着色器传递给 片段着色器颜色属性
const char *vertexShaderSource = R"(
    #version 330 core // 声明版本
    layout (location = 0) in vec3 position; // 输入变量：顶点坐标，索引设置为0
    void main() {
        gl_Position = vec4(position, 1.0); //齐次坐标，openGL内建变量，表示点在裁剪空间的位置，本例给出NDC内坐标，避免复杂转换
    }
)";

// 片段着色器源码
// v2 直接从 uniform 变量获取颜色
const char *fragmentShaderSource = R"(
    #version 330 core
	uniform vec4 fragColor; //uniform 变量：颜色
    out vec4 color; // 输出变量，作为下一阶段的输入变量
    void main() {
        color = fragColor; // 颜色处理
    }
)";

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

	// v2
	GLFWwindow *pWindow = glfwCreateWindow(800, 600, "Triangle-Single Color", NULL, NULL);
	glfwMaximizeWindow(pWindow); // 手动最大化
	if (!pWindow)				 // 如果创建显示器失败
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

	/*创建并编译顶点着色器对象*/

	// 创建 参数 GL_SHADER_TYPE == GL_VERTEX_SHADER 的着色器对象，即顶点着色器对象
	// vertexShader 是着色器对象的 ID（引用）
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// 覆盖 着色器对象中 旧的源码
	// 目标着色器 字符串个数 字符串数组指针
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

	glCompileShader(vertexShader); // 编译着色器对象（源码），编译状态会被保存在着色器对象中

	// 检查编译状态
	isCompileSuccess(vertexShader);

	/*创建并编译片段着色器对象*/

	// 创建 参数 GL_SHADER_TYPE == GL_FRAGMENT_SHADER 的着色器对象，即片段着色器对象
	// fragmentShader 是着色器对象的 ID（引用）
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// 覆盖 着色器对象中 旧的源码
	// 目标着色器 字符串个数 字符串数组指针
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

	glCompileShader(fragmentShader); // 编译着色器对象（源码），编译状态会被保存在着色器对象中

	isCompileSuccess(fragmentShader); // 检查编译是否成功

	/*创建程序对象并链接着色器*/

	GLuint shaderProgram = glCreateProgram();	   // 创建空的着色程序对象
	glAttachShader(shaderProgram, vertexShader);   // attach 顶点着色器
	glAttachShader(shaderProgram, fragmentShader); // attach 片段着色器
	glad_glLinkProgram(shaderProgram);			   // 链接
	isLinkSuccess(shaderProgram);				   // 检查链接是否成功

	// v1 创建顶点数据,4个三角形
	//          ------
	//          | /\ |
	//          |/  \|
	//          |\  /|
	//          | \/ |
	//          ------
	//v2 顶点数据中新增了顶点颜色
	GLfloat vertics[] =
		{
			0.0f, 0.3f, 0.0f, // 0
			-0.2f, 0.3f, 0.0f, // 1
			-0.2f, 0.0f, 0.0f, // 2

			// 0.0f, 0.3f, 0.0f, // 0
			0.2f, 0.3f, 0.0f, // 3
			0.2f, 0.0f, 0.0f, // 4

			0.0f, -0.3f, 0.0f, // 5
			-0.2f, -0.3f, 0.0f, // 6
			//-0.2f, 0.0f,  0.0f, // 2

			// 0.0f, -0.3f, 0.0f, // 5
			0.2f, -0.3f, 0.0f, // 7
			// 0.2f,  0.0f, 0.0f, // 4
		};

	// 创建顶点索引数据
	GLuint indeies[] =
		{
			0, 1, 2,
			0, 3, 4,
			5, 6, 2,
			5, 7, 4};
	
	/*配置顶点属性*/
	// 创建VBO、EBO、VAO
	GLuint verticBuffer;  // 用于存储VBO，如果是多个缓冲区对象，就创建一个数组
	GLuint elementBuffer; // 用于存储EBO，如果是多个缓冲区对象，就创建一个数组
	GLuint verticArray;	  // 用于存储VAO，如果是多个缓冲区对象，就创建一个数组

	glGenBuffers(1, &verticBuffer);		// 创建 1 个缓冲对象并放在 verBuffer 中
	glGenBuffers(1, &elementBuffer);	// 创建 1 个缓冲对象并放在 elementBuffer 中
	glGenVertexArrays(1, &verticArray); // 创建 1 个顶点数组对象并放在 verticArray 中

	glBindVertexArray(verticArray); // 将 verticArray 绑定到openGL 上下文，下面的信息将会被记录在 verticArray 中

	// 将 verBuffer 缓冲对象标记为 GL_ARRAY_BUFFER，表示它用于存储顶点属性数据
	glBindBuffer(GL_ARRAY_BUFFER, verticBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertics), vertics, GL_STATIC_DRAW); // 在GPU中分配内存，存储顶点数据

	// 将 elementBuffer 缓冲对象标记为 GL_ELEMENT_ARRAY_BUFFER，表示它用于存储顶点索引数据
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indeies), indeies, GL_STATIC_DRAW); // 在GPU中分配内存，存储索引数据

	//设置顶点属性指针,告知GPU如何解析顶点数据
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const GLvoid *)0); // 如何解析位置坐标
	glEnableVertexAttribArray(0);

	glBindVertexArray(verticArray);// 将 verticArray 从 openGL 上下文解绑，之后的信息将不会记录
	
	//使 透明值 生效
	glEnable(GL_BLEND);//启用颜色混合操作功能
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//设置颜色混合模式

	
	
	/*主循环*/
	glfwSwapInterval(1); // 设置 渲染完成 1 帧，交换一次前后缓冲
	while (!glfwWindowShouldClose(pWindow))
	{
		glClearColor(0.3f, 0.5f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT); // 仅清除颜色缓冲区
		
		glUseProgram(shaderProgram);//激活着色程序
		GLuint location = glGetUniformLocation(shaderProgram, "fragColor");//必须在激活程序之后，因为 fragColor 在程序 shaderProgram 中
		
		// 这里给一个动态变化的颜色
		glUniform4f(location, RandFrom0to1(), RandFrom0to1(), RandFrom0to1(), RandFrom0to1());

		glBindVertexArray(verticArray);//第二次绑定同一个 VAO 时，OpenGL 会使用这个 VAO 中记录的所有配置信息来进行绘制操作
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, (void*)0);
		glfwSwapBuffers(pWindow);
		glfwPollEvents();
	}

	/*资源清理*/
	glDeleteBuffers(1, &verticBuffer);
	glDeleteBuffers(1, &elementBuffer);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &verticArray);

	// glfwTerminate();//终止GLFW。放在这里会出问题，应为 glfwDestroyWindow 还会使用 GLFW 库
	glfwDestroyWindow(pWindow); // 销毁窗口
	glfwTerminate();			// 终止GLFW
	exit(EXIT_SUCCESS);

	return 0;
}