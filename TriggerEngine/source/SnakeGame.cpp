
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

map<char, array<int, FONT_WIDTH* FONT_HEIGHT>> fontMap = {
    {' ', {{0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0}}},
    {'A', {{0,1,1,0,0, 1,0,0,1,0, 1,1,1,1,0, 1,0,0,1,0, 1,0,0,1,0}}},
    {'B', {{1,1,1,0,0, 1,0,0,1,0, 1,1,1,0,0, 1,0,0,1,0, 1,1,1,0,0}}},
    {'C', {{0,1,1,1,0, 1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0, 0,1,1,1,0}}},
    {'D', {{1,1,1,0,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 1,1,1,0,0}}},
    {'E', {{1,1,1,1,0, 1,0,0,0,0, 1,1,1,0,0, 1,0,0,0,0, 1,1,1,1,0}}},
    {'F', {{1,1,1,1,0, 1,0,0,0,0, 1,1,1,0,0, 1,0,0,0,0, 1,0,0,0,0}}},
    {'G', {{0,1,1,1,0, 1,0,0,0,0, 1,0,1,1,0, 1,0,0,1,0, 0,1,1,1,0}}},
    {'H', {{1,0,0,1,0, 1,0,0,1,0, 1,1,1,1,0, 1,0,0,1,0, 1,0,0,1,0}}},
    {'I', {{1,1,1,0,0, 0,1,0,0,0, 0,1,0,0,0, 0,1,0,0,0, 1,1,1,0,0}}},
    {'J', {{0,0,1,1,0, 0,0,0,1,0, 0,0,0,1,0, 1,0,0,1,0, 0,1,1,0,0}}},
    {'K', {{1,0,0,1,0, 1,0,1,0,0, 1,1,0,0,0, 1,0,1,0,0, 1,0,0,1,0}}},
    {'L', {{1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0, 1,1,1,1,0}}},
    {'M', {{1,0,0,0,1, 1,1,0,1,1, 1,0,1,0,1, 1,0,0,0,1, 1,0,0,0,1}}},
    {'N', {{1,0,0,0,1, 1,1,0,0,1, 1,0,1,0,1, 1,0,0,1,1, 1,0,0,0,1}}},
    {'O', {{0,1,1,0,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 0,1,1,0,0}}},
    {'P', {{1,1,1,0,0, 1,0,0,1,0, 1,1,1,0,0, 1,0,0,0,0, 1,0,0,0,0}}},
    {'Q', {{0,1,1,0,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,1,0,0, 0,1,0,1,0}}},
    {'R', {{1,1,1,0,0, 1,0,0,1,0, 1,1,1,0,0, 1,0,1,0,0, 1,0,0,1,0}}},
    {'S', {{0,1,1,1,0, 1,0,0,0,0, 0,1,1,0,0, 0,0,0,1,0, 1,1,1,0,0}}},
    {'T', {{1,1,1,1,1, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0}}},
    {'U', {{1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 0,1,1,0,0}}},
    {'V', {{1,0,0,0,1, 1,0,0,0,1, 0,1,0,1,0, 0,1,0,1,0, 0,0,1,0,0}}},
    {'W', {{1,0,0,0,1, 1,0,0,0,1, 1,0,1,0,1, 1,0,1,0,1, 0,1,0,1,0}}},
    {'X', {{1,0,0,0,1, 0,1,0,1,0, 0,0,1,0,0, 0,1,0,1,0, 1,0,0,0,1}}},
    {'Y', {{1,0,0,0,1, 0,1,0,1,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0}}},
    {'Z', {{1,1,1,1,1, 0,0,0,1,0, 0,0,1,0,0, 0,1,0,0,0, 1,1,1,1,1}}},
    {'0', {{0,1,1,0,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 0,1,1,0,0}}},
    {'1', {{0,0,1,0,0, 0,1,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,1,1,1,0}}},
    {'2', {{0,1,1,0,0, 1,0,0,1,0, 0,0,1,0,0, 0,1,0,0,0, 1,1,1,1,0}}},
    {'3', {{1,1,1,0,0, 0,0,0,1,0, 0,1,1,0,0, 0,0,0,1,0, 1,1,1,0,0}}},
    {'4', {{0,0,1,1,0, 0,1,0,1,0, 1,0,0,1,0, 1,1,1,1,1, 0,0,0,1,0}}},
    {'5', {{1,1,1,1,0, 1,0,0,0,0, 1,1,1,0,0, 0,0,0,1,0, 1,1,1,0,0}}},
    {'6', {{0,1,1,0,0, 1,0,0,0,0, 1,1,1,0,0, 1,0,0,1,0, 0,1,1,0,0}}},
    {'7', {{1,1,1,1,0, 0,0,0,1,0, 0,0,1,0,0, 0,1,0,0,0, 1,0,0,0,0}}},
    {'8', {{0,1,1,0,0, 1,0,0,1,0, 0,1,1,0,0, 1,0,0,1,0, 0,1,1,0,0}}},
    {'9', {{0,1,1,0,0, 1,0,0,1,0, 0,1,1,1,0, 0,0,0,1,0, 0,1,1,0,0}}},
    {':', {{0,0,0,0,0, 0,0,1,0,0, 0,0,0,0,0, 0,0,1,0,0, 0,0,0,0,0}}},
    {'-', {{0,0,0,0,0, 0,0,0,0,0, 1,1,1,1,0, 0,0,0,0,0, 0,0,0,0,0}}},
    {'.', {{0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,1,0,0}}}


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

     glGenVertexArrays(1, &vao);
     glGenBuffers(1, &vbo);

     glBindVertexArray(vao);
     glBindBuffer(GL_ARRAY_BUFFER, vbo);
     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

     glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
     glEnableVertexAttribArray(0);

     // starts game
     InitGame();

     // Game Loop 
	 auto lastTime = chrono::high_resolution_clock::now(); // stores the current time point using the high-resolution clock, which provides a more precise measurement of time intervals compared to other clock types, and is used to calculate the delta time for updating the game state at a consistent rate
     while (!glfwWindowShouldClose(window))
     {
         auto currentTime = chrono::high_resolution_clock::now();
         float deltaTime = chrono::duration<float>(currentTime - lastTime).count();
         lastTime = currentTime;

         glfwPollEvents();

        
         UpdateGame(deltaTime);
         RenderGame(window);
      //   glfwSwapBuffers(window);
        
     }
     
     // clean up 
     glDeleteVertexArrays(1, &vao);
     glDeleteBuffers(1, &vbo);
     glDeleteProgram(shadeprogram);
     glfwTerminate();
	 return 0;

}

