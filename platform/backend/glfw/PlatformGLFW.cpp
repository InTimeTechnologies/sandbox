#include "PlatformGLFW.h"

// Dependencies | glfw
#include <GLFW/glfw3.h>

namespace it {
	// class WindowBackendGLFW

	// Object | public

	// Constructor / Destructor
	WindowBackendGLFW::~WindowBackendGLFW() {
		deinit();
	}

	// Getters | WindowBackend
	bool WindowBackendGLFW::getIsInit() const {
		return isInit;
	}

	// Functions | WindowBackend
	bool WindowBackendGLFW::init() {
		isInit = static_cast<bool>(glfwInit());
		return isInit;
	}
	void WindowBackendGLFW::deinit() {
		if (isInit)
			glfwTerminate();
	}
	void WindowBackendGLFW::update() {
		glfwPollEvents();
	}
}
