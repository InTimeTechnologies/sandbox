#pragma once

// Dependencies | std
#include <filesystem>

// Dependencies | glm
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace it {
	// Forward declarations
	class ImageGray;
	class ImageGrayAlpha;
	class ImageRGB;
	class ImageRGBA;

	// Enums
	enum class DynamicRange {
		UNKNOWN = -1,
		LDR,
		HDR
	};
	enum class Format {
		UNKNOWN = -1,
		JPEG,
		PNG,
		TGA,
		BMP,
		PSD,
		GIF,
		HDR,
		PIC,
		PNM
	};

	// Classes
	class ImageGray {
		// Friends
		friend class ImageGrayAlpha;
		friend class ImageRGB;
		friend class ImageRGBA;

		// Static
		public:
			// Properties
			static const int CHANNELS{ 1 };

			// class
			struct RowView {
				// Object

				// Properties
				unsigned char* data{ nullptr };
				int width{ 0 };

				// Operators | member access
				unsigned char& operator[](size_t y);
				const unsigned char& operator[](size_t y) const;
			};

		// Object
		private:
			// Properties
			int width{ 0 };
			int height{ 0 };
			unsigned char* data{ nullptr };

		public:
			// Constructor / Destructor
			ImageGray() = default;
			ImageGray(int width, int height);
			ImageGray(const std::filesystem::path& path);
			ImageGray(const ImageGray& other);
			ImageGray(const ImageGrayAlpha& other, bool factorInAlpha = false);
			ImageGray(const ImageRGB& other);
			ImageGray(const ImageRGBA& other, bool factorInAlpha = false);
			ImageGray(ImageGray&& other) noexcept;
			~ImageGray();

			// Operators | assignment
			ImageGray& operator=(const ImageGray& other);
			ImageGray& operator=(ImageGray&& other) noexcept;
			ImageGray& operator=(const ImageGrayAlpha& other);
			ImageGray& operator=(ImageGrayAlpha&& other) noexcept;
			ImageGray& operator=(const ImageRGB& other);
			ImageGray& operator=(ImageRGB&& other) noexcept;
			ImageGray& operator=(const ImageRGBA& other);
			ImageGray& operator=(ImageRGBA&& other) noexcept;

			// Operators | member access
			RowView operator[](size_t y);
			const RowView operator[](size_t y) const;

			// Getters
			int getWidth() const;
			int getHeight() const;
			int getChannels() const;
			unsigned char* getData() const;

			// Functions | allocation / deallocation
			unsigned char* allocate(int width, int height);
			bool isAllocated() const;
			size_t dataSize() const;
			void free();

			// Functions | file loading (allocates memory) / saving
			bool load(const std::filesystem::path& path, bool flipImageOnLoad = false);
			bool loadFromMemory(const unsigned char* fileInMemory, size_t size, bool flipImageOnLoad = false);
			bool copy(const ImageGray& other);
			bool copy(const ImageGrayAlpha& other, bool factorInAlpha = false);
			bool copy(const ImageRGB& other);
			bool copy(const ImageRGBA& other, bool factorInAlpha = false);
			bool saveAsPNG(const std::filesystem::path& path) const;
			bool saveAsJPEG(const std::filesystem::path& path, int quality = 90) const;
			bool saveAsBMP(const std::filesystem::path& path) const;
			bool saveAsTGA(const std::filesystem::path& path) const;
			bool save(const std::filesystem::path& path, int quality = 90) const;

			// Functions | pixel manipulation
			size_t pixelCount() const;
			unsigned char pixelAt(int x, int y) const;
			bool setPixel(int x, int y, unsigned char pixel);
			bool setPixel(int x, int y, float pixel);
			void fillRect(int x, int y, int width, int height, unsigned char color);
	};
	class ImageGrayAlpha {
		// Friends
		friend class ImageGray;
		friend class ImageRGB;
		friend class ImageRGBA;

		// Static
		public:
			// Properties
			static const int CHANNELS{ 2 };

			// class
			struct RowView {
				// Object

				// Properties
				glm::u8vec2* data{ nullptr };
				int width{ 0 };

				// Operators | member access
				glm::u8vec2& operator[](size_t y);
				const glm::u8vec2& operator[](size_t y) const;
			};

		// Object
		private:
			// Properties
			int width{ 0 };
			int height{ 0 };
			glm::u8vec2* data{ nullptr };

		public:
			// Constructor / Destructor
			ImageGrayAlpha() = default;
			ImageGrayAlpha(int width, int height);
			ImageGrayAlpha(const std::filesystem::path& path);
			ImageGrayAlpha(const ImageGray& other);
			ImageGrayAlpha(const ImageGrayAlpha& other);
			ImageGrayAlpha(const ImageRGB& other);
			ImageGrayAlpha(const ImageRGBA& other);
			ImageGrayAlpha(ImageGrayAlpha&& other) noexcept;
			~ImageGrayAlpha();

			// Operators | assignment
			ImageGrayAlpha& operator=(const ImageGray& other);
			ImageGrayAlpha& operator=(ImageGray&& other) noexcept;
			ImageGrayAlpha& operator=(const ImageGrayAlpha& other);
			ImageGrayAlpha& operator=(ImageGrayAlpha&& other) noexcept;
			ImageGrayAlpha& operator=(const ImageRGB& other);
			ImageGrayAlpha& operator=(ImageRGB&& other) noexcept;
			ImageGrayAlpha& operator=(const ImageRGBA& other);
			ImageGrayAlpha& operator=(ImageRGBA&& other) noexcept;

			// Operators | member access
			RowView operator[](size_t y);
			const RowView operator[](size_t y) const;

			// Getters
			int getWidth() const;
			int getHeight() const;
			int getChannels() const;
			glm::u8vec2* getData() const;

			// Functions | allocation / deallocation
			glm::u8vec2* allocate(int width, int height);
			bool isAllocated() const;
			size_t dataSize() const;
			void free();

			// Functions | file loading / saving
			bool load(const std::filesystem::path& path, bool flipImageOnLoad = false);
			bool loadFromMemory(const unsigned char* fileInMemory, size_t size, bool flipImageOnLoad = false);
			bool copy(const ImageGray& other);
			bool copy(const ImageGrayAlpha& other);
			bool copy(const ImageRGB& other);
			bool copy(const ImageRGBA& other);
			bool saveAsPNG(const std::filesystem::path& path) const;
			bool saveAsJPEG(const std::filesystem::path& path, int quality = 90) const;
			bool saveAsBMP(const std::filesystem::path& path) const;
			bool saveAsTGA(const std::filesystem::path& path) const;
			bool save(const std::filesystem::path& path, int quality = 90) const;

			// Functions | pixel manipulation
			size_t pixelCount() const;
			glm::u8vec2 pixeAt(int x, int y) const;
			bool paintPixel(int x, int y, glm::u8vec2 pixel);
			bool paintPixel(int x, int y, glm::vec2 pixel);
			void fillRect(int rectX, int rectY, int rectWidth, int rectHeight, const glm::u8vec2& color);
	};
	class ImageRGB {
		// Friends
		friend class ImageGray;
		friend class ImageGrayAlpha;
		friend class ImageRGBA;

		// Static
		public:
			// Properties
			static const int CHANNELS{ 3 };

			// class
			struct RowView {
				// Object

				// Properties
				glm::u8vec3* data{ nullptr };
				int width{ 0 };

				// Operators | member access
				glm::u8vec3& operator[](size_t y);
				const glm::u8vec3& operator[](size_t y) const;
			};

		// Object
		private:
			// Properties
			int width{ 0 };
			int height{ 0 };
			glm::u8vec3* data{ nullptr };

		public:
			// Constructor / Destructor
			ImageRGB() = default;
			ImageRGB(int width, int height);
			ImageRGB(const std::filesystem::path& path);
			ImageRGB(const ImageGray& other);
			ImageRGB(const ImageGrayAlpha& other, bool factorInAlpha = false);
			ImageRGB(const ImageRGB& other);
			ImageRGB(const ImageRGBA& other, bool factorInAlpha = false);
			ImageRGB(ImageRGB&& other) noexcept;
			~ImageRGB();

			// Operators | assignment
			ImageRGB& operator=(const ImageGray& other);
			ImageRGB& operator=(ImageGray&& other) noexcept;
			ImageRGB& operator=(const ImageGrayAlpha& other);
			ImageRGB& operator=(ImageGrayAlpha&& other) noexcept;
			ImageRGB& operator=(const ImageRGB& other);
			ImageRGB& operator=(ImageRGB&& other) noexcept;
			ImageRGB& operator=(const ImageRGBA& other);
			ImageRGB& operator=(ImageRGBA&& other) noexcept;

			// Operators | member access
			RowView operator[](size_t y);
			const RowView operator[](size_t y) const;

			// Getters
			int getWidth() const;
			int getHeight() const;
			int getChannels() const;
			glm::u8vec3* getData() const;

			// Functions | allocation / deallocation
			glm::u8vec3* allocate(int width, int height);
			bool isAllocated() const;
			size_t dataSize() const;
			void free();
			
			// Functions | file loading (allocates memory) / saving
			bool load(const std::filesystem::path& path, bool flipImageOnLoad = false);
			bool loadFromMemory(const unsigned char* fileInMemory, size_t size, bool flipImageOnLoad = false);
			bool copy(const ImageGray& other);
			bool copy(const ImageGrayAlpha& other, bool factorInAlpha = false);
			bool copy(const ImageRGB& other);
			bool copy(const ImageRGBA& other, bool factorInAlpha = false);
			bool saveAsPNG(const std::filesystem::path& path) const;
			bool saveAsJPEG(const std::filesystem::path& path, int quality = 90) const;
			bool saveAsBMP(const std::filesystem::path& path) const;
			bool saveAsTGA(const std::filesystem::path& path) const;
			bool save(const std::filesystem::path& path, int quality = 90) const;

			// Functions | pixel manipulation
			size_t getPixelCount() const;
			glm::u8vec3 getPixel(int x, int y) const;
			bool setPixel(int x, int y, glm::u8vec3 pixel);
			bool setPixel(int x, int y, glm::vec3 pixel);
			void fillRect(int rectX, int rectY, int rectWidth, int rectHeight, const glm::u8vec3& color);
	};
	class ImageRGBA {
		// Friends
		friend class ImageGray;
		friend class ImageGrayAlpha;
		friend class ImageRGB;

		// class
		struct RowView {
			// Object

			// Properties
			glm::u8vec4* data{ nullptr };
			int width{ 0 };

			// Operators | member access
			glm::u8vec4& operator[](size_t y);
			const glm::u8vec4& operator[](size_t y) const;
		};

		// Static
		public:
			// Properties
			static const int CHANNELS{ 4 };

		// Object
		private:
			// Properties
			int width{ 0 };
			int height{ 0 };
			glm::u8vec4* data{ nullptr };

		public:
			// Constructor / Destructor
			ImageRGBA() = default;
			ImageRGBA(int width, int height);
			ImageRGBA(const std::filesystem::path& path);
			ImageRGBA(const ImageGray& other);
			ImageRGBA(const ImageGrayAlpha& other);
			ImageRGBA(const ImageRGB& other);
			ImageRGBA(const ImageRGBA& other);
			ImageRGBA(ImageRGBA&& other) noexcept;
			~ImageRGBA();

			// Operators | assignment
			ImageRGBA& operator=(const ImageGray& other);
			ImageRGBA& operator=(ImageGray&& other) noexcept;
			ImageRGBA& operator=(const ImageGrayAlpha& other);
			ImageRGBA& operator=(ImageGrayAlpha&& other) noexcept;
			ImageRGBA& operator=(const ImageRGB& other);
			ImageRGBA& operator=(ImageRGB&& other) noexcept;
			ImageRGBA& operator=(const ImageRGBA& other);
			ImageRGBA& operator=(ImageRGBA&& other) noexcept;

			// Operators | member access
			RowView operator[](size_t y);
			const RowView operator[](size_t y) const;

			// Getters
			int getWidth() const;
			int getHeight() const;
			int getChannels() const;
			glm::u8vec4* getData() const;

			// Functions | allocation / deallocation
			glm::u8vec4* allocate(int width, int height);
			bool isAllocated() const;
			size_t dataSize() const;
			void free();

			// Functions
			bool load(const std::filesystem::path& path, bool flipImageOnLoad = false);
			bool loadFromMemory(const unsigned char* fileInMemory, size_t size, bool flipImageOnLoad = false);
			bool copy(const ImageGray& other);
			bool copy(const ImageGrayAlpha& other);
			bool copy(const ImageRGB& other);
			bool copy(const ImageRGBA& other);
			bool saveAsPNG(const std::filesystem::path& path) const;
			bool saveAsJPEG(const std::filesystem::path& path, int quality = 90) const;
			bool saveAsBMP(const std::filesystem::path& path) const;
			bool saveAsTGA(const std::filesystem::path& path) const;
			bool save(const std::filesystem::path& path, int quality = 90) const;

			// Functions | pixel manipulation
			size_t pixelCount() const;
			glm::u8vec4 pixelAt(int x, int y) const;
			bool paintPixel(int x, int y, glm::u8vec4 pixel);
			bool paintPixel(int x, int y, glm::vec4 pixel);
			void fillRect(int rectX, int rectY, int rectWidth, int rectHeight, const glm::u8vec4& color);
	};

	struct ImageView {
		// Properties
		int width{ 0 };
		int height{ 0 };
		int channels{ 0 };
		unsigned char* data{ nullptr };

		// Constructors | copy / conversions
		ImageView(const ImageGray& othger);
		ImageView(const ImageGrayAlpha& other);
		ImageView(const ImageRGB& other);
		ImageView(const ImageRGBA& other);

		// Operators | conversions
		ImageView& operator=(const ImageGray& other);
		ImageView& operator=(const ImageGrayAlpha& other);
		ImageView& operator=(const ImageRGB& other);
		ImageView& operator=(const ImageRGBA& other);

		// Functions
		size_t pixelCount() const;
		size_t dataSize() const;
		unsigned char* pixelAt(int x, int y) const;
		bool hasData() const;
	};

	struct ImageViewGray {
		// Properties
		int width{ 0 };
		int height{ 0 };
		int channels{ 0 };
		unsigned char* data{ nullptr };

		// Constructors | copy / conversions
		ImageViewGray(const ImageGray& othger);

		// Operators | onversions
		ImageViewGray& operator=(const ImageGray& other);

		// Functions
		size_t pixelCount() const;
		size_t dataSize() const;
		unsigned char pixelAt(int x, int y) const;
		bool hasData() const;
	};

	struct ImageViewGrayAlpha {
		// Properties
		int width{ 0 };
		int height{ 0 };
		int channels{ 0 };
		glm::u8vec2* data{ nullptr };

		// Constructors | copy / conversions
		ImageViewGrayAlpha(const ImageGrayAlpha& other);

		// Operators | onversions
		ImageViewGrayAlpha& operator=(const ImageGrayAlpha& other);

		// Functions
		size_t pixelCount() const;
		size_t dataSize() const;
		glm::u8vec2 pixelAt(int x, int y) const;
		bool hasData() const;
	};

	struct ImageViewRGB {
		// Properties
		int width{ 0 };
		int height{ 0 };
		int channels{ 0 };
		glm::u8vec3* data{ nullptr };

		// Constructors | copy / conversions
		ImageViewRGB(const ImageRGB& othger);

		// Operators | onversions
		ImageViewRGB& operator=(const ImageRGB& other);

		// Functions
		size_t pixelCount() const;
		size_t dataSize() const;
		glm::u8vec3 pixelAt(int x, int y) const;
		bool hasData() const;
	};

	struct ImageViewRGBA {
		// Properties
		int width{ 0 };
		int height{ 0 };
		int channels{ 0 };
		glm::u8vec4* data{ nullptr };

		// Constructors | copy / conversions
		ImageViewRGBA(const ImageRGBA& othger);

		// Operators | onversions
		ImageViewRGBA& operator=(const ImageRGBA& other);

		// Functions
		size_t pixelCount() const;
		size_t dataSize() const;
		glm::u8vec4 pixelAt(int x, int y) const;
		bool hasData() const;
	};
}
