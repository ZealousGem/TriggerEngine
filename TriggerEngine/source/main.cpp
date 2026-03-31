// TriggerEngine.cpp : Defines the entry point for the application.
//

#include "main.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <string>

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


	// tells compiler to use opengl version 3.3 core profile
	// declares a vertex shader that takes in a vec3 attribute called aPos, which represents the position of a vertex in 3D space
	// main function converges the 3D position of the vertex to a 4D homogeneous coordinate by adding a fourth component with a value of 1.0, which is necessary for rendering in OpenGL
	string VertexShader = R"(
         #version 330 core 
         layout(location = 0) in vec3 aPos; 
         layout(location = 1) in vec3 aColor;

         out vec3 ourColor;

          void main()
		 {
			 ourColor = aColor;
			 gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
		 }
    )";

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); // creates a vertex shader object and returns its ID, which can be used to reference the shader in subsequent OpenGL calls
	const char* vertexShaderCStr = VertexShader.c_str(); // converts the vertex shader source code from a C++ string to a C-style string (const char*) that can be passed to OpenGL functions
	glShaderSource(vertexShader, 1, &vertexShaderCStr, nullptr); // sets the source code of the vertex shader object to the provided string, where vertexShader is the ID of the shader object, 1 indicates that there is one string being passed, &vertexShaderCStr is a pointer to the string containing the shader source code, and nullptr indicates that the length of the string is determined by the null terminator

	glCompileShader(vertexShader);

	GLint Sucess; 
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &Sucess);

	if (!Sucess) 
	{
		char infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		cerr << "Error compiling vertex shader: " << infoLog << endl;
	}



	// declares a fragment shader that outputs a vec4 color value, which represents the color of a pixel on the screen
	string FragmentShader = R"(
		 #version 330 core 
		 out vec4 FragColor; 

         in vec3 ourColor;
          
		  void main()
		 {
			 FragColor = vec4(ourColor, 1.0);
		 }
	)";

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); 
	// creates a fragment shader object and returns its ID, which can be used to reference the shader in subsequent OpenGL calls
	const char* FragemntShaderSource = FragmentShader.c_str();
	glShaderSource(fragmentShader, 1, &FragemntShaderSource, nullptr);
	glCompileShader(fragmentShader);


    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &Sucess);

	if (!Sucess)
	{
		char infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		cerr << "Error compiling fragment shader: " << infoLog << endl;
	}

    
	GLuint shadeprogram = glCreateProgram();
	glAttachShader(shadeprogram, vertexShader);
	glAttachShader(shadeprogram, fragmentShader);
	glLinkProgram(shadeprogram);

	glGetProgramiv(shadeprogram, GL_LINK_STATUS, &Sucess);
	if (!Sucess)
	{
		char infoLog[512];
		glGetProgramInfoLog(shadeprogram, 512, nullptr, infoLog);
		cerr << "Error in Shader program linking failed " << infoLog << endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// counter clock wise order of vertices, to render a triangle 
	vector<float> vertices = {
		  0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // top
		  -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // left
		  0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f  // right
	};
	
	GLuint vbo;
	glGenBuffers(1, &vbo); // generates a buffer object and returns its ID, which can be used to reference the buffer in subsequent OpenGL calls
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // binds the buffer object to the GL_ARRAY_BUFFER target, which indicates that the buffer will be used for vertex attribute data
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW); // creates and initializes the buffer object's data store with the vertex data, where GL_ARRAY_BUFFER is the target to which the buffer is bound, vertices.size() * sizeof(float) specifies the size of the data in bytes, vertices.data() is a pointer to the vertex data, and GL_STATIC_DRAW indicates that the data will be used for drawing and will not be modified frequently
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint vao; // generates a vertex array object and returns its ID, which can be used to reference the vertex array in subsequent OpenGL calls
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao); // binds the vertex array object, which allows you to store the state of vertex attribute configurations and buffer bindings
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // binds the vertex buffer object to the GL_ARRAY_BUFFER target, which indicates that the buffer will be used for vertex attribute data

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)0); // defines an array of vertex attribute data, where 0 is the index of the vertex attribute, 3 is the number of components per vertex attribute (in this case, x, y, z), GL_FLOAT is the data type of each component, false indicates that the data should not be normalized, 3 * sizeof(float) specifies the byte offset between consecutive vertex attributes (the stride), and (void*)0 indicates that the vertex attribute data starts at the beginning of the buffer
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)(3 * sizeof(float))); // defines an array of vertex attribute data for the color attribute, where 1 is the index of the vertex attribute, 3 is the number of components per vertex attribute (in this case, r, g, b), GL_FLOAT is the data type of each component, false indicates that the data should not be normalized, 6 * sizeof(float) specifies the byte offset between consecutive vertex attributes (the stride), and (void*)(3 * sizeof(float)) indicates that the vertex attribute data starts at an offset of 3 floats from the beginning of the buffer (after the position data)
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	while (!glfwWindowShouldClose(window))
	{

	  // back buffer
	  glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	  /// handles the colour of thw windw by rendering the coulr using opengl api
	  glClear(GL_COLOR_BUFFER_BIT);

	  glUseProgram(shadeprogram);
	  glBindVertexArray(vao);
	  glDrawArrays(GL_TRIANGLES, 0, 3); // renders primitives from array data, where GL_TRIANGLES specifies the mode (in this case, triangles), 0 is the starting index in the enabled arrays, and 3 is the number of vertices to be rendered (in this case, one triangle)

	  // font buffer
	  glfwSwapBuffers(window);
	  glfwPollEvents();
	} 
	
	glfwTerminate();

	return 0;
}
