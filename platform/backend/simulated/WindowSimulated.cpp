#include "WindowSimulated.h"

namespace it {
    // class WindowSimulated

    // Object | public

    // Operators | equality
    bool WindowSimulated::operator==(const WindowSimulated& other) const {
        return this == &other;
    }

    // Setters | Window
    void WindowSimulated::setTitle(const std::string& title) {
        this->title = title;
    }
    void WindowSimulated::setPosition(int x, int y) {
        this->x = x;
        this->y = y;
    }
    void WindowSimulated::setSize(int width, int height) {
        this->width = width;
        this->height = height;
    }
    void WindowSimulated::setVisible(bool visible) {
        this->visible = visible;
    }

    // Functions | Window
    bool WindowSimulated::create() {
        return true;
    }
	void WindowSimulated::swapBuffers() {

    }
	void WindowSimulated::makeContextCurrent() {

    }

}
