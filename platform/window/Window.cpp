#include "Window.h"

// Dependencies | std
#include <cassert>

namespace it {
	// class Window
	
	// Static | public
	
	// Properties
	std::string Window::s_defaultTitle{ "Title" };
	int Window::s_defaultX{ 200 };
	int Window::s_defaultY{ 200 };
	int Window::s_defaultWidth{ 800 };
	int Window::s_defaultHeight{ 600 };
	bool Window::s_defaultVisibility{ true };
	
	// Object | public
	
	// Operators | equality
	bool Window::operator==(const Window& other) const {
		return this == &other;
	}
	
	// Getters
	std::string Window::getTitle() const {
		return title;
	}
	int Window::getX() const {
		return x;
	}
	int Window::getY() const {
		return y;
	}
	int Window::getWidth() const {
		return width;
	}
	int Window::getHeight() const {
		return height;
	}
	bool Window::getVisible() const {
		return visible;
	}
	
	// Functions | Feeders
	void Window::feedTitle(const std::string& title) {
		this->title = title;
	}
	void Window::feedPosition(int x, int y) {
		this->x = x;
		this->y = y;
	}
	void Window::feedSize(int width, int height) {
		this->width = width;
		this->height = height;
	}
	void Window::feedVisible(bool visible) {
		this->visible = visible;
	}
}
