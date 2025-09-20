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

        MouseClick(int button, int action, int mods, double xpos, double ypos);

        bool isLeftButton() const;
        bool isRightButton() const;
        bool isMiddleButton() const;

        bool isPressed() const;
        bool isReleased() const;
    };

    struct KeyAction
    {
        int key, scancode, action, mods;

        KeyAction(int key, int scancode, int action, int mods);

        bool isPressed() const;
        bool isReleased() const;
        bool isRepeated() const;
    };

    static void initialize(GLFWwindow* window);

    static const std::vector<MouseClick>& getMouseClicks();
    static const std::vector<KeyAction>& getKeyActions();
    static void clearInputs();

    static bool isMouseButtonPressed(int button);
    static bool isKeyPressed(int key);
private:
    static GLFWwindow* window;
    static std::vector<MouseClick> mouseClicks;
    static std::vector<KeyAction> keyActions;

    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

