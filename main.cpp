#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Constants.h"
#include "Game.h"

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

    // Lock mouse cursor to screen centre
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Game game(window, glm::vec3(BLOCK_SIZE * 25, BLOCK_SIZE * 50, BLOCK_SIZE * 25));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, game.texture);
    
    while (!(glfwWindowShouldClose(window))) {

        game.process_input(window);

        // Rendering
        glClearColor(126.0f/255.0f, 192.0f/255.0f, 255.0f/255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        game.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    };

    return 0;
}