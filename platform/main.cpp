// Dependencies | std
#include <iostream>
#include <chrono>
#include <thread>

// Dependencies | it
#include "backend/glfw/PlatformGLFW.h"
#include "backend/glfw/WindowGLFW.h"

void testWindowing() {
	it::WindowBackendGLFW windowBackendGLFW{};
	bool success = windowBackendGLFW.init();
	if (!success) {
		std::cerr << "Error: Failed to init glfw." << std::endl;
		return;
	}

	it::WindowGLFW windowGLFW{};
	windowGLFW.create();
	windowGLFW.setVisible(true);
	windowGLFW.onMaximize = [](bool maximize) {
		std::cout << (maximize ? "maximized" : "minimized") << std::endl;
	};
	
	std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point currentTime = startTime;
	std::chrono::steady_clock::time_point stopTime = startTime + std::chrono::seconds(3);
	std::chrono::nanoseconds sleepTime = std::chrono::milliseconds(16);
	while (currentTime < stopTime) {
		

		windowGLFW.makeContextCurrent();
		windowGLFW.swapBuffers();
		windowBackendGLFW.update();
		std::this_thread::sleep_for(sleepTime);
		currentTime = std::chrono::steady_clock::now();
	}
}

int main(int argc, char* argv[]) {
	std::cout << "Program operating." << std::endl;

	testWindowing();

	std::cout << "Program terminating." << std::endl;
	return 0;
}