void InitGame() 
{
	ResetGame();
	SpawnFruit();
}

void ResetGame() // resets game state 
{

    snakeBody = { Vec2i(5,10), Vec2i(4,10), Vec2i(3,10) };
    currentDirection = Dir::NONE;
    score = 0;
	gameOver = false;
    gameStarted = false;
	timeSinceLastUpdate = 0.0f;
	SnakeSpeed = Update_Interval;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    if (action != GLFW_PRESS) return;

    if (!gameStarted && key != GLFW_KEY_R)
    {
        gameStarted = true;
		currentDirection = Dir::RIGHT;
        return;
	}

    if (gameOver && key != GLFW_KEY_R)
    {
		ResetGame();
		SpawnFruit();
        return;
    }

    if (gameOver && !gameStarted) return;

    switch (key)
    {
    case GLFW_KEY_UP:
        if (currentDirection != Dir::DOWN) currentDirection = Dir::UP;
        break;
    case GLFW_KEY_DOWN:
        if (currentDirection != Dir::UP) currentDirection = Dir::DOWN;
        break;
    case GLFW_KEY_LEFT:
        if (currentDirection != Dir::RIGHT) currentDirection = Dir::LEFT;
        break;
    case GLFW_KEY_RIGHT:
        if (currentDirection != Dir::LEFT) currentDirection = Dir::RIGHT;
        break;
    default:
        break;

    }
    

}

void SpawnFruit() // spawns fruit at random position on the grid that is not occupied by the snake's body, which is done by generating random x and y coordinates within the grid size and checking if they collide with any of the snake's body segments before placing the fruit
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, GRID_SIZE - 1);
    while (true)
    {
        Vec2i potentialFruit(dis(gen), dis(gen));
        bool collision = false;
        for (const auto& segment : snakeBody)
        {
            if (segment == potentialFruit)
            {
                collision = true;
                break;
            }
        }
        if (!collision)
        {
            fruit = potentialFruit;
            break;
        }
    }
}

