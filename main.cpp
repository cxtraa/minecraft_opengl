#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Constants.h"
#include "Game.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>

int main()
{   
    // GLFW setup
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Minecraft OpenGL C++", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // GLAD setup
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Game game(window, glm::vec3(BLOCK_SIZE * 25, BLOCK_SIZE * 25, BLOCK_SIZE * 25), true);
    glfwSetInputMode(window, GLFW_CURSOR, game.gameState == InGame ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, game.texture);
    
    while (!(glfwWindowShouldClose(window))) {

        glClearColor(126.0f / 255.0f, 192.0f / 255.0f, 255.0f / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwPollEvents();

        game.process_input();
        game.draw();

        glfwSwapBuffers(window);
    };

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}