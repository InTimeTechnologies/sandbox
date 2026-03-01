#pragma once

namespace it {
	class Platform {
		// Object
		protected:
			// Properties
			bool isInit{ false };

		public:
			// Constructor / Destructor
			Platform() = default;
			virtual ~Platform() = default;

			// Getters
			virtual bool getIsInit() const = 0;

			// Functions
			virtual bool init() = 0;
			virtual void deinit() = 0;
			virtual void update() = 0;
	};

	class WindowEventForwarder {
		// Object
		public:
			// Properties
			bool windowEventForwardingEnabled{ true };

			// Functions
			virtual void forwardWindowEvents() = 0;
	};

	class MouseEventForwarder {
		// Object
		public:
			// Properties
			bool mouseEventForwardingEnabled{ true };

			// Functions
			virtual void forwardMouseEvents() = 0;
	};

	class KeyboardEventForwarder {
		// Object
		public:
			// Properties
			bool keyboardEventForwardingEnabled{ true };

			// Functions
			virtual void forwardKeyboardEvents() = 0;
	};

	class JoystickEventForwarder {
		// Object
		public:
			// Properties
			bool joystickEventForwardingEnabled{ true };

			// Functions
			virtual void forwardJoystickEvents() = 0;
	};
}
