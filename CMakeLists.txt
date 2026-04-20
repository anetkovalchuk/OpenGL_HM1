cmake_minimum_required(VERSION 3.10)
project(OpenGL_Start)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(external)

find_package(OpenGL REQUIRED)

set(libs
        ${OPENGL_LIBRARY}
        glfw
        glad
        glm
)


add_executable(${PROJECT_NAME}
        src/main.cpp
        hm1/task1/task1.h
        hm1/task1/task1.cpp
        hm1/task2/task2.h
        hm1/task2/task2.cpp
        hm1/task3/task3.cpp
        hm1/task3/task3.h
        hm1/task4/task4.cpp
        hm1/task4/task4.h
        hm1/task5/task5.h
        hm1/task5/task5.cpp
        hm1/task6/task6.h
        hm1/task6/task6.cpp
        hm1/task7/task7.h
        hm1/task7/task7.cpp
        hm1/task8/task8.cpp
        hm1/task8/task8.h
        hm1/task9/task9.cpp
        hm1/task9/task9.h
        hm1/task10/task10.cpp
        hm1/task10/task10.h
        hm1/task11/task11.cpp
        hm1/task11/task11.h
        hm1/task12/task12.cpp
        hm1/task12/task12.h
        hm1/task13/task13.h
        hm1/task13/task13.cpp
        hm1/task14/task14.cpp
        hm1/task14/task14.h
)
target_link_libraries(${PROJECT_NAME} ${libs})