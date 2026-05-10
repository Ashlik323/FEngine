#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <render/rendergl.h>
#include <world/world.h>
#include <glm/glm.hpp>
#include <thread>

#include <string>
#include <utils/fileload.h>

#include <chrono>

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    std::this_thread::sleep_for(std::chrono::seconds(2));
    world_load("./map.fmap");

    render_Camera_change_transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f, -30.0f)));

    render_Camera_change_perspective(glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 1000.0f));

    std::thread worldthread(world_init);
    worldthread.detach();

    //std::this_thread::sleep_for(std::chrono::seconds(2));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        render_tick(&window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    render_deinit();

    glfwTerminate();
    return 0;
}