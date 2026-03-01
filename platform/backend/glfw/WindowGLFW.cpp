#include "WindowGLFW.h"

// Dependencies | std
#include <iostream> // TODO: remove

// Dependencies | glfw
#include <GLFW/glfw3.h>

namespace it {
	static void s_windowSizeCallback(GLFWwindow* glfwWindow, int width, int height) {
		WindowGLFW* windowGLFW = reinterpret_cast<WindowGLFW*>(glfwGetWindowUserPointer(glfwWindow));
		windowGLFW->feedSize(width, height);
		if (windowGLFW->onResize)
			windowGLFW->onResize(width, height);
	}
	static void s_windowPositionCallback(GLFWwindow* glfwWindow, int x, int y) {
		WindowGLFW* windowGLFW = reinterpret_cast<WindowGLFW*>(glfwGetWindowUserPointer(glfwWindow));
		windowGLFW->feedPosition(x, y);
		if (windowGLFW->onMove)
			windowGLFW->onMove(x, y);
	}
	static void s_windowCloseCallback(GLFWwindow* glfwWindow) {
		WindowGLFW* windowGLFW = reinterpret_cast<WindowGLFW*>(glfwGetWindowUserPointer(glfwWindow));
		if (windowGLFW->onClose)
			windowGLFW->onClose();
	}
	static void s_windowFocusCallback(GLFWwindow* glfwWindow, int focused) {
		WindowGLFW* windowGLFW = reinterpret_cast<WindowGLFW*>(glfwGetWindowUserPointer(glfwWindow));
		if (windowGLFW->onFocus)
			windowGLFW->onFocus(static_cast<bool>(focused));
	}
	static void s_windowIconifyCallback(GLFWwindow* glfwWindow, int iconify) {
		WindowGLFW* windowGLFW = reinterpret_cast<WindowGLFW*>(glfwGetWindowUserPointer(glfwWindow));
		if (windowGLFW->onIconify)
			windowGLFW->onIconify(static_cast<bool>(iconify));
	}
	static void s_windowRefreshCallback(GLFWwindow* glfwWindow) {
		WindowGLFW* windowGLFW = reinterpret_cast<WindowGLFW*>(glfwGetWindowUserPointer(glfwWindow));
		if (windowGLFW->onRefresh)
			windowGLFW->onRefresh();
	}
	static void s_windowMaximizeCallback(GLFWwindow* glfwWindow, int maximized) {
		WindowGLFW* windowGLFW = reinterpret_cast<WindowGLFW*>(glfwGetWindowUserPointer(glfwWindow));
		if (windowGLFW->onMaximize)
			windowGLFW->onMaximize(static_cast<bool>(maximized));
	}
	static void s_contentScaleCallback(GLFWwindow* glfwWindow, float xScale, float yScale) {
		WindowGLFW* windowGLFW = reinterpret_cast<WindowGLFW*>(glfwGetWindowUserPointer(glfwWindow));
		if (windowGLFW->onScale)
			windowGLFW->onScale(xScale, yScale);
	}

	// class WindowGLFW

	// Object | public

	// Operators | equality
	bool WindowGLFW::operator==(const WindowGLFW& other) const {
		return this == &other;
	}

	// Setters | Window
	void WindowGLFW::setTitle(const std::string& title) {
		GLFWwindow* glfwWindow = reinterpret_cast<GLFWwindow*>(this->backendWindowPointer);
		glfwSetWindowTitle(glfwWindow, title.c_str());
		this->title = title;
	}
	void WindowGLFW::setPosition(int x, int y) {
		GLFWwindow* glfwWindow = reinterpret_cast<GLFWwindow*>(this->backendWindowPointer);
		glfwSetWindowPos(glfwWindow, x, y);
		this->x = x;
		this->y = y;
	}
	void WindowGLFW::setSize(int width, int height) {
		GLFWwindow* glfwWindow = reinterpret_cast<GLFWwindow*>(this->backendWindowPointer);
		glfwSetWindowSize(glfwWindow, width, height);
		this->width = width;
		this->height = height;
	}
	void WindowGLFW::setVisible(bool visible) {
		GLFWwindow* glfwWindow = reinterpret_cast<GLFWwindow*>(this->backendWindowPointer);
		if (visible)
			glfwShowWindow(glfwWindow);
		else
			glfwHideWindow(glfwWindow);
		this->visible = visible;
	}

	// Functions | Window
	bool WindowGLFW::create() {
		// Create window
		GLFWwindow* glfwWindow = glfwCreateWindow(s_defaultWidth, s_defaultHeight, s_defaultTitle.c_str(), nullptr, nullptr);
		if (!glfwWindow)
			return false;

		// Link this to window
		glfwSetWindowUserPointer(glfwWindow, this);
		backendWindowPointer = glfwWindow;

		// Set callbacks
		glfwSetWindowSizeCallback(glfwWindow, s_windowSizeCallback);
		glfwSetWindowPosCallback(glfwWindow, s_windowPositionCallback);
		glfwSetWindowCloseCallback(glfwWindow, s_windowCloseCallback);
		glfwSetWindowFocusCallback(glfwWindow, s_windowFocusCallback);
		glfwSetWindowIconifyCallback(glfwWindow, s_windowIconifyCallback);
		glfwSetWindowRefreshCallback(glfwWindow, s_windowRefreshCallback);
		glfwSetWindowMaximizeCallback(glfwWindow, s_windowMaximizeCallback);
		glfwSetWindowContentScaleCallback(glfwWindow, s_contentScaleCallback);

		// Success
		return true;
	}

	// Functions
	void WindowGLFW::swapBuffers() {
		GLFWwindow* glfwWindow = reinterpret_cast<GLFWwindow*>(this->backendWindowPointer);
		glfwSwapBuffers(glfwWindow);
	}
	void WindowGLFW::makeContextCurrent() {
		GLFWwindow* glfwWindow = reinterpret_cast<GLFWwindow*>(this->backendWindowPointer);
		glfwMakeContextCurrent(glfwWindow);
	}
}
