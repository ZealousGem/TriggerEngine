#include "MiniRenderLab.h"
using namespace std;


struct Vec2 // stores 2d postion, for screenspaced position used for shaders 
{

	float x, y;

	Vec2() : x(0.0f), y(0.0f) {}
	Vec2(float x, float y) : x(x), y(y) {}
};

Vec2 Offset; 

void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	// Paste your keyboard processing code here
}


int main()
{
	if (!glfwInit()) return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "MiniRenderLab", nullptr, nullptr);

	if (window == nullptr)
	{
		cout << "Window is null or has errors";
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, keyCallback);

	glfwMakeContextCurrent(window);

	glGetError();
	GLenum glewStatus = glewInit();
	if (glewStatus != GLEW_OK)
	{
		glfwTerminate();
		cout << "Error with the OpenGL API";
		return -1;
	}

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Paste your code for rendering here

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;

}



