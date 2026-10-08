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
	// shader code and Obejects

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

	vector<float> TriangleVertices = {
	
		// 1st triangle (Far Left: Center -0.7)
	-0.70f,  0.3f, 0.0f,  1.0f, 1.0f, 0.0f, // top
	-0.85f, -0.5f, 0.0f,  1.0f, 1.0f, 0.0f, // left
	-0.55f, -0.5f, 0.0f,  1.0f, 1.0f, 0.0f, // right

	// 2nd triangle (Mid Left: Center -0.35)
	-0.35f,  0.3f, 0.0f,  0.0f, 1.0f, 0.0f, // top
	-0.50f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // left
	-0.20f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // right

	// 3rd triangle (Center: Center 0.0)
	 0.00f,  0.3f, 0.0f,  1.0f, 0.0f, 0.0f, // top
	-0.15f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // left
	 0.15f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // right

	 // 4th triangle (Mid Right: Center +0.35)
	  0.35f,  0.3f, 0.0f,  1.0f, 1.0f, 0.0f, // top
	  0.20f, -0.5f, 0.0f,  1.0f, 0.0f, 1.0f, // left
	  0.50f, -0.5f, 0.0f,  1.0f, 0.0f, 1.0f, // right

	  // 5th triangle (Far Right: Center +0.7)
	   0.70f,  0.3f, 0.0f,  0.0f, 1.0f, 0.0f, // top
	   0.55f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // left
	   0.85f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f  // right
	
	};

	vector<unsigned int> TriangleIndices = {
		0, 1, 2, // first triangle
		3, 4, 5, // second triangle
		6, 7, 8, // third triangle
		9, 10, 11, // fourth triangle
		12, 13, 14 // fifth triangle
	};

	GLuint vbo;
	glGenBuffers(1, &vbo); // generates a buffer object and returns its ID, which can be used to reference the buffer in subsequent OpenGL calls
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // binds the buffer object to the GL_ARRAY_BUFFER target, which indicates that the buffer will be used for vertex attribute data
	glBufferData(GL_ARRAY_BUFFER, TriangleVertices.size() * sizeof(float), TriangleVertices.data(), GL_STATIC_DRAW); // creates and initializes the buffer object's data store with the vertex data, where GL_ARRAY_BUFFER is the target to which the buffer is bound, vertices.size() * sizeof(float) specifies the size of the data in bytes, vertices.data() is a pointer to the vertex data, and GL_STATIC_DRAW indicates that the data will be used for drawing and will not be modified frequently
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint ebo;
	glGenBuffers(1, &ebo); // generates a buffer object for the element buffer and returns its ID
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // binds the element buffer object to the GL_ELEMENT_ARRAY_BUFFER target, which indicates that the buffer will be used for index data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, TriangleIndices.size() * sizeof(unsigned int), TriangleIndices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	GLuint vao; // generates a vertex array object and returns its ID, which can be used to reference the vertex array in subsequent OpenGL calls
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao); // binds the vertex array object, which allows you to store the state of vertex attribute configurations and buffer bindings
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // binds the vertex buffer object to the GL_ARRAY_BUFFER target, which indicates that the buffer will be used for vertex attribute data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)0); // defines an array of vertex attribute data, where 0 is the index of the vertex attribute, 3 is the number of components per vertex attribute (in this case, x, y, z), GL_FLOAT is the data type of each component, false indicates that the data should not be normalized, 3 * sizeof(float) specifies the byte offset between consecutive vertex attributes (the stride), and (void*)0 indicates that the vertex attribute data starts at the beginning of the buffer
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)(3 * sizeof(float))); // defines an array of vertex attribute data for the color attribute, where 1 is the index of the vertex attribute, 3 is the number of components per vertex attribute (in this case, r, g, b), GL_FLOAT is the data type of each component, false indicates that the data should not be normalized, 6 * sizeof(float) specifies the byte offset between consecutive vertex attributes (the stride), and (void*)(3 * sizeof(float)) indicates that the vertex attribute data starts at an offset of 3 floats from the beginning of the buffer (after the position data)
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Paste your code for rendering here
		glUseProgram(shadeprogram);
		glBindVertexArray(vao);

		glDrawElements(GL_TRIANGLES, 15, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;

}



