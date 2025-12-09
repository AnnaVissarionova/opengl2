#define _USE_MATH_DEFINES
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <iostream>
#include <vector>
#include <math.h>

// ID шейдерных программ
GLuint programConstant;     // Константное закрашивание
GLuint programUniform;      // Закрашивание через uniform
GLuint programGradient;     // Градиентное закрашивание

// ID буферов вершин
GLuint vboConstant;
GLuint vboUniform;
GLuint vboGradient;

// Структуры для разных типов вершин
struct VertexConstant {
    GLfloat x;
    GLfloat y;
};

struct VertexUniform {
    GLfloat x;
    GLfloat y;
};

struct VertexGradient {
    GLfloat x;
    GLfloat y;
    GLfloat r;
    GLfloat g;
    GLfloat b;
};

// Исходный код шейдера для константного закрашивания
const char* vertexShaderSourceConstant = R"(
    #version 330 core
    in vec2 coord;
    void main() {
        gl_Position = vec4(coord, 0.0, 1.0);
    }
)";

const char* fragShaderSourceConstant = R"(
    #version 330 core
    out vec4 color;
    void main() {
        color = vec4(0.0, 1.0, 0.0, 1.0);
    }
)";

// Исходный код шейдера для закрашивания через uniform
const char* vertexShaderSourceUniform = R"(
    #version 330 core
    in vec2 coord;
    void main() {
        gl_Position = vec4(coord, 0.0, 1.0);
    }
)";

const char* fragShaderSourceUniform = R"(
    #version 330 core
    uniform vec3 shape_color;  
    out vec4 color;
    void main() {
        color = vec4(shape_color, 1.0);
    }
)";

// Исходный код шейдера для градиентного закрашивания
const char* vertexShaderSourceGradient = R"(
    #version 330 core
    in vec2 coord;
    in vec3 color_in;       
    out vec3 color_out;
    void main() {
        gl_Position = vec4(coord, 0.0, 1.0);
        color_out = color_in;
    }
)";

const char* fragShaderSourceGradient = R"(
    #version 330 core
    in vec3 color_out;
    out vec4 color;
    void main() {
        color = vec4(color_out, 1.0);
    }
)";

void checkOpenGLerror() {
    GLenum errCode;
    if ((errCode = glGetError()) != GL_NO_ERROR)
        std::cout << "Ошибка OpenGL: " << errCode << std::endl;
}

GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource, const char* programName) {
    std::cout << "Создание шейдерной программы: " << programName << std::endl;

    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vertexSource, NULL);
    glCompileShader(vShader);

    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fragmentSource, NULL);
    glCompileShader(fShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    glLinkProgram(program);

    int link_ok;
    glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        std::cout << "Ошибка при связывании шейдеров: " << programName << std::endl;
        return 0;
    }

    checkOpenGLerror();
    return program;
}

void initializeShaders() {
    programConstant = createShaderProgram(vertexShaderSourceConstant, fragShaderSourceConstant, "Constant");
    programUniform = createShaderProgram(vertexShaderSourceUniform, fragShaderSourceUniform, "Uniform");
    programGradient = createShaderProgram(vertexShaderSourceGradient, fragShaderSourceGradient, "Gradient");
}

void createVerticesConstant(std::vector<VertexConstant>& vertices, float yPos) {
    float size = 0.2f;
    float radius = 0.2f;

    // четырехугольник (слева)
    float xPos = -0.7f;
    vertices.push_back({ xPos - size, yPos - size });
    vertices.push_back({ xPos + size, yPos - size });
    vertices.push_back({ xPos + size, yPos + size });
    vertices.push_back({ xPos - size, yPos + size });

    // веер (в центре)
    xPos = 0.0f;
    // центральная вершина
    vertices.push_back({ xPos, yPos });
    // внешние вершины (4 точки)
    for (int i = 0; i <= 4; i++) {
        float angle = 2.0f * M_PI * i / 8.0f;
        vertices.push_back({ xPos + radius * cos(angle), yPos + radius * sin(angle) });
    }

    // пятиугольник (справа)
    xPos = 0.7f;
    for (int i = 0; i < 5; i++) {
        float angle = 2.0f * M_PI * i / 5.0f - M_PI / 2.0f;
        vertices.push_back({ xPos + radius * cos(angle), yPos + radius * sin(angle) });
    }
}

