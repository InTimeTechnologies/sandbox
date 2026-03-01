#pragma once

// Dependencies | std
#include <string>
#include <functional>

namespace it {
	class Window {
		// Friends
		friend class WindowManager;
		
		// Static
		public:
			// Properties
			static std::string s_defaultTitle;
			static int s_defaultX, s_defaultY;
			static int s_defaultWidth, s_defaultHeight;
			static bool s_defaultVisibility;
		
		// Object
		protected:
			// Properties
			std::string title{""};
			int x{ 0 }, y{ 0 };
			int width{ 0 }, height{ 0 };
			bool visible{ false };
		
		public:
			// Properties
			std::function<void(int, int)> onResize;
			std::function<void(int, int)> onMove;
			std::function<void()> onClose;
			std::function<void(bool)> onFocus;
			std::function<void(bool)> onIconify;
			std::function<void()> onRefresh;
			std::function<void(bool)> onMaximize;
			std::function<void(float, float)> onScale;

			// Constructor / Destructor
			Window() = default;
			Window(const Window& other) = delete;
			Window(Window&& other) = delete;
			virtual ~Window() = default;
		
			// Operators | assignment
			Window& operator=(const Window& other) = delete;
			Window& operator=(Window&& other) = delete;
			
			// Operators | equality
			bool operator==(const Window& other) const;
			
			// Getters
			std::string getTitle() const;
			int getX() const;
			int getY() const;
			int getWidth() const;
			int getHeight() const;
			bool getVisible() const;
			
			// Setters
			virtual void setTitle(const std::string& title) = 0;
			virtual void setPosition(int x, int y) = 0;
			virtual void setSize(int width, int height) = 0;
			virtual void setVisible(bool visible) = 0;

			// Functions
			virtual bool create() = 0;
			virtual void makeContextCurrent() = 0;
			virtual void swapBuffers() = 0;
					
			// Functions | Feeders
			void feedTitle(const std::string& title);
			void feedPosition(int x, int y);
			void feedSize(int width, int height);
			void feedVisible(bool visible);
	};
}
