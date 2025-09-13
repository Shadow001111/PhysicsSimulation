#include "InputManager.h"

GLFWwindow* InputManager::window = nullptr;
std::vector<InputManager::MouseClick> InputManager::mouseClicks;
std::vector<InputManager::KeyAction> InputManager::keyActions;

void InputManager::initialize(GLFWwindow* window)
{
	InputManager::window = window;
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetKeyCallback(window, keyCallback);
}

const std::vector<InputManager::MouseClick>& InputManager::getMouseClicks()
{
	return mouseClicks;
}

const std::vector<InputManager::KeyAction>& InputManager::getKeyActions()
{
	return keyActions;
}

void InputManager::clearInputs()
{
	mouseClicks.clear();
	keyActions.clear();
}

bool InputManager::isMouseButtonPressed(int button)
{
	return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

bool InputManager::isKeyPressed(int key)
{
	return glfwGetKey(window, key) == GLFW_PRESS;
}

void InputManager::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	mouseClicks.emplace_back(button, action, mods, xpos, ypos);
}

void InputManager::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	keyActions.emplace_back(key, scancode, action, mods);
}


InputManager::MouseClick::MouseClick(int button, int action, int mods, double xpos, double ypos)
	: button(button), action(action), mods(mods), xpos(xpos), ypos(ypos)
{
}

bool InputManager::MouseClick::isLeftButton() const
{
	return button == 0;
}

bool InputManager::MouseClick::isRightButton() const
{
	return button == 1;
}

bool InputManager::MouseClick::isMiddleButton() const
{
	return button == 2;
}

bool InputManager::MouseClick::isPressed() const
{
	return action == 1;
}

bool InputManager::MouseClick::isReleased() const
{
	return action == 0;
}


InputManager::KeyAction::KeyAction(int key, int scancode, int action, int mods)
	: key(key), scancode(scancode), action(action), mods(mods)
{
}

bool InputManager::KeyAction::isPressed() const
{
	return action == GLFW_PRESS;
}

bool InputManager::KeyAction::isReleased() const
{
	return action == GLFW_RELEASE;
}

bool InputManager::KeyAction::isRepeated() const
{
	return action == GLFW_REPEAT;
}