void createVerticesUniform(std::vector<VertexUniform>& vertices, float yPos) {
    float size = 0.2f;
    float radius = 0.2f;

    // четырехугольник (слева)
    float xPos = -0.7f;
    vertices.push_back({ xPos - size, yPos - size });
    vertices.push_back({ xPos + size, yPos - size });
    vertices.push_back({ xPos + size, yPos + size });
    vertices.push_back({ xPos - size, yPos + size });

    // веер (в центре)
    xPos = 0.0f;
    // центральная вершина
    vertices.push_back({ xPos, yPos });
    // внешние вершины (4 точки)
    for (int i = 0; i <= 4; i++) {
        float angle = 2.0f * M_PI * i / 8.0f;
        vertices.push_back({ xPos + radius * cos(angle), yPos + radius * sin(angle) });
    }

    // пятиугольник (справа)
    xPos = 0.7f;
    for (int i = 0; i < 5; i++) {
        float angle = 2.0f * M_PI * i / 5.0f - M_PI / 2.0f;
        vertices.push_back({ xPos + radius * cos(angle), yPos + radius * sin(angle) });
    }
}

void createVerticesGradient(std::vector<VertexGradient>& vertices, float yPos) {
    float size = 0.2f;
    float radius = 0.2f;

    // цвета для градиентного закрашивания
    float colors[5][3] = {
        {1.0f, 0.0f, 0.0f}, // красный
        {0.0f, 1.0f, 0.0f}, // зеленый
        {0.0f, 0.0f, 1.0f}, // синий
        {1.0f, 1.0f, 0.0f}, // желтый
        {1.0f, 0.0f, 1.0f}  // фиолетовый
    };

    // четырехугольник 
    float xPos = -0.7f;
    vertices.push_back({ xPos - size, yPos - size, colors[0][0], colors[0][1], colors[0][2] }); // красный
    vertices.push_back({ xPos + size, yPos - size, colors[1][0], colors[1][1], colors[1][2] }); // зеленый
    vertices.push_back({ xPos + size, yPos + size, colors[2][0], colors[2][1], colors[2][2] }); // синий
    vertices.push_back({ xPos - size, yPos + size, colors[3][0], colors[3][1], colors[3][2] }); // желтый

    // веер с градиентом
    xPos = 0.0f;
    // центр - фиолетовый
    vertices.push_back({ xPos, yPos, colors[4][0], colors[4][1], colors[4][2] });
    // внешние вершины - разные цвета
    for (int i = 0; i <= 4; i++) {
        float angle = 2.0f * M_PI * i / 8.0f;
        vertices.push_back({
            xPos + radius * cos(angle),
            yPos + radius * sin(angle),
            colors[i][0],
            colors[i][1],
            colors[i][2]
            });
    }

    // пятиугольник с градиентом
    xPos = 0.7f;
    for (int i = 0; i < 5; i++) {
        float angle = 2.0f * M_PI * i / 5.0f - M_PI / 2.0f;
        vertices.push_back({
            xPos + radius * cos(angle),
            yPos + radius * sin(angle),
            colors[i][0],
            colors[i][1],
            colors[i][2]
            });
    }
}

