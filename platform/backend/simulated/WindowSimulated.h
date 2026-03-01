#pragma once

// Dependencies | Window
#include "../../window/Window.h"

namespace it {
	class WindowSimulated : public Window {
		// Object
		public:
			// Constructor / Destructor
			WindowSimulated() = default;
			WindowSimulated(const WindowSimulated& other) = delete;
			WindowSimulated(WindowSimulated&& other) = delete;
			virtual ~WindowSimulated() = default;

			// Operators | assignment
			WindowSimulated& operator=(const WindowSimulated& other) = delete;
			WindowSimulated& operator=(WindowSimulated&& other) = delete;

			// Operators | equality
			bool operator==(const WindowSimulated& other) const;

			// Setters | Window
			void setTitle(const std::string& title) override;
			void setPosition(int x, int y) override;
			void setSize(int width, int height) override;
			void setVisible(bool visible);

			// Functions | Window
			bool create() override;
			void swapBuffers() override;
			void makeContextCurrent() override;
	};
}
