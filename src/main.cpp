#define GLFW_INCLUDE_NONE
#define GLM_ENABLE_EXPERIMENTAL
#include <GLFW/glfw3.h>
#include <render/rendergl.h>
#include <world/world.h>
#include <glm/glm.hpp>
#include <thread>
#include <iostream>

#include <string>
#include <utils/fileload.h>

#include <chrono>

int main(void)
{
    std::cout << "STARTS" << std::endl;
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()){
        std::cout << "INIT FAILED" << std::endl;
        return -1;
    };

    std::cout << "INIT SUCCEED" << std::endl;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    std::cout << "WINDOW CREATED" << std::endl;

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    unsigned char sacrifice = 0;
    ImageRenderInfo& tex = loadtexture(1, 1, &sacrifice, 0);

    std::cout << "CONTEXT CREATED" << std::endl;

    world_load("./map.fmap");

    render_Camera_change_transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f, -30.0f)));

    render_Camera_change_perspective(glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 1000.0f));

    std::thread worldthread(world_init);
    worldthread.detach();

    //std::this_thread::sleep_for(std::chrono::seconds(2));


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glfwSwapBuffers(window);
        render_tick(&window);
        std::this_thread::sleep_for(std::chrono::seconds(2));

        /* Poll for and process events */
        glfwPollEvents();
    }
    render_deinit();


    std::this_thread::sleep_for(std::chrono::seconds(200));
    glfwTerminate();
    return 0;
}