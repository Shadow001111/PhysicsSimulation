#pragma once
#include <GLFW/glfw3.h>
#include <vector>
#include <utility>

class InputManager
{
public:
    struct MouseClick
    {
        int button;
        int action;
        int mods;
        double xpos;
        double ypos;

        bool isLeftButton() const;
        bool isRightButton() const;
        bool isMiddleButton() const;

        bool isPressed() const;
        bool isReleased() const;
    };

    static void initialize(GLFWwindow* window);

    static const std::vector<MouseClick>& getMouseClicks();

    static void clearInputs();
private:
    static std::vector<MouseClick> mouseClicks;
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
};

