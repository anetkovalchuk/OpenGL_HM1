#include "task6.h"

const char* vertexShaderSource6 = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 MVP;
void main() {
    gl_Position = MVP * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource6 = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 objectColor;
void main() {
    FragColor = vec4(objectColor, 1.0);
}
)";

void runTask6() {
    // 1. Ініціалізація GLFW
    if (!glfwInit()) return;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Task 6: Composition (Scale, Rotate, Translate)", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return;

    // 2. Компіляція шейдерів
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource6, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource6, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 3. Геометрія (Контур квадрата)
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

    // 4. Матриці
    // X: від -4 до 4, Y: від -1 до 6
    glm::mat4 projection = glm::ortho(-4.0f, 4.0f, -1.0f, 6.0f, -1.0f, 1.0f);

    // Стан 1: Початковий
    glm::mat4 model1 = glm::mat4(1.0f);

    // Стан 2: Масштабування по осі Y у 3 рази
    glm::mat4 model2 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));

    // Стан 3: Масштабування + Поворот на 60 градусів
    glm::mat4 model3 = glm::rotate(glm::mat4(1.0f), glm::radians(60.0f), glm::vec3(0, 0, 1));
    model3 = model3 * model2; // R * S

    // Стан 4: Масштабування + Поворот + Переміщення на (2, 3)
    glm::mat4 model4 = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 3.0f, 0.0f));
    model4 = model4 * model3; // T * R * S

    glLineWidth(2.5f);

    // 5. Головний цикл
    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        GLint mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
        GLint colorLoc = glGetUniformLocation(shaderProgram, "objectColor");

        // Малюємо Стан 1 (Початковий) - Сірий
        glm::mat4 mvp1 = projection * model1;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp1));
        glUniform3f(colorLoc, 0.6f, 0.6f, 0.6f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        // Малюємо Стан 2 (Тільки розтяг) - Блакитний
        glm::mat4 mvp2 = projection * model2;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp2));
        glUniform3f(colorLoc, 0.2f, 0.6f, 0.9f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        // Малюємо Стан 3 (Розтяг + Поворот) - Помаранчевий
        glm::mat4 mvp3 = projection * model3;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp3));
        glUniform3f(colorLoc, 0.9f, 0.5f, 0.1f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        // Малюємо Стан 4 (Фінальний) - Зелений
        glm::mat4 mvp4 = projection * model4;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp4));
        glUniform3f(colorLoc, 0.1f, 0.7f, 0.2f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
}