void initializeVBOs() {
    // Задание 2: Константное закрашивание (верхний ряд)
    std::vector<VertexConstant> verticesConstant;
    createVerticesConstant(verticesConstant, 0.7f);

    glGenBuffers(1, &vboConstant);
    glBindBuffer(GL_ARRAY_BUFFER, vboConstant);
    glBufferData(GL_ARRAY_BUFFER,
        verticesConstant.size() * sizeof(VertexConstant),
        verticesConstant.data(),
        GL_STATIC_DRAW);

    // Задание 3: Закрашивание через uniform (средний ряд)
    std::vector<VertexUniform> verticesUniform;
    createVerticesUniform(verticesUniform, 0.0f);

    glGenBuffers(1, &vboUniform);
    glBindBuffer(GL_ARRAY_BUFFER, vboUniform);
    glBufferData(GL_ARRAY_BUFFER,
        verticesUniform.size() * sizeof(VertexUniform),
        verticesUniform.data(),
        GL_STATIC_DRAW);

    // Задание 4: Градиентное закрашивание (нижний ряд)
    std::vector<VertexGradient> verticesGradient;
    createVerticesGradient(verticesGradient, -0.7f);

    glGenBuffers(1, &vboGradient);
    glBindBuffer(GL_ARRAY_BUFFER, vboGradient);
    glBufferData(GL_ARRAY_BUFFER,
        verticesGradient.size() * sizeof(VertexGradient),
        verticesGradient.data(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    checkOpenGLerror();
}

// Задание 2: Плоское закрашивание - цвет задан константой в шейдере
void drawConstantRow() {
    glUseProgram(programConstant);

    GLint attribCoord = glGetAttribLocation(programConstant, "coord");
    glEnableVertexAttribArray(attribCoord);

    glBindBuffer(GL_ARRAY_BUFFER, vboConstant);
    glVertexAttribPointer(attribCoord, 2, GL_FLOAT, GL_FALSE, sizeof(VertexConstant), (void*)0);

    // четырехугольник
    glDrawArrays(GL_POLYGON, 0, 4);
    // веер
    glDrawArrays(GL_TRIANGLE_FAN, 4, 5);
    // пятиугольник
    glDrawArrays(GL_POLYGON, 10, 5);

    glDisableVertexAttribArray(attribCoord);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

// Задание 3: Плоское закрашивание - цвет через uniform из программы
void drawUniformRow() {
    glUseProgram(programUniform);

    GLint attribCoord = glGetAttribLocation(programUniform, "coord");
    glEnableVertexAttribArray(attribCoord);

    glBindBuffer(GL_ARRAY_BUFFER, vboUniform);
    glVertexAttribPointer(attribCoord, 2, GL_FLOAT, GL_FALSE, sizeof(VertexUniform), (void*)0);

    GLint uniformColor = glGetUniformLocation(programUniform, "shape_color");

    // четырехугольник - красный
    glUniform3f(uniformColor, 1.0f, 0.0f, 0.0f);
    glDrawArrays(GL_POLYGON, 0, 4);

    // веер - синий
    glUniform3f(uniformColor, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_TRIANGLE_FAN, 4, 5);

    // пятиугольник - фиолетовый
    glUniform3f(uniformColor, 0.5f, 0.0f, 0.5f);
    glDrawArrays(GL_POLYGON, 10, 5);

    glDisableVertexAttribArray(attribCoord);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

// Задание 4: Градиентное закрашивание (каждая вершина своим цветом)
void drawGradientRow() {
    glUseProgram(programGradient);

    GLint attribCoord = glGetAttribLocation(programGradient, "coord");
    GLint attribColor = glGetAttribLocation(programGradient, "color_in");

    glEnableVertexAttribArray(attribCoord);
    glEnableVertexAttribArray(attribColor);

    glBindBuffer(GL_ARRAY_BUFFER, vboGradient);
    glVertexAttribPointer(attribCoord, 2, GL_FLOAT, GL_FALSE, sizeof(VertexGradient), (void*)0);
    glVertexAttribPointer(attribColor, 3, GL_FLOAT, GL_FALSE, sizeof(VertexGradient), (void*)(2 * sizeof(GLfloat)));

    // четырехугольник с градиентом
    glDrawArrays(GL_POLYGON, 0, 4);
    // веер с градиентом
    glDrawArrays(GL_TRIANGLE_FAN, 4, 5);
    // пятиугольник с градиентом
    glDrawArrays(GL_POLYGON, 10, 5);

    glDisableVertexAttribArray(attribColor);
    glDisableVertexAttribArray(attribCoord);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

void draw() {
    // верхний ряд - задание 2 (константное закрашивание)
    drawConstantRow();

    // средний ряд - задание 3 (закрашивание через uniform)
    drawUniformRow();

    // нижний ряд - задание 4 (градиентное закрашивание)
    drawGradientRow();

    checkOpenGLerror();
}

void releaseShaders() {
    glUseProgram(0);
    if (programConstant) glDeleteProgram(programConstant);
    if (programUniform) glDeleteProgram(programUniform);
    if (programGradient) glDeleteProgram(programGradient);
}

void releaseVBOs() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (vboConstant) glDeleteBuffers(1, &vboConstant);
    if (vboUniform) glDeleteBuffers(1, &vboUniform);
    if (vboGradient) glDeleteBuffers(1, &vboGradient);
}

void release() {
    releaseShaders();
    releaseVBOs();
}

int main() {
    setlocale(LC_ALL, "RUS");
    sf::RenderWindow window(sf::VideoMode({ 1000, 1000 }), "Лабораторная работа: 3 типа закрашивания");
    window.setVerticalSyncEnabled(true);
    window.setActive(true);

    glewInit();

    initializeShaders();
    initializeVBOs();

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
                glViewport(0, 0, resized->size.x, resized->size.y);
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw();

        window.display();
    }

    release();
    return 0;
}