#pragma once

// Dependencies | it
#include "Platform.h"

namespace it {
	class WindowBackendGLFW : public Platform {
		// Object
		public:
			// Constructor / Destructor
			WindowBackendGLFW() = default;
			virtual ~WindowBackendGLFW();

			// Getters | WindowBackend
			bool getIsInit() const override;

			// Functions | WindowBackend
			bool init() override;
			void deinit() override;
			void update() override;
	};
}
