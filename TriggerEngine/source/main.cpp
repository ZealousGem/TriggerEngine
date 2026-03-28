// TriggerEngine.cpp : Defines the entry point for the application.
//

#include "main.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

using namespace std;

// Double Buffering - is an api technique that prevents visual flickering and artifacts during rendering

// front buffer - is the buffer that is currently being displayed on the screen

// back buffer - is the buffer that is being drawn to in the background

// objects are rendered in 3d space defined by an axis using either the right hand rule(points at you) or the left hand rule(points away from you)

// all models consitst of primitive points, lines and triangles

// normalised coordinates - are coordinates that are between -1 and 1, where the center of the screen is (0, 0) and the corners are (-1, -1), (1, -1), (-1, 1) and (1, 1)

// vertex order - is the order in which the vertices of a triangle are defined, which determines the front face of the triangle and how it is rendered

// winding order of vertices - is the order in which the vertices of a triangle are defined, which determines the front face of the triangle and how it is rendered,
// Opengl uses Counter Clock Wise (CCW) winding order by default, which means that if the vertices of a triangle are defined in a counter clockwise order, the triangle will be rendered as front facing, and if they are defined in a clockwise order, the triangle will be rendered as back facing

int main()
{
	// checks if library is initialized successfully
	if (!glfwInit()) return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "TriggerEngine", nullptr, nullptr);

	if (window == nullptr) 
	{
		cout << "Windpw os null or has errors";
		glfwTerminate();
		return -1;

	}

	//runs on second monitor
	//glfwSetWindowPos(window, 2000, 150);

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;

	// Clear any GL error that can be produced by GLEW on core profiles
	glGetError();
	GLenum glewStatus = glewInit();
	if (glewStatus != GLEW_OK)
	{
		glfwTerminate();
		cout << "error with the opengl api";
		return -1;
	}

	// counter clock wise order of vertices, to render a triangle 
	vector<float> vertices = {
		  0.0f, 0.5f, 0.0f, // top
		  -0.5f, -0.5f, 0.0f, // left
		  0.0f, - 0.5f, 0.0f  // right
	};
	

	while (!glfwWindowShouldClose(window))
	{

	  // back buffer
	  glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	  /// handles the colour of thw windw by rendering the coulr using opengl api
	  glClear(GL_COLOR_BUFFER_BIT);

	  // font buffer
	  glfwSwapBuffers(window);
	  glfwPollEvents();
	} 
	
	glfwTerminate();

	return 0;
}
