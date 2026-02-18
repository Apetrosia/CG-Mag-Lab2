#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>

GLuint Program;
GLuint VAO_square, VBO_square, EBO_square;
GLuint VAO_triangle, VBO_triangle, EBO_triangle;
GLuint VAO_pentagon, VBO_pentagon, EBO_pentagon;

GLfloat transformMatrix[16];

int task = 1;

// Вершины: coord (x,y,z) + color (r,g,b)
std::vector<GLfloat> vertices_square = {
    // x,     y,    z      r,   g,   b
    -0.5f, -0.5f, 0.0f,  0.2f, 0.7f, 1.0f, // нижняя левая
     0.5f, -0.5f, 0.0f,  0.2f, 0.7f, 1.0f, // нижняя правая
     0.5f,  0.5f, 0.0f,  0.2f, 0.7f, 1.0f, // верхняя правая
    -0.5f,  0.5f, 0.0f,  0.2f, 0.7f, 1.0f  // верхняя левая
};

// Вершины: coord (x,y,z) + color (r,g,b)
std::vector<GLfloat> vertices_triangle = {
    // x,     y,    z      r,   g,   b
    -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // нижняя левая
     0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, // нижняя правая
     0.0f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // верхняя
};

std::vector<GLfloat> vertices_pentagon = {
    // x,     y,    z      r,   g,   b
	 0.0f,  -0.5f,   0.0f, 1.0f, 0.0f, 0.0f,
     0.475f,-0.155f, 0.0f, 1.0f, 0.0f, 0.0f,
     0.293f, 0.405f, 0.0f, 1.0f, 0.0f, 0.0f,
    -0.293f, 0.405f, 0.0f, 1.0f, 0.0f, 0.0f,
    -0.475f,-0.155f, 0.0f, 1.0f, 0.0f, 0.0f,
};


std::vector<GLuint> indices_square = {
    0, 1, 2, 3
};

std::vector<GLuint> indices_triangle = {
    0, 1, 2
};
std::vector<GLuint> indices_pentagon = {
    0, 1, 2, 3, 4
};

void CreateTransformMatrix(float angleX, float angleY, float angleZ)
{
    GLfloat rx[16] = {
        1,0,0,0,
        0,cos(angleX),-sin(angleX),0,
        0,sin(angleX), cos(angleX),0,
        0,0,0,1
    };

    GLfloat ry[16] = {
        cos(angleY),0,sin(angleY),0,
        0,1,0,0,
        -sin(angleY),0,cos(angleY),0,
        0,0,0,1
    };

    GLfloat rz[16] = {
        cos(angleZ),-sin(angleZ),0,0,
        sin(angleZ), cos(angleZ),0,0,
        0,0,1,0,
        0,0,0,1
    };

    GLfloat s[16] = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };

    GLfloat t1[16], t2[16];

    for (int i = 0; i < 16; i++)
        t1[i] = ry[i % 4] * rx[i / 4 * 4] +
        ry[i % 4 + 4] * rx[i / 4 * 4 + 1] +
        ry[i % 4 + 8] * rx[i / 4 * 4 + 2] +
        ry[i % 4 + 12] * rx[i / 4 * 4 + 3];

    for (int i = 0; i < 16; i++)
        t2[i] = rz[i % 4] * t1[i / 4 * 4] +
        rz[i % 4 + 4] * t1[i / 4 * 4 + 1] +
        rz[i % 4 + 8] * t1[i / 4 * 4 + 2] +
        rz[i % 4 + 12] * t1[i / 4 * 4 + 3];

    for (int i = 0; i < 16; i++)
        transformMatrix[i] =
        s[i % 4] * t2[i / 4 * 4] +
        s[i % 4 + 4] * t2[i / 4 * 4 + 1] +
        s[i % 4 + 8] * t2[i / 4 * 4 + 2] +
        s[i % 4 + 12] * t2[i / 4 * 4 + 3];
}

const char* VS = R"(
#version 330 core
layout(location=0) in vec3 coord;
layout(location=1) in vec3 color;

out vec3 vColor;

void main() {
    gl_Position = vec4(coord, 1.0);
    vColor = color;
}
)";

const char* FS = R"(
#version 330 core
in vec3 vColor;
out vec4 color;

void main() {
    color = vec4(vColor, 1.0);
}
)";

