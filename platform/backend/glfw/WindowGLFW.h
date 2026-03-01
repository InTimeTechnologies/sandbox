#pragma once

// Dependencies | it
#include "../../window/Window.h"

namespace it {
	class WindowGLFW : public Window {
		// Object
		private:
			// Protected
			void* backendWindowPointer{ nullptr };

		public:
			// Constructor / Destructor
			WindowGLFW() = default;
			WindowGLFW(const WindowGLFW& other) = delete;
			WindowGLFW(WindowGLFW&& other) = delete;
			virtual ~WindowGLFW() = default;

			// Operators | assignment
			WindowGLFW& operator=(const WindowGLFW& other) = delete;
			WindowGLFW& operator=(WindowGLFW&& other) = delete;

			// Operators | equality
			bool operator==(const WindowGLFW& other) const;

			// Setters | Window
			void setTitle(const std::string& title) override;
			void setPosition(int x, int y) override;
			void setSize(int width, int height) override;
			void setVisible(bool visible);

			
			// Functions | Window
			bool create() override;

			// Functions
			void swapBuffers() override;
			void makeContextCurrent() override;
	};
}
