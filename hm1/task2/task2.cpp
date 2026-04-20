#include "task2.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int runTask2()
{
    // 1. Ініціалізація GLFW
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Task 2: Stretch and Rotate", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // 2. Ініціалізація GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 3. Вершинний шейдер
    auto vertexShaderCode = R"(
    #version 330 core
    layout(location = 0) in vec2 aPos;
    
    uniform mat4 model;      
    uniform mat4 projection; 
    
    void main() {
        gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
    }
    )";
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderCode, nullptr);
    glCompileShader(vertexShader);

    // 4. Фрагментний шейдер
    auto fragmentShaderCode = R"(
    #version 330 core
    out vec4 FragColor;
    uniform vec3 objectColor; 
    void main() {
        FragColor = vec4(objectColor, 1.0);
    }
    )";
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, nullptr);
    glCompileShader(fragmentShader);

    // 5. Створення програми з шейдерів
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 6. Дані вершин: Квадрат з діагоналлю (0,0) - (1,1)
    float vertices[] = {
        0.0f, 0.0f, 
        1.0f, 0.0f, 
        1.0f, 1.0f, 
        
        1.0f, 1.0f, 
        0.0f, 1.0f, 
        0.0f, 0.0f  
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint colorLoc = glGetUniformLocation(shaderProgram, "objectColor");

    // Налаштовуємо камеру так, щоб було добре видно центр координат і фігуру
    glm::mat4 projection = glm::ortho(-2.0f, 4.0f, -1.0f, 4.0f, -1.0f, 1.0f);
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // 7. Головний цикл рендерингу
    while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // ПОЧАТКОВИЙ СТАН: Оригінальний квадрат (Сірий)
        glm::mat4 modelOriginal = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelOriginal));
        glUniform3f(colorLoc, 0.6f, 0.6f, 0.6f);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // ФІНАЛЬНИЙ СТАН: Розтяг по осі Х у 2 рази, потім поворот на 45 градусів
        // Важливо: в GLM трансформації записуються у зворотному порядку до їхнього математичного застосування!
        glm::mat4 modelFinal = glm::mat4(1.0f);
        modelFinal = glm::rotate(modelFinal, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        modelFinal = glm::scale(modelFinal, glm::vec3(2.0f, 1.0f, 1.0f));
        
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelFinal));
        glUniform3f(colorLoc, 0.8f, 0.2f, 0.2f); // Червоний
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 8. Звільнення пам'яті
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}