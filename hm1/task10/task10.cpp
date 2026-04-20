#include "task10.h"

const char* vertexShaderSource10 = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 MVP;
void main() {
    gl_Position = MVP * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource10 = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 objectColor;
void main() {
    FragColor = vec4(objectColor, 1.0);
}
)";

static glm::mat4 getPivotScaleMatrix(float px, float py, float scaleX, float scaleY) {
    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, glm::vec3(px, py, 0.0f));
    m = glm::scale(m, glm::vec3(scaleX, scaleY, 1.0f));
    m = glm::translate(m, glm::vec3(-px, -py, 0.0f));
    return m;
}

static glm::mat4 getPivotRotationMatrix(float px, float py, float angleDegrees) {
    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, glm::vec3(px, py, 0.0f));
    m = glm::rotate(m, glm::radians(angleDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
    m = glm::translate(m, glm::vec3(-px, -py, 0.0f));
    return m;
}

void runTask10() {
    if (!glfwInit()) return;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Task 10: Matrix Multiplication Order", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return;

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource10, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource10, NULL);
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

    // Pivot (0.5, 0.5)
    float pivotVertex[] = { 0.5f, 0.5f, 0.0f };
    unsigned int pivotVBO, pivotVAO;
    glGenVertexArrays(1, &pivotVAO);
    glGenBuffers(1, &pivotVBO);
    glBindVertexArray(pivotVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pivotVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pivotVertex), pivotVertex, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Проекція: X[-2, 5], Y[-4, 3]
    glm::mat4 projection = glm::ortho(-2.0f, 5.0f, -4.0f, 3.0f, -1.0f, 1.0f);
    
    //Обчислення матриць
    glm::mat4 translateMat = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 0.0f));
    glm::mat4 scalePivotMat = getPivotScaleMatrix(0.5f, 0.5f, 2.0f, 2.0f);
    glm::mat4 rotatePivotMat = getPivotRotationMatrix(0.5f, 0.5f, 30.0f);

    // 1. Масштабування -> Обертання -> Зсув (Множимо: T * R * S)
    glm::mat4 modelOrder1 = translateMat * rotatePivotMat * scalePivotMat;

    // 2. Зсув -> Масштабування -> Обертання (Множимо: R * S * T)
    glm::mat4 modelOrder2 = rotatePivotMat * scalePivotMat * translateMat;

    // 3. Масштабування -> Зсув -> Обертання (Множимо: R * T * S)
    glm::mat4 modelOrder3 = rotatePivotMat * translateMat * scalePivotMat;

    glLineWidth(2.5f);
    glPointSize(10.0f);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        GLint mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
        GLint colorLoc = glGetUniformLocation(shaderProgram, "objectColor");

        glm::mat4 currentMVP;
        glBindVertexArray(VAO);

        // Початковий (Сірий)
        currentMVP = projection * glm::mat4(1.0f);
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(currentMVP));
        glUniform3f(colorLoc, 0.7f, 0.7f, 0.7f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        // Порядок 1 (Блакитний)
        currentMVP = projection * modelOrder1;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(currentMVP));
        glUniform3f(colorLoc, 0.2f, 0.6f, 1.0f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        // Порядок 2 (Зелений)
        currentMVP = projection * modelOrder2;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(currentMVP));
        glUniform3f(colorLoc, 0.2f, 0.8f, 0.2f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        // Порядок 3 (Помаранчевий)
        currentMVP = projection * modelOrder3;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(currentMVP));
        glUniform3f(colorLoc, 1.0f, 0.5f, 0.0f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        // Pivot-точкa
        glBindVertexArray(pivotVAO);
        currentMVP = projection * glm::mat4(1.0f); 
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(currentMVP));
        glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f); // Червона
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