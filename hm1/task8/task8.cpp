#include "task8.h"

const char* vertexShaderSource8 = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 MVP;
void main() {
    gl_Position = MVP * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource8 = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 objectColor;
void main() {
    FragColor = vec4(objectColor, 1.0);
}
)";

static glm::mat4 getPivotScaleMatrix(float px, float py, float scaleX, float scaleY) {
    glm::mat4 m = glm::mat4(1.0f);
    // 3. Повертаємо об'єкт назад
    m = glm::translate(m, glm::vec3(px, py, 0.0f));
    // 2. Розтягуємо
    m = glm::scale(m, glm::vec3(scaleX, scaleY, 1.0f));
    // 1. Переміщуємо опорну точку в (0,0)
    m = glm::translate(m, glm::vec3(-px, -py, 0.0f));
    return m;
}

void runTask8() {
    if (!glfwInit()) return;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Task 8: Pivot Scaling", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return;

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource8, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource8, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        0.0f, 0.0f, 0.0f, // P1
        1.0f, 0.0f, 0.0f, // P2
        1.0f, 1.0f, 0.0f, // P3
        0.0f, 1.0f, 0.0f  // P4
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    glm::mat4 projection = glm::ortho(-4.0f, 5.0f, -5.0f, 5.0f, -1.0f, 1.0f);
    glm::mat4 modelBase = glm::mat4(1.0f);
    glm::mat4 modelPivot1 = getPivotScaleMatrix(0.5f, 0.5f, 2.0f, 3.0f); // Центр
    glm::mat4 modelPivot2 = getPivotScaleMatrix(0.0f, 1.0f, 2.0f, 3.0f); // Лівий верхній
    glm::mat4 modelPivot3 = getPivotScaleMatrix(1.0f, 1.0f, 2.0f, 3.0f); // Правий верхній
    glm::mat4 modelPivot4 = getPivotScaleMatrix(2.0f, 2.0f, 2.0f, 3.0f); // Зовні

    glLineWidth(2.5f);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        GLint mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
        GLint colorLoc = glGetUniformLocation(shaderProgram, "objectColor");

        glm::mat4 currentMVP;

        // 0. Базовий квадрат (Сірий)
        currentMVP = projection * modelBase;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(currentMVP));
        glUniform3f(colorLoc, 0.6f, 0.6f, 0.6f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        // 1. Pivot (0.5, 0.5) - Блакитний
        currentMVP = projection * modelPivot1;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(currentMVP));
        glUniform3f(colorLoc, 0.2f, 0.6f, 0.9f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        // 2. Pivot (0, 1) - Помаранчевий
        currentMVP = projection * modelPivot2;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(currentMVP));
        glUniform3f(colorLoc, 0.9f, 0.5f, 0.1f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        // 3. Pivot (1, 1) - Зелений
        currentMVP = projection * modelPivot3;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(currentMVP));
        glUniform3f(colorLoc, 0.1f, 0.7f, 0.2f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        // 4. Pivot (2, 2) - Фіолетовий
        currentMVP = projection * modelPivot4;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(currentMVP));
        glUniform3f(colorLoc, 0.6f, 0.2f, 0.8f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
}