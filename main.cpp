#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>

GLuint Program;
GLuint VAO_square, VBO_square, EBO_square;
GLuint VAO_triangle, VBO_triangle, EBO_triangle;

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

std::vector<GLuint> indices_square = {
    0, 1, 2, 3
};

std::vector<GLuint> indices_triangle = {
    0, 1, 2
};

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

void InitShaders()
{
    InitShaderColor();
}

void InitBuffers()
{
	InitSquare();
	InitTriangle();
}

void Draw()
{
    if (task == 1)
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
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        Draw();

        window.display();
    }
}
