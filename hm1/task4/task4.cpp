#include "task4.h"

const char* vertexShaderSource4 = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 MVP;
void main() {
    gl_Position = MVP * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource4 = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 objectColor;
void main() {
    FragColor = vec4(objectColor, 1.0);
}
)";

void runTask4() {
    // 1. Ініціалізація GLFW
    if (!glfwInit()) return;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Task 4: Scale and Rotate (Line Loop)", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return;

    // 2. Компіляція шейдерів
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource4, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource4, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 3. Геометрія: Квадрат (0,0) - (1,1)
    float vertices[] = {
        0.0f, 0.0f, 0.0f, // P1 (нижня ліва)
        1.0f, 0.0f, 0.0f, // P2 (нижня права)
        1.0f, 1.0f, 0.0f, // P3 (верхня права)
        0.0f, 1.0f, 0.0f  // P4 (верхня ліва)
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
    glm::mat4 projection = glm::ortho(-4.0f, 4.0f, -1.0f, 5.0f, -1.0f, 1.0f);

    glm::mat4 model1 = glm::mat4(1.0f);
    glm::mat4 model2 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));

    // R * S: спочатку масштабування, потім поворот
    glm::mat4 model3 = glm::rotate(glm::mat4(1.0f), glm::radians(60.0f), glm::vec3(0, 0, 1));
    model3 = model3 * model2;
    glLineWidth(2.0f);

    // 5. Цикл рендерингу
    while (!glfwWindowShouldClose(window)) {
        // Білий фон
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        GLint mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
        GLint colorLoc = glGetUniformLocation(shaderProgram, "objectColor");

        //Стан 1: Сірий контур
        glm::mat4 mvp1 = projection * model1;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp1));
        glUniform3f(colorLoc, 0.4f, 0.4f, 0.4f);
        // Використовуємо GL_LINE_LOOP замість GL_TRIANGLES
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        //Стан 2: Блакитний контур
        glm::mat4 mvp2 = projection * model2;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp2));
        glUniform3f(colorLoc, 0.2f, 0.6f, 0.9f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        //Стан 3: Зелений контур
        glm::mat4 mvp3 = projection * model3;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp3));
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