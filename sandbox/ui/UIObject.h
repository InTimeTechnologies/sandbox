#pragma once

// Dependencies | std
#include <list>
#include <typeinfo>

namespace it {
	namespace ui {
		class Object {
			// Object
			private:
				// Properties
				std::list<Object*> objects{};

				// Getters
				std::list<Object*> getObjects() const;

				// Functions
				virtual const std::type_info& typeInfo() const = 0;
		};
	}
}