void InitShaderColor()
{
    GLuint v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &VS, nullptr);
    glCompileShader(v);

    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &FS, nullptr);
    glCompileShader(f);

    Program = glCreateProgram();
    glAttachShader(Program, v);
    glAttachShader(Program, f);
    glLinkProgram(Program);

    glDeleteShader(v);
    glDeleteShader(f);
}

void InitSquare()
{
    glGenVertexArrays(1, &VAO_square);
    glBindVertexArray(VAO_square);

    glGenBuffers(1, &VBO_square);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_square);
    glBufferData(GL_ARRAY_BUFFER,
        vertices_square.size() * sizeof(GLfloat),
        vertices_square.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO_square);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_square);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices_square.size() * sizeof(GLuint),
        indices_square.data(), GL_STATIC_DRAW);

    // coord
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void InitTriangle()
{
	glGenVertexArrays(1, &VAO_triangle);
	glBindVertexArray(VAO_triangle);

	glGenBuffers(1, &VBO_triangle);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_triangle);
    glBufferData(GL_ARRAY_BUFFER,
        vertices_triangle.size() * sizeof(GLfloat),
		vertices_triangle.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &EBO_triangle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_triangle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices_triangle.size() * sizeof(GLuint),
		indices_triangle.data(), GL_STATIC_DRAW);

    // coord
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void InitPentagon()
{
    glGenVertexArrays(1, &VAO_pentagon);
    glBindVertexArray(VAO_pentagon);

    glGenBuffers(1, &VBO_pentagon);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_pentagon);
    glBufferData(GL_ARRAY_BUFFER,
        vertices_pentagon.size() * sizeof(GLfloat),
        vertices_pentagon.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO_pentagon);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_pentagon);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices_pentagon.size() * sizeof(GLuint),
        indices_pentagon.data(), GL_STATIC_DRAW);

    // coord
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void InitShaders()
{
    InitShaderColor();
}

void InitBuffers()
{
	InitSquare();
	InitTriangle();
	InitPentagon();
}

void Draw()
{
    if (task == 0)
    {

    }
    else if (task == 1)
    {
        // Левая половина экрана (квадрат)
        glViewport(0, 0, 800, 800);
        glUseProgram(Program);
        glBindVertexArray(VAO_square);
        glDrawElements(GL_QUADS, indices_square.size(), GL_UNSIGNED_INT, 0);

        // Правая половина экрана (треугольник)
        glViewport(800, 0, 800, 800);
        glUseProgram(Program);
        glBindVertexArray(VAO_triangle);
        glDrawElements(GL_TRIANGLES, indices_triangle.size(), GL_UNSIGNED_INT, 0);
    }
    else if (task == 2)
    {
        glViewport(400, 0, 800, 800);
        glUseProgram(Program);
        glBindVertexArray(VAO_pentagon);
        glDrawElements(GL_TRIANGLE_FAN, indices_pentagon.size(), GL_UNSIGNED_INT, 0);
    }
    else if (task == 2)
    {
        glViewport(400, 0, 800, 800);
        glUseProgram(Program);
        glBindVertexArray(VAO_pentagon);
        glDrawElements(GL_TRIANGLE_FAN, indices_pentagon.size(), GL_UNSIGNED_INT, 0);
    }
    else if (task == 2)
    {
        glViewport(400, 0, 800, 800);
        glUseProgram(Program);
        glBindVertexArray(VAO_pentagon);
        glDrawElements(GL_TRIANGLE_FAN, indices_pentagon.size(), GL_UNSIGNED_INT, 0);
    }
}

int main()
{
    sf::Window window(
        sf::VideoMode(1600, 800),
        "Square right side",
        sf::Style::Default,
        sf::ContextSettings(24)
    );

    window.setVerticalSyncEnabled(true);
    glewInit();

    InitShaders();
    InitBuffers();

    while (window.isOpen())
    {
        sf::Event e;
        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
                window.close();
            if (e.type == sf::Event::KeyPressed)
            {
                switch (e.key.code)
                {
                case(sf::Keyboard::Num1):
                    task = 1;
                    break;
                case(sf::Keyboard::Q):
                    task = 0;
                    break;
                case(sf::Keyboard::Num2):
                    task = 2;
                    break;
                case(sf::Keyboard::W):
                    task = 21;
                    break;
                case(sf::Keyboard::E):
                    task = 22;
                    break;
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        Draw();

        window.display();
    }
}
