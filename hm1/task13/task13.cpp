#include "task13.h"

const char* vertexShaderSource13 = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 MVP;
void main() {
    gl_Position = MVP * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource13 = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 objectColor;
void main() {
    FragColor = vec4(objectColor, 1.0);
}
)";

void runTask13() {
    if (!glfwInit()) return;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Task 13: Matrix Decomposition", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return;

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource13, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource13, NULL);
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

    // Ортографічна проекція, налаштована для охоплення всієї фігури 
    // X від -2 до 4, Y від -1 до 6
    glm::mat4 projection = glm::ortho(-2.0f, 4.0f, -1.0f, 6.0f, -1.0f, 1.0f);

    
    // 1. Масштабування S(2, 3)
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 3.0f, 1.0f));
    
    // 2. Поворот R(45°) навколо початку координат
    glm::mat4 rotateMat = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    
    // 3. Переміщення T(1, 1)
    glm::mat4 translateMat = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.0f));

    glm::mat4 finalTransformation = translateMat * rotateMat * scaleMat;

    glLineWidth(2.5f);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        GLint mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
        GLint colorLoc = glGetUniformLocation(shaderProgram, "objectColor");

        glBindVertexArray(VAO);

        // Малюємо початковий квадрат (Світло-сірий)
        glm::mat4 mvpInitial = projection * glm::mat4(1.0f);
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpInitial));
        glUniform3f(colorLoc, 0.7f, 0.7f, 0.7f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        // Малюємо трансформований квадрат (Зелений)
        glm::mat4 mvpTransformed = projection * finalTransformation;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpTransformed));
        glUniform3f(colorLoc, 0.1f, 0.8f, 0.2f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
}