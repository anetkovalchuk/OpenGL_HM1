#include "task11.h"

const char* vertexShaderSource11 = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 MVP;
void main() {
    gl_Position = MVP * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource11 = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 objectColor;
void main() {
    FragColor = vec4(objectColor, 1.0);
}
)";

void runTask11() {
    // 1. Ініціалізація GLFW
    if (!glfwInit()) return;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Task 11: Inverse Transformation", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return;

    // 2. Компіляція шейдерів
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource11, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource11, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //Обчислення на CPU
    // Задаємо матрицю T з умови.
    glm::mat4 T = glm::mat4(
        2.934f,  0.624f, 0.0f, 0.0f,
       -0.416f,  1.956f, 0.0f, 0.0f,
        0.0f,    0.0f,   1.0f, 0.0f,
        2.000f,  3.400f, 0.0f, 1.0f
    );

    // Зоротна матриця T^-1
    glm::mat4 T_inv = glm::inverse(T);

    glm::vec4 globalPoints[4] = {
        glm::vec4(2.0f, 3.4f, 0.0f, 1.0f),
        glm::vec4(4.9f, 4.0f, 0.0f, 1.0f),
        glm::vec4(4.5f, 6.0f, 0.0f, 1.0f),
        glm::vec4(1.6f, 5.4f, 0.0f, 1.0f)
    };

    float globalVertices[12];
    float localVertices[12];

    std::cout << "Обчислення Завдання 11 " << std::endl;
    std::cout << std::fixed << std::setprecision(3);
    
    for (int i = 0; i < 4; ++i) {
        globalVertices[i*3] = globalPoints[i].x;
        globalVertices[i*3+1] = globalPoints[i].y;
        globalVertices[i*3+2] = 0.0f;

        // Локальні координати
        glm::vec4 localP = T_inv * globalPoints[i];
        localVertices[i*3] = localP.x;
        localVertices[i*3+1] = localP.y;
        localVertices[i*3+2] = 0.0f;

        std::cout << "Глобальна P" << i+1 << "(" << globalPoints[i].x << ", " << globalPoints[i].y << ") --> ";
        std::cout << "Локальна P" << i+1 << "(" << localP.x << ", " << localP.y << ")" << std::endl;
    }
    std::cout << "==============================" << std::endl;

    
    unsigned int VBO_Global, VAO_Global;
    glGenVertexArrays(1, &VAO_Global);
    glGenBuffers(1, &VBO_Global);
    glBindVertexArray(VAO_Global);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Global);
    glBufferData(GL_ARRAY_BUFFER, sizeof(globalVertices), globalVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int VBO_Local, VAO_Local;
    glGenVertexArrays(1, &VAO_Local);
    glGenBuffers(1, &VBO_Local);
    glBindVertexArray(VAO_Local);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Local);
    glBufferData(GL_ARRAY_BUFFER, sizeof(localVertices), localVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glm::mat4 projection = glm::ortho(-1.0f, 6.0f, -1.0f, 7.0f, -1.0f, 1.0f);
    
    glLineWidth(2.5f);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        GLint mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
        GLint colorLoc = glGetUniformLocation(shaderProgram, "objectColor");

        glm::mat4 currentMVP = projection * glm::mat4(1.0f); 
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(currentMVP));

        // 1. Малюємо локальний  прямокутник - Зелений
        glBindVertexArray(VAO_Local);
        glUniform3f(colorLoc, 0.1f, 0.8f, 0.1f); 
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        // 2. Малюємо глобальний прямокутник - Червоний
        glBindVertexArray(VAO_Global);
        glUniform3f(colorLoc, 0.9f, 0.2f, 0.2f); 
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO_Global);
    glDeleteBuffers(1, &VBO_Global);
    glDeleteVertexArrays(1, &VAO_Local);
    glDeleteBuffers(1, &VBO_Local);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
}