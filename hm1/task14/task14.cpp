#include "task14.h"

const char* vertexShaderSource14 = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 MVP;
void main() {
    gl_Position = MVP * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource14 = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 objectColor;
void main() {
    FragColor = vec4(objectColor, 1.0);
}
)";

// Функція для друку матриці в консоль
void printMatrix(const glm::mat4& m, const std::string& name) {
    std::cout << "Матриця " << name << ":" << std::endl;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            // Зверни увагу: GLM зберігає дані по стовпцях m[стовпець][рядок]
            std::cout << std::setw(8) << std::fixed << std::setprecision(3) << m[j][i] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "-----------------------------------" << std::endl;
}

void runTask14() {
    if (!glfwInit()) return;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Task 14: Matrix Rebuilding with Pivot", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return;

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource14, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource14, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        0.0f, 0.0f, 0.0f, 
        1.0f, 0.0f, 0.0f, 
        1.0f, 1.0f, 0.0f, 
        0.0f, 1.0f, 0.0f  
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    float pivotVertex[] = { 1.0f, 1.0f, 0.0f };
    unsigned int pivotVBO, pivotVAO;
    glGenVertexArrays(1, &pivotVAO);
    glGenBuffers(1, &pivotVBO);
    glBindVertexArray(pivotVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pivotVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pivotVertex), pivotVertex, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glm::mat4 rebuiltMatrix = glm::mat4(1.0f);
    
    // 3. Фінальне чисте переміщення
    rebuiltMatrix = glm::translate(rebuiltMatrix, glm::vec3(4.732f, -1.268f, 0.0f));
    
    // 2. Блок трансформації навколо Pivot(1, 1)
    rebuiltMatrix = glm::translate(rebuiltMatrix, glm::vec3(1.0f, 1.0f, 0.0f));      // 2в: Повертаємо в (1,1)
    rebuiltMatrix = glm::rotate(rebuiltMatrix, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // 2б: Поворот 30°
    rebuiltMatrix = glm::scale(rebuiltMatrix, glm::vec3(2.0f, 2.0f, 1.0f));          // 2а: Масштаб 2x2
    rebuiltMatrix = glm::translate(rebuiltMatrix, glm::vec3(-1.0f, -1.0f, 0.0f));    // 2а: Зсув у (0,0)

    std::cout << "\n=== Завдання 14: Перевірка ===" << std::endl;
    printMatrix(rebuiltMatrix, "T (Відновлена з компонентів)");
    std::cout << "Порівняй з умовою: (1.732, -1, 5) | (1, 1.732, -3) | (0, 0, 1)\n";

    glm::mat4 projection = glm::ortho(-2.0f, 8.0f, -4.0f, 3.0f, -1.0f, 1.0f);
    
    glLineWidth(2.5f);
    glPointSize(8.0f);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        GLint mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
        GLint colorLoc = glGetUniformLocation(shaderProgram, "objectColor");

        // 1. Початковий квадрат (Сірий)
        glBindVertexArray(VAO);
        glm::mat4 mvpInitial = projection * glm::mat4(1.0f);
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpInitial));
        glUniform3f(colorLoc, 0.6f, 0.6f, 0.6f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        // 2. Фінальний квадрат (Зелений) за допомогою відновленої матриці
        glm::mat4 mvpFinal = projection * rebuiltMatrix;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpFinal));
        glUniform3f(colorLoc, 0.1f, 0.8f, 0.2f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        // 3. Опорна точка (Червона)
        glBindVertexArray(pivotVAO);
        glm::mat4 mvpPivot = projection * glm::mat4(1.0f);
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpPivot));
        glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f);
        glDrawArrays(GL_POINTS, 0, 1);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &pivotVAO);
    glDeleteBuffers(1, &pivotVBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
}