void UpdateGame(float deltaTime) 
{

    if (!gameStarted && gameOver) return;
    
	timeSinceLastUpdate += deltaTime;

	if (timeSinceLastUpdate >= SnakeSpeed) // checks if the accumulated time since the last update exceeds the defined SnakeSpeed, which determines how often the game state should be updated based on the player's speed and the defined update interval
    {
		timeSinceLastUpdate = 0.0f;

		Vec2i newHead = snakeBody.front(); // creates a new head position for the snake based on the current direction of movement, which is determined by the currentDirection variable and updates the x or y coordinate accordingly to move the snake in the desired direction

        switch (currentDirection)
        {
        case Dir::UP:
            newHead.y += 1;
            break;
        case Dir::DOWN:
            newHead.y -= 1;
            break;
        case Dir::LEFT:
            newHead.x -= 1;
            break;
        case Dir::RIGHT:
            newHead.x += 1;
            break;
        default:
            return; // if the current direction is NONE, the function returns early without updating the game state, which means that the snake will not move until a valid direction is set
		}

        if (newHead.x < 0 || newHead.x >= GRID_SIZE || newHead.y < 0 || newHead.y >= GRID_HEIGHT) 
        {
            gameOver = true;
            return;
        }

        // if snake hit its self 
        for (const auto& segment : snakeBody)
        {
            if (segment == newHead)
            {
                gameOver = true;
                return;
            }
        }

        // adds head 
		snakeBody.insert(snakeBody.begin(), newHead);

        if (newHead == fruit) 
        {
            score += 10;
			SpawnFruit();

            if (score % 50 == 0 && SnakeSpeed > 0.05f)
            {
                SnakeSpeed -= 0.01f;
            }
        }

        else
        {
			snakeBody.pop_back(); // removes the tail segment of the snake, which allows the snake to move forward without growing in length unless it has just eaten a fruit, in which case the tail is not removed and the snake grows by one segment
        }
    }

}

void RenderGame(GLFWwindow* window) 
{

    // back buffer
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    /// handles the colour of thw windw by rendering the coulr using opengl api
    glClear(GL_COLOR_BUFFER_BIT);
  
    glUseProgram(shadeprogram);
    glBindVertexArray(vao);

	DrawBorder();

    if(!gameStarted)
    {
		DrawStartScreen();
    }

    else if (gameOver) 
    {
		DrawGameOver();
    }

    else
    {
		DrawSnake();
        DrawScore();
    }

	glBindVertexArray(0); // unbinds the vertex array object, which is a good practice to prevent accidental modifications to the vertex attribute configurations and buffer bindings in subsequent rendering calls, and ensures that the state of the vertex array is not unintentionally altered by other parts of the code
    glfwSwapBuffers(window);

}

void DrawStartScreen()
{
    // Draws Snake and Fruit
    DrawSnake();

    // Draws Title
    DrawText("SNAKE GAME", 0.0f, 0.3f, 0.025f, Vec3(0.2f, 0.8f, 0.3f));

    // instructions
    DrawText("USE ARROW KEYS TO MOVE", 0.0f, 0.0f, 0.012f, Vec3(0.9f, 0.8f, 0.3f));
    DrawText("EAT THE RED FRUIT TO GROW", 0.0f, -0.1f, 0.012f, Vec3(0.9f, 0.8f, 0.3f));
    DrawText("DONT TOUCH THE WALLS OR YOURSELF", 0.0f, -0.2f, 0.010f, Vec3(0.9f, 0.8f, 0.3f));
    DrawText("PRESS ANY KEY TO START", 0.0f, -0.4f, 0.010f, Vec3(0.9f, 0.8f, 0.3f));

}

void DrawGameOver()
{

    DrawText("GAME OVER", 0.0f, 0.1f, 0.03f, Vec3(1.0f, 0.2f, 0.2f));
    DrawText("SCORE: " + to_string(score), 0.0f, -0.05f, 0.02f, Vec3(1.0f, 0.8f, 0.2f));
    DrawText("PRESS ANY KEY TO RESTART", 0.0f, -0.2f, 0.010f, Vec3(1.0f, 0.8f, 0.2f));
}

void DrawCell(const Vec2i& gridPos, const Vec3& color)
{

    // finds the centre grid position of the grid
    Vec2 offset(-1.0f + gridPos.x * CELL_WIDTH + CELL_WIDTH * 0.5f, 
        -1.0f + gridPos.y * CELL_HEIGHT + CELL_HEIGHT * 0.5f);

	Vec2 scale(CELL_WIDTH * 0.9f, CELL_HEIGHT * 0.9f); // makes cell slightly smaller to create a gap between cells for better visibility

	// draws Cell Rectangle
    glUniform3f(uColorLoc, color.r, color.g, color.b);
    glUniform2f(uOffsetLoc, offset.x, offset.y);
    glUniform2f(uScaleLoc, scale.x, scale.y); 
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}

void DrawChar(char character, float x, float y, float scale, const Vec3& color)
{

    /*if (fontMap.size() != FONT_WIDTH * FONT_HEIGHT)
    {
        std::cerr << "Font data corrupted for char: " << character << std::endl;
        return;
    }*/
	//character = toupper(character); // converts to uppercase
	auto it = fontMap.find(character);

    if (it == fontMap.end())
    {
        std::cerr << "Character not found: " << character << std::endl;
		it = fontMap.find(' '); // if character not found, use space character
    }

	const array<int, 25>& bitmap = it->second; // retrieves the bitmap representation of the character from the fontMap

    if (bitmap.size() != FONT_WIDTH * FONT_HEIGHT)
    {
        std::cerr << "Font data corrupted for char: " << character << std::endl;
        return;
    }

	float charWidth = FONT_WIDTH * scale;
	float charHeight = FONT_HEIGHT * scale;

    for (int row = 0; row < FONT_HEIGHT; ++row)
    {
        for (int col = 0; col < FONT_WIDTH; ++col)
        {
            if (bitmap[row * FONT_WIDTH + col]) // 
            {
                Vec2 offset(x + col * scale - charWidth / 2.0f,
                    y - row * scale + charHeight / 2.0f);

                glUniform3f(uColorLoc, color.r, color.g, color.b);
                glUniform2f(uOffsetLoc, offset.x, offset.y);
                glUniform2f(uScaleLoc, scale, scale);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            }
        }
    }
}

void DrawText(const string& text, float x, float y, float scale, const Vec3& color)
{
	float charWidth = FONT_WIDTH * scale + FONT_SPACE * scale;
    
	float totalWidth = text.size() * charWidth - FONT_SPACE * scale; // calculates the total width of the text string based on the number of characters, the width of each character, and the spacing between characters, which is used to center the text on the screen by adjusting the starting x position accordingly

    float startX = x - totalWidth / 2.0f;

    for (size_t i = 0; i < text.length(); ++i)
    {
        DrawChar(text[i], startX + i * charWidth, y, scale, color); // draws each character in the text string by calling the DrawChar function with the appropriate parameters for position, scale, and color
    }
}

void DrawBorder() 
{
    Vec3 BorderColour(0.3f, 0.3f, 0.5f);

    for (int x = 0; x < GRID_SIZE; ++x)
    {
        DrawCell(Vec2i(x, 0), BorderColour); // bottom border
        DrawCell(Vec2i(x, GRID_HEIGHT - 1), BorderColour); // top border
	}

    for (int y = 0; y < GRID_HEIGHT; ++y)
    {
        DrawCell(Vec2i(0, y), BorderColour); // left border
        DrawCell(Vec2i(GRID_SIZE - 1, y), BorderColour); // right border    
    }

    // grid lines
	Vec3 gridColour(0.2f, 0.2f, 0.4f);
    for (int x = 1; x < GRID_SIZE - 1; ++x)
    {
        for (int y = 1; y < GRID_HEIGHT - 1; ++y)
        {
            DrawCell(Vec2i(x, y), gridColour); // draws the inner grid cells with a different color to create a visual distinction between the borders and the playable area of the grid
        }
	}
}

void DrawSnake()
{
	Vec3 HeadColour(0.0f, 0.95f, 0.3f);
	Vec3 bodyColour(0.0f, 0.7f, 0.1f);

    // Draws Snake Body
    for (size_t i = 0; i < snakeBody.size(); ++i) 
    {
        // gradient effect
		float factor = static_cast<float>(i) / snakeBody.size();
        Vec3 segementColour(bodyColour.r * (1.0f - factor) + 0.1f * factor,
            bodyColour.g * (1.0f - factor) + 0.8f * factor,
            bodyColour.b * (1.0f - factor));

		DrawCell(snakeBody[i], segementColour);
    }

    DrawCell(snakeBody.front(), HeadColour);

	Vec3 FruitColour(0.95f, 0.1f, 0.1f);

    DrawCell(fruit, FruitColour);
}

void DrawScore()
{

	string scoreText = "SCORE: " + to_string(score);
	DrawText(scoreText, 0.0f, 0.9f, 0.02f, Vec3(1.0f, 1.0f, 1.0f));

}