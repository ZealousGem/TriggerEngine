#include "SnakeGame.h"
using namespace std;

// vector structures
struct Vec2 // stores 2d postion, for screenspaced position used for shaders 
{
 
	float x, y;

	Vec2() : x(0.0f), y(0.0f) {}
	Vec2(float x, float y) : x(x), y(y) {}
};

struct Vec2i // stores 2d postion, for grid position used for game logic 
{
	int x, y;
	Vec2i() : x(0), y(0) {}
	Vec2i(int x, int y) : x(x), y(y) {}
	bool operator==(const Vec2i& other) const { // to determine if the vec2i components are equal 
		return x == other.x && y == other.y;
	}
};

struct Vec3 // handles RGB colour values , used for shaders
{
	float r, g, b;
	Vec3() : r(0.0f), g(0.0f), b(0.0f) {}
	Vec3(float r, float g, float b) : r(r), g(g), b(b) {}
};

// constants 
const int GRID_SIZE = 20; // defines the size of the game grid, which is a 20x20 grid in this case
const int GRID_HEIGHT = 20; 
const float Update_Interval = 0.2f; // defines the time interval (in seconds) at which the game state should be updated, 
const float CELL_WIDTH = 2.0F / GRID_SIZE;
const float CELL_HEIGHT = 2.0F / GRID_HEIGHT;
// calculates the width and height of each cell in the grid based on the total grid size and the normalized coordinate system used for rendering

// direction player will go
enum class Dir {
	UP, DOWN, LEFT, RIGHT, NONE
};

Dir currentDirection = Dir::NONE; // initializes the current direction of the snake to NONE, indicating that the snake is not moving in any direction at the start of the game
vector<Vec2i> snakeBody{ Vec2i(5,10), Vec2i(4,10), Vec2i(3,10) }; // stores the positions of the snake's body segments, where each Vec2i represents the grid coordinates of a segment
Vec2i fruit; // stores the position of the fruit on the grid, which the snake will try to eat to grow longer and earn points
int score = 0;
bool gameOver = false; 
bool gameStarted = false; 
float timeSinceLastUpdate = 0.0f; // tracks the time elapsed since the last game state update, which is used to control the update frequency of the game logic based on the defined Update_Interval
float SnakeSpeed = Update_Interval;

// shaderSource
string VertexShader = R"(
         #version 330 core 
         layout(location = 0) in vec2 aPos; 

		 uniform vec2 uOffset;
         uniform vec2 uScale; 

         out vec3 ourColor;

          void main()
		 {
			 vec2 scaledPos = (aPos * uScale) + uOffset; // scales the vertex position by multiplying it with the uniform scale factor, which allows for resizing the rendered objects based on the defined cell width and height
			 gl_Position = vec4(scaledPos, 0.0, 1.0);
		 }
    )";

string FragmentShader = R"(
		 #version 330 core 
		 out vec4 FragColor; 
         uniform vec3 uColor;
          
		  void main()
		 {
			 FragColor = vec4(uColor, 1.0);
		 }
	)";

// openGL objects
GLuint shadeprogram;
GLuint vao, vbo;
GLuint uOffsetLoc, uScaleLoc, uColorLoc;

const int FONT_WIDTH = 5;
const int FONT_HEIGHT = 5;
const int FONT_SPACE = 1;

map<char, vector<int>> fontMap = {
	{' ', {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0}},
    {'A', {0,1,1,0,0, 1,0,0,1,0, 1,1,1,1,0, 1,0,0,1,0, 1,0,0,1,0}},
    {'B', {1,1,1,0,0, 1,0,0,1,0, 1,1,1,0,0, 1,0,0,1,0, 1,1,1,0,0}},
    {'C', {0,1,1,1,0, 1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0, 0,1,1,1,0}},
    {'D', {1,1,1,0,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 1,1,1,0,0}},
    {'E', {1,1,1,1,0, 1,0,0,0,0, 1,1,1,0,0, 1,0,0,0,0, 1,1,1,1,0}},
    {'F', {1,1,1,1,0, 1,0,0,0,0, 1,1,1,0,0, 1,0,0,0,0, 1,0,0,0,0}},
    {'G', {0,1,1,1,0, 1,0,0,0,0, 1,0,1,1,0, 1,0,0,1,0, 0,1,1,1,0}},
    {'H', {1,0,0,1,0, 1,0,0,1,0, 1,1,1,1,0, 1,0,0,1,0, 1,0,0,1,0}},
    {'I', {1,1,1,0,0, 0,1,0,0,0, 0,1,0,0,0, 0,1,0,0,0, 1,1,1,0,0}},
    {'J', {0,0,1,1,0, 0,0,0,1,0, 0,0,0,1,0, 1,0,0,1,0, 0,1,1,0,0}},
    {'K', {1,0,0,1,0, 1,0,1,0,0, 1,1,0,0,0, 1,0,1,0,0, 1,0,0,1,0}},
    {'L', {1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0, 1,1,1,1,0}},
    {'M', {1,0,0,0,1, 1,1,0,1,1, 1,0,1,0,1, 1,0,0,0,1, 1,0,0,0,1}},
    {'N', {1,0,0,0,1, 1,1,0,0,1, 1,0,1,0,1, 1,0,0,1,1, 1,0,0,0,1}},
    {'O', {0,1,1,0,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 0,1,1,0,0}},
    {'P', {1,1,1,0,0, 1,0,0,1,0, 1,1,1,0,0, 1,0,0,0,0, 1,0,0,0,0}},
    {'Q', {0,1,1,0,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,1,0,0, 0,1,0,1,0}},
    {'R', {1,1,1,0,0, 1,0,0,1,0, 1,1,1,0,0, 1,0,1,0,0, 1,0,0,1,0}},
    {'S', {0,1,1,1,0, 1,0,0,0,0, 0,1,1,0,0, 0,0,0,1,0, 1,1,1,0,0}},
    {'T', {1,1,1,1,1, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0}},
    {'U', {1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 0,1,1,0,0}},
    {'V', {1,0,0,0,1, 1,0,0,0,1, 0,1,0,1,0, 0,1,0,1,0, 0,0,1,0,0}},
    {'W', {1,0,0,0,1, 1,0,0,0,1, 1,0,1,0,1, 1,0,1,0,1, 0,1,0,1,0}},
    {'X', {1,0,0,0,1, 0,1,0,1,0, 0,0,1,0,0, 0,1,0,1,0, 1,0,0,0,1}},
    {'Y', {1,0,0,0,1, 0,1,0,1,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0}},
    {'Z', {1,1,1,1,1, 0,0,0,1,0, 0,0,1,0,0, 0,1,0,0,0, 1,1,1,1,1}},
    {'0', {0,1,1,0,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 0,1,1,0,0}},
    {'1', {0,0,1,0,0, 0,1,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,1,1,1,0}},
    {'2', {0,1,1,0,0, 1,0,0,1,0, 0,0,1,0,0, 0,1,0,0,0, 1,1,1,1,0}},
    {'3', {1,1,1,0,0, 0,0,0,1,0, 0,1,1,0,0, 0,0,0,1,0, 1,1,1,0,0}},
    {'4', {0,0,1,1,0, 0,1,0,1,0, 1,0,0,1,0, 1,1,1,1,1, 0,0,0,1,0}},
    {'5', {1,1,1,1,0, 1,0,0,0,0, 1,1,1,0,0, 0,0,0,1,0, 1,1,1,0,0}},
    {'6', {0,1,1,0,0, 1,0,0,0,0, 1,1,1,0,0, 1,0,0,1,0, 0,1,1,0,0}},
    {'7', {1,1,1,1,0, 0,0,0,1,0, 0,0,1,0,0, 0,1,0,0,0, 1,0,0,0,0}},
    {'8', {0,1,1,0,0, 1,0,0,1,0, 0,1,1,0,0, 1,0,0,1,0, 0,1,1,0,0}},
    {'9', {0,1,1,0,0, 1,0,0,1,0, 0,1,1,1,0, 0,0,0,1,0, 0,1,1,0,0}},
    {':', {0,0,0,0,0, 0,0,1,0,0, 0,0,0,0,0, 0,0,1,0,0, 0,0,0,0,0}},
    {'-', {0,0,0,0,0, 0,0,0,0,0, 1,1,1,1,0, 0,0,0,0,0, 0,0,0,0,0}},
    {'.', {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,1,0,0}}
    
	
};

void SpawnFruit();
void InitGame();
void ResetGame();
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void DrawCell(const Vec2i& gridPos, const Vec3& color);
void DrawChar(char character, float x, float y, float scale, const Vec3& color);
void DrawText(const string& text, float x, float y, float scale, const Vec3& color);
void RenderGame(GLFWwindow* window);
void UpdateGame(float deltaTime);
void DrawBorder();
void DrawSnake();
void DrawScore();
void DrawGameOver();
void DrawStartScreen();

int main() 
{
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "SnakeGame", nullptr, nullptr);

    if (window == nullptr)
    {
        cout << "Windpw os null or has errors";
        glfwTerminate();
        return -1;

    }

    glfwSetKeyCallback(window, keyCallback);


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

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); 
    const char* vertexShaderCStr = VertexShader.c_str(); 
    glShaderSource(vertexShader, 1, &vertexShaderCStr, nullptr); 
    glCompileShader(vertexShader);

    GLint Sucess;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &Sucess);

    if (!Sucess)
    {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        cerr << "Error compiling vertex shader: " << infoLog << endl;
        glfwTerminate();
        return -1;
    }


    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    const char* FragemntShaderSource = FragmentShader.c_str();
    glShaderSource(fragmentShader, 1, &FragemntShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &Sucess);

    if (!Sucess)
    {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        cerr << "Error compiling fragment shader: " << infoLog << endl;
        glfwTerminate();
        return -1;
    }

    shadeprogram = glCreateProgram();
    glAttachShader(shadeprogram, vertexShader);
    glAttachShader(shadeprogram, fragmentShader);
    glLinkProgram(shadeprogram);
    glGetProgramiv(shadeprogram, GL_LINK_STATUS, &Sucess);

    if (!Sucess)
    {
        char infoLog[512];
        glGetProgramInfoLog(shadeprogram, 512, nullptr, infoLog);
        cerr << "Error in Shader program linking failed " << infoLog << endl;
		glfwTerminate();
		return -1;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

     uColorLoc = glGetUniformLocation(shadeprogram, "uColor");
	 uScaleLoc = glGetUniformLocation(shadeprogram, "uScale");
     uOffsetLoc = glGetUniformLocation(shadeprogram, "uOffset");

     const float vertices[] = {
         -0.5f,-0.5f, // bottom left
          0.5f,-0.5f, // bottom right
         -0.5f, 0.5f, // top right
          0.5f, 0.5f  // top left
     };



}
