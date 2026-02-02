#include "Image.h"

// Dependencies | std
#include <cassert>
#include <fstream>
#include <cstring>
#include <algorithm>

// Dependencies | stb
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

namespace it {
	// class ImageGray

	// class ImageGray::RowView

	// Object | public

	// Operators | member access
	unsigned char& ImageGray::RowView::operator[](size_t x) {
		assert(data != nullptr && "data == nullptr");
		assert(width >= 0 && "rectX < 0 (rectWidth is a negative number)");
		assert(x < width && "rectX is out of bounds");
		return data[x];
	}
	const unsigned char& ImageGray::RowView::operator[](size_t x) const {
		assert(data != nullptr && "data == nullptr");
		assert(width >= 0 && "rectX < 0 (rectWidth is a negative number)");
		assert(x < width && "rectX is out of bounds");
		return data[x];
	}

	// Object | public

	// Constructor / Destructor
	ImageGray::ImageGray(int width, int height) {
		assert(width > 0 && "rectWidth must be greater than 0");
		assert(height > 0 && "rectHeight must be greater than 0");

		size_t bufferSize = static_cast<size_t>(width * height);
		data = reinterpret_cast<unsigned char*>(std::malloc(bufferSize));
		if (data == nullptr)
			return;
		this->width = width;
		this->height = height;
	}
	ImageGray::ImageGray(const std::filesystem::path& path) {
		load(path);
	}
	ImageGray::ImageGray(const ImageGray& other) {
		if (this == &other || other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return;

		size_t otherDataSize = other.dataSize();
		data = reinterpret_cast<unsigned char*>(std::malloc(otherDataSize));
		if (data == nullptr)
			return;

		width = other.width;
		height = other.height;
		std::copy(other.data, other.data + otherDataSize, data);
	}
	ImageGray::ImageGray(const ImageGrayAlpha& other, bool factorInAlpha) {
		copy(other, factorInAlpha);
	}
	ImageGray::ImageGray(const ImageRGB& other) {
		copy(other);
	}
	ImageGray::ImageGray(const ImageRGBA& other, bool factorInAlpha) {
		copy(other, factorInAlpha);
	}
	ImageGray::ImageGray(ImageGray&& other) noexcept {
		if (this == &other || other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return;

		width = other.width;
		height = other.height;
		data = other.data;

		other.width = 0;
		other.height = 0;
		other.data = nullptr;
	}
	ImageGray::~ImageGray() {
		free();
	}

	// Operators | assignment
	ImageGray& ImageGray::operator=(const ImageGray& other) {
		copy(other);
		return *this;
	}
	ImageGray& ImageGray::operator=(ImageGray&& other) noexcept {
		if (this == &other || other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return *this;

		width = other.width;
		height = other.height;
		data = other.data;

		size_t pixelCount = other.pixelCount();
		glm::u8vec2* otherData = reinterpret_cast<glm::u8vec2*>(other.data);
		for (size_t i = 0ULL; i < pixelCount; i++) {
			data[i] = otherData[i][0];
		}

		other.width = 0;
		other.height = 0;
		other.data = nullptr;

		return *this;
	}
	ImageGray& ImageGray::operator=(const ImageGrayAlpha& other) {
		copy(other, true);
		return *this;
	}
	ImageGray& ImageGray::operator=(ImageGrayAlpha&& other) noexcept {
		copy(other, true);
		other.free();
		return *this;
	}
	ImageGray& ImageGray::operator=(const ImageRGB& other) {
		copy(other);
		return *this;
	}
	ImageGray& ImageGray::operator=(ImageRGB&& other) noexcept {
		copy(other);
		other.free();
		return *this;
	}
	ImageGray& ImageGray::operator=(const ImageRGBA& other) {
		copy(other, true);
		return *this;
	}
	ImageGray& ImageGray::operator=(ImageRGBA&& other) noexcept {
		copy(other, true);
		other.free();
		return *this;
	}

	// Operators | member access operator
	ImageGray::RowView ImageGray::operator[](size_t y) {
		assert(data != nullptr && "data == nullptr");
		assert(y < height && "rectY >= rectHeight");
		return RowView{ data + y * width, width };
	}
	const ImageGray::RowView ImageGray::operator[](size_t y) const {
		assert(data != nullptr && "data == nullptr");
		assert(y < height && "rectY >= rectHeight");
		return RowView{ data + y * width, width };
	}

	// Getters
	int ImageGray::getWidth() const {
		return width;
	}
	int ImageGray::getHeight() const {
		return height;
	}
	int ImageGray::getChannels() const {
		return CHANNELS;
	}
	unsigned char* ImageGray::getData() const {
		return data;
	}

	// Functions | allocation
	unsigned char* ImageGray::allocate(int width, int height) {
		// Free previous data if any
		free();

		if (width <= 0 || height <= 0)
			return nullptr;

		size_t bufferSize = static_cast<size_t>(width * height);
		data = reinterpret_cast<unsigned char*>(std::malloc(bufferSize));
		return data;
	}
	bool ImageGray::isAllocated() const {
		return data != nullptr;
	}
	size_t ImageGray::dataSize() const {
		return static_cast<size_t>(width) * static_cast<size_t>(height) * CHANNELS;
	}
	void ImageGray::free() {
		width = 0;
		height = 0;
		if (data != nullptr) {
			stbi_image_free(data);
			data = nullptr;
		}
	}

	// Functions | file loading (allocated memory) / saving
	bool ImageGray::load(const std::filesystem::path& path, bool flipImageOnLoad) {
		if (path.empty())
			return false; // No path set

		// Free previous data if any
		free();

		// Read file into memory
		std::ifstream ifstream{ path, std::ios::binary };
		if (!ifstream.is_open())
			return false; // Failed to open file

		ifstream.seekg(0, std::ios::end);
		size_t fileSize{ static_cast<size_t>(ifstream.tellg()) };
		ifstream.seekg(0, std::ios::beg);

		unsigned char* fileData{ static_cast<unsigned char*>(std::malloc(fileSize)) };
		if (fileData == nullptr)
			return false; // Memory allocation failed

		if (!ifstream.read(reinterpret_cast<char*>(fileData), fileSize)) {
			std::free(fileData);
			return false; // Failed to read file
		}
		ifstream.close();

		// Load image from memory
		bool success{ loadFromMemory(fileData, fileSize, flipImageOnLoad) };

		// Free temporary memory
		std::free(fileData);

		return success;
	}
	bool ImageGray::loadFromMemory(const unsigned char* fileInMemory, size_t size, bool flipImageOnLoad) {
		// Error check
		if (fileInMemory == nullptr || size == 0)
			return false;

		// Free previous data if any
		free();

		// Set vertical flip
		stbi_set_flip_vertically_on_load(flipImageOnLoad);

		// Load image with one channel
		int unusedChannelParameter{ 0 }; // Reason: CHANNELS returns 1, enforcing it to always be 1 channel
		data = stbi_load_from_memory(fileInMemory, static_cast<int>(size), &width, &height, &unusedChannelParameter, CHANNELS);

		return data != nullptr;
	}
	bool ImageGray::copy(const ImageGray& other) {
		// Error check
		if (this == &other || other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return false;

		// Release existing data
		free();

		// Allocate memory for copy operation
		size_t dataSize = other.dataSize();
		data = reinterpret_cast<unsigned char*>(std::malloc(dataSize));
		if (data == nullptr)
			return false;
		width = other.width;
		height = other.height;

		// Copy data
		std::memcpy(data, other.data, dataSize);

		// Success
		return true;
	}
	bool ImageGray::copy(const ImageGrayAlpha& other, bool factorInAlpha) {
		// Error check
		if (other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return false;

		// Release existing data
		free();

		// Allocate memory for copy operation
		size_t otherPixelCount = other.pixelCount();
		data = reinterpret_cast<unsigned char*>(std::malloc(otherPixelCount));
		if (data == nullptr)
			return false;
		width = other.width;
		height = other.height;

		// Copy data
		for (size_t i = 0; i < otherPixelCount; i++) {
			unsigned char pixel = other.data[i].r;
			if (factorInAlpha) {
				float alphaFactor = other.data[i][1] / 255.0f; // assuming second channel is alpha
				pixel = static_cast<unsigned char>(pixel * alphaFactor);
			}
			data[i] = pixel;
		}

		// Success
		return true;
	}
	bool ImageGray::copy(const ImageRGB& other) {
		// Error check
		if (other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return false;

		// Release existing data
		free();

		// Allocate memory for copy operation
		size_t otherPixelCount = other.getPixelCount();
		data = reinterpret_cast<unsigned char*>(std::malloc(otherPixelCount));
		if (data == nullptr)
			return false;
		width = other.width;
		height = other.height;

		// Copy data
		for (size_t i = 0ULL; i < otherPixelCount; i++) {
			unsigned char r = other.data[i].r;
			unsigned char g = other.data[i].g;
			unsigned char b = other.data[i].b;

			// Luminance formula
			unsigned char pixel = static_cast<unsigned char>(
			0.299f * r + 0.587f * g + 0.114f * b
				);

			data[i] = pixel;
		}

		// Success
		return true;
	}
	bool ImageGray::copy(const ImageRGBA& other, bool factorInAlpha) {
		// Error check
		if (other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return false;

		// Release existing data
		free();

		// Allocate memory for copy operation
		size_t otherPixelCount = other.pixelCount();
		data = reinterpret_cast<unsigned char*>(std::malloc(otherPixelCount));
		if (data == nullptr)
			return false;
		width = other.width;
		height = other.height;

		// Copy data
		for (size_t i = 0ULL; i < otherPixelCount; i++) {
			unsigned char r = other.data[i].r;
			unsigned char g = other.data[i].g;
			unsigned char b = other.data[i].b;
			unsigned char a = other.data[i].a;

			// Convert ImageRGB to grayscale
			float pixel = 0.299f * r + 0.587f * g + 0.114f * b;

			// Factor in alpha if requested
			if (factorInAlpha) {
				float alphaFactor = a / 255.0f;
				pixel *= alphaFactor;
			}

			data[i] = static_cast<unsigned char>(pixel);
		}

		// Success
		return true;
	}
	bool ImageGray::saveAsPNG(const std::filesystem::path& path) const {
		const int STRIDE = width * CHANNELS;
		return static_cast<bool>(stbi_write_png(path.string().c_str(), width, height, CHANNELS, data, STRIDE));
	}
	bool ImageGray::saveAsJPEG(const std::filesystem::path& path, int quality) const {
		return static_cast<bool>(stbi_write_jpg(path.string().c_str(), width, height, CHANNELS, data, quality));
	}
	bool ImageGray::saveAsBMP(const std::filesystem::path& path) const {
		return static_cast<bool>(stbi_write_bmp(path.string().c_str(), width, height, CHANNELS, data));
	}
	bool ImageGray::saveAsTGA(const std::filesystem::path& path) const {
		return static_cast<bool>(stbi_write_tga(path.string().c_str(), width, height, CHANNELS, data));
	}
	bool ImageGray::save(const std::filesystem::path& path, int quality) const {
		auto ext = path.extension().string();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

		if (ext == ".png") {
			return saveAsPNG(path);
		}
		else if (ext == ".jpg" || ext == ".jpeg") {
			return saveAsJPEG(path, quality);
		}
		else if (ext == ".bmp") {
			return saveAsBMP(path);
		}
		else if (ext == ".tga") {
			return saveAsTGA(path);
		}

		// Unsupported extension
		return false;
	}

	// Functions | pixel manipulation
	size_t ImageGray::pixelCount() const {
		return static_cast<size_t>(width) * static_cast<size_t>(height);
	}
	unsigned char ImageGray::pixelAt(int x, int y) const {
		// Error check
		assert(data != nullptr);
		assert(x >= 0 && "rectX < 0");
		assert(y >= 0 && "rectY < 0");
		assert(x < width && " rectX >= rectWidth");
		assert(y < height && "rectY >= rectHeight");
		if (!data || x < 0 || x >= width || y < 0 || y >= height)
			return 0U;

		// Get pixel
		size_t index = (static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x));
		return data[index];
	}

	// Functions | painting
	bool ImageGray::setPixel(int x, int y, unsigned char pixel) {
		assert(data != nullptr);
		assert(x >= 0 && "rectX < 0");
		assert(y >= 0 && "rectY < 0");
		assert(x < width && " rectX >= rectWidth");
		assert(y < height && "rectY >= rectHeight");
		if (data == nullptr || x < 0 || y < 0 || x >= width || y >= height)
			return false;

		// Set pixel
		size_t index = (static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x));
		data[index] = pixel;

		// Success
		return true;
	}
	bool ImageGray::setPixel(int x, int y, float pixel) {
		assert(data != nullptr);
		assert(x >= 0 && "rectX < 0");
		assert(y >= 0 && "rectY < 0");
		assert(x < width && " rectX >= rectWidth");
		assert(y < height && "rectY >= rectHeight");
		if (data == nullptr || x < 0 || y < 0 || x >= width || y >= height)
			return false;

		// Set pixel
		size_t index = static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x);
		data[index] = static_cast<unsigned char>(std::clamp(pixel, 0.0f, 1.0f) * 255.0f);

		// Success
		return true;
	}
	void ImageGray::fillRect(int x, int y, int width, int height, unsigned char color) {
		// Error check
		assert(data != nullptr);
		assert(x >= 0 && "rectX < 0");
		assert(y >= 0 && "rectY < 0");
		assert(x < this->width && " rectX >= this->rectWidth");
		assert(y < this->height && "rectY >= this->rectHeight");
		assert(x + width <= this->width && "specified rectWidth is out of image bounds");
		assert(y + height <= this->height && "specified rectHeight is out of image bounds");
		
		for (int currentX = x; currentX <= x; x++) {
			for (int currentY = y; y <= y; y++) {
				size_t index = static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x);
				data[index] = color;
			}
		}
	}

	// class ImageGrayAlpha

	// class ImageGrayAlpha::RowView

	// Object | public

	// Operators | member access
	glm::u8vec2& ImageGrayAlpha::RowView::operator[](size_t x) {
		assert(data != nullptr && "data == nullptr");
		assert(width >= 0 && "rectX < 0 (rectWidth is a negative number)");
		assert(x < width && "rectX is out of bounds");
		return data[x];
	}
	const glm::u8vec2& ImageGrayAlpha::RowView::operator[](size_t x) const {
		assert(data != nullptr && "data == nullptr");
		assert(width >= 0 && "rectX < 0 (rectWidth is a negative number)");
		assert(x < width && "rectX is out of bounds");
		return data[x];
	}

	// Object | public

	// Constructor / Destructor
	ImageGrayAlpha::ImageGrayAlpha(int width, int height) {
		assert(width > 0 && "rectWidth must be greater than 0");
		assert(height > 0 && "rectHeight must be greater than 0");

		size_t bufferSize = static_cast<size_t>(width * height * 2);
		data = reinterpret_cast<glm::u8vec2*>(std::malloc(bufferSize));
		if (data == nullptr)
			return;
		this->width = width;
		this->height = height;
	}
	ImageGrayAlpha::ImageGrayAlpha(const std::filesystem::path& path) {
		load(path);
	}
	ImageGrayAlpha::ImageGrayAlpha(const ImageGray& other) {
		copy(other);
	}
	ImageGrayAlpha::ImageGrayAlpha(const ImageGrayAlpha& other) {
		if (this == &other || other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return;

		size_t otherDataSize = other.dataSize();
		data = reinterpret_cast<glm::u8vec2*>(std::malloc(otherDataSize));
		if (data == nullptr)
			return;

		width = other.width;
		height = other.height;
		std::copy(other.data, other.data + otherDataSize, data);
	}
	ImageGrayAlpha::ImageGrayAlpha(const ImageRGB& other) {
		copy(other);
	}
	ImageGrayAlpha::ImageGrayAlpha(const ImageRGBA& other) {
		copy(other);
	}
	ImageGrayAlpha::ImageGrayAlpha(ImageGrayAlpha&& other) noexcept {
		if (this == &other || other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return;

		width = other.width;
		height = other.height;
		data = other.data;

		other.width = 0;
		other.height = 0;
		other.data = nullptr;
	}
	ImageGrayAlpha::~ImageGrayAlpha() {
		free();
	}

	// Operators | assignment
	ImageGrayAlpha& ImageGrayAlpha::operator=(const ImageGray& other) {
		copy(other);
		return *this;
	}
	ImageGrayAlpha& ImageGrayAlpha::operator=(ImageGray&& other) noexcept {
		copy(other);
		other.free();
		return *this;
	}
	ImageGrayAlpha& ImageGrayAlpha::operator=(const ImageGrayAlpha& other) {
		copy(other);
		return *this;
	}
	ImageGrayAlpha& ImageGrayAlpha::operator=(ImageGrayAlpha&& other) noexcept {
		if (this == &other || other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return *this;

		width = other.width;
		height = other.height;
		data = other.data;

		other.width = 0;
		other.height = 0;
		other.data = nullptr;

		return *this;
	}
	ImageGrayAlpha& ImageGrayAlpha::operator=(const ImageRGB& other) {
		copy(other);
		return *this;
	}
	ImageGrayAlpha& ImageGrayAlpha::operator=(ImageRGB&& other) noexcept {
		copy(other);
		other.free();
		return *this;
	}
	ImageGrayAlpha& ImageGrayAlpha::operator=(const ImageRGBA& other) {
		copy(other);
		return *this;
	}
	ImageGrayAlpha& ImageGrayAlpha::operator=(ImageRGBA&& other) noexcept {
		copy(other);
		other.free();
		return *this;
	}

	// Operators | member access operator
	ImageGrayAlpha::RowView ImageGrayAlpha::operator[](size_t y) {
		assert(data != nullptr && "data == nullptr");
		assert(y < height && "rectY >= rectHeight");
		return RowView{ data + y * width, width };
	}
	const ImageGrayAlpha::RowView ImageGrayAlpha::operator[](size_t y) const {
		assert(data != nullptr && "data == nullptr");
		assert(y < height && "rectY >= rectHeight");
		return RowView{ data + y * width, width };
	}

	// Getters
	int ImageGrayAlpha::getWidth() const {
		return width;
	}
	int ImageGrayAlpha::getHeight() const {
		return height;
	}
	int ImageGrayAlpha::getChannels() const {
		return CHANNELS;
	}
	glm::u8vec2* ImageGrayAlpha::getData() const {
		return data;
	}

	// Functions | allocation
	glm::u8vec2* ImageGrayAlpha::allocate(int width, int height) {
		// Free previous data if any
		free();

		if (width <= 0 || height <= 0)
			return nullptr;

		size_t bufferSize = static_cast<size_t>(width * height * 2);
		data = reinterpret_cast<glm::u8vec2*>(std::malloc(bufferSize));
		return data;
	}
	bool ImageGrayAlpha::isAllocated() const {
		return data != nullptr;
	}
	size_t ImageGrayAlpha::dataSize() const {
		return static_cast<size_t>(width) * static_cast<size_t>(height) * CHANNELS;
	}
	void ImageGrayAlpha::free() {
		width = 0;
		height = 0;
		if (data != nullptr) {
			stbi_image_free(data);
			data = nullptr;
		}
	}

	// Functions | file loading (allocates memory) / saving
	bool ImageGrayAlpha::load(const std::filesystem::path& path, bool flipImageOnLoad) {
		if (path.empty())
			return false; // No path set

		// Free previous data if any
		free();

		// Read file into memory
		std::ifstream ifstream{ path, std::ios::binary };
		if (!ifstream.is_open())
			return false; // Failed to open file

		ifstream.seekg(0, std::ios::end);
		size_t fileSize{ static_cast<size_t>(ifstream.tellg()) };
		ifstream.seekg(0, std::ios::beg);

		unsigned char* fileData{ static_cast<unsigned char*>(std::malloc(fileSize)) };
		if (fileData == nullptr)
			return false; // Memory allocation failed

		if (!ifstream.read(reinterpret_cast<char*>(fileData), fileSize)) {
			std::free(fileData);
			return false; // Failed to read file
		}
		ifstream.close();

		// Load image from memory
		bool success{ loadFromMemory(fileData, fileSize, flipImageOnLoad) };

		// Free temporary memory
		std::free(fileData);

		return success;
	}
	bool ImageGrayAlpha::loadFromMemory(const unsigned char* fileInMemory, size_t size, bool flipImageOnLoad) {
		// Error check
		if (fileInMemory == nullptr || size == 0)
			return false;

		// Free previous data if any
		free();

		// Set vertical flip
		stbi_set_flip_vertically_on_load(flipImageOnLoad);

		// Load image with one channel
		int unusedChannelParameter{ 0 }; // Reason: CHANNELS returns 2, enforcing it to always be 2 channel
		data = reinterpret_cast<glm::u8vec2*>(stbi_load_from_memory(fileInMemory, static_cast<int>(size), &width, &height, &unusedChannelParameter, CHANNELS));

		return data != nullptr;
	}
	bool ImageGrayAlpha::copy(const ImageGray& other) {
		// Error check
		if (other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return false;

		// Release existing data
		free();

		// Allocate memory for copy operation
		size_t pixelCount = other.pixelCount();
		data = reinterpret_cast<glm::u8vec2*>(std::malloc(pixelCount * sizeof(glm::u8vec2)));
		if (data == nullptr)
			return false;
		width = other.width;
		height = other.height;

		// Copy data
		for (size_t i = 0ULL; i < pixelCount; i++)
			data[i] = glm::u8vec2(other.data[i], 255U);

		// Success
		return true;
	}
	bool ImageGrayAlpha::copy(const ImageGrayAlpha& other) {
		// Error check
		if (this == &other || other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return false;

		// Release existing data
		free();

		// Allocate memory for copy operation
		size_t dataSize = other.dataSize();
		data = reinterpret_cast<glm::u8vec2*>(std::malloc(dataSize));
		if (data == nullptr)
			return false;
		width = other.width;
		height = other.height;

		// Copy data
		std::memcpy(data, other.data, dataSize);

		// Success
		return true;
	}
	bool ImageGrayAlpha::copy(const ImageRGB& other) {
		// Error check
		if (other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return false;

		// Release existing data
		free();

		// Allocate memory for copy operation
		size_t pixelCount = other.getPixelCount();
		data = reinterpret_cast<glm::u8vec2*>(std::malloc(pixelCount * sizeof(glm::u8vec2)));
		if (data == nullptr)
			return false;
		width = other.width;
		height = other.height;

		// Copy data
		for (size_t i = 0ULL; i < pixelCount; i++) {
			unsigned char r = other.data[i].r;
			unsigned char g = other.data[i].g;
			unsigned char b = other.data[i].b;

			// Luminance formula
			unsigned char grayValue = static_cast<unsigned char>(
				0.299f * r + 0.587f * g + 0.114f * b
				);

			data[i] = glm::u8vec2(grayValue, 255U);
		}

		// Success
		return true;
	}
	bool ImageGrayAlpha::copy(const ImageRGBA& other) {
		// Error check
		if (other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return false;

		// Release existing data
		free();

		// Allocate memory for copy operation
		size_t pixelCount = other.pixelCount();
		data = reinterpret_cast<glm::u8vec2*>(std::malloc(pixelCount * sizeof(glm::u8vec2)));
		if (data == nullptr)
			return false;
		width = other.width;
		height = other.height;

		// Copy data
		for (size_t i = 0ULL; i < pixelCount; i++) {
			unsigned char r = other.data[i].r;
			unsigned char g = other.data[i].g;
			unsigned char b = other.data[i].b;

			// Luminance formula
			unsigned char grayValue = static_cast<unsigned char>(
				0.299f * r + 0.587f * g + 0.114f * b
				);

			data[i] = glm::u8vec2(grayValue, other.data[i].a);
		}

		// Success
		return true;
	}
	bool ImageGrayAlpha::saveAsPNG(const std::filesystem::path& path) const {
		const int STRIDE = width * CHANNELS;
		return static_cast<bool>(stbi_write_png(path.string().c_str(), width, height, CHANNELS, data, STRIDE));
	}
	bool ImageGrayAlpha::saveAsJPEG(const std::filesystem::path& path, int quality) const {
		return static_cast<bool>(stbi_write_jpg(path.string().c_str(), width, height, CHANNELS, data, quality));
	}
	bool ImageGrayAlpha::saveAsBMP(const std::filesystem::path& path) const {
		return static_cast<bool>(stbi_write_bmp(path.string().c_str(), width, height, CHANNELS, data));
	}
	bool ImageGrayAlpha::saveAsTGA(const std::filesystem::path& path) const {
		return static_cast<bool>(stbi_write_tga(path.string().c_str(), width, height, CHANNELS, data));
	}
	bool ImageGrayAlpha::save(const std::filesystem::path& path, int quality) const {
		auto ext = path.extension().string();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

		if (ext == ".png") {
			return saveAsPNG(path);
		}
		else if (ext == ".jpg" || ext == ".jpeg") {
			return saveAsJPEG(path, quality);
		}
		else if (ext == ".bmp") {
			return saveAsBMP(path);
		}
		else if (ext == ".tga") {
			return saveAsTGA(path);
		}

		// Unsupported extension
		return false;
	}

	// Functions | pixel manipulations
	size_t ImageGrayAlpha::pixelCount() const {
		return static_cast<size_t>(width) * static_cast<size_t>(height);
	}
	glm::u8vec2 ImageGrayAlpha::pixeAt(int x, int y) const {
		// Error check
		assert(data != nullptr);
		assert(x >= 0 && "rectX < 0");
		assert(y >= 0 && "rectY < 0");
		assert(x < width && " rectX >= rectWidth");
		assert(y < height && "rectY >= rectHeight");
		if (!data || x < 0 || x >= width || y < 0 || y >= height)
			return glm::u8vec2(0u);

		// Get pixel
		size_t index = (static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x));
		return reinterpret_cast<glm::u8vec2*>(data)[index];
	}

	// Setters
	bool ImageGrayAlpha::paintPixel(int x, int y, glm::u8vec2 pixel) {
		// Error check
		assert(data != nullptr);
		assert(x >= 0 && "rectX < 0");
		assert(y >= 0 && "rectY < 0");
		assert(x < width && " rectX >= rectWidth");
		assert(y < height && "rectY >= rectHeight");
		if (data == nullptr || x < 0 || y < 0 || x >= width || y >= height)
			return false;

		// Set pixel
		size_t index = (static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x));
		data[index] = pixel;

		// Success
		return true;
	}
	bool ImageGrayAlpha::paintPixel(int x, int y, glm::vec2 pixel) {
		// Error check
		assert(data != nullptr);
		assert(x >= 0 && "rectX < 0");
		assert(y >= 0 && "rectY < 0");
		assert(x < width && " rectX >= rectWidth");
		assert(y < height && "rectY >= rectHeight");
		if (data == nullptr || x < 0 || y < 0 || x >= width || y >= height)
			return false;

		// Set pixel
		size_t index = static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x);
		data[index] = glm::u8vec2(
			static_cast<unsigned char>(std::clamp(pixel[0], 0.0f, 1.0f) * 255.0f),
			static_cast<unsigned char>(std::clamp(pixel[1], 0.0f, 1.0f) * 255.0f)
		);

		// Success
		return true;
	}
	void ImageGrayAlpha::fillRect(int rectX, int rectY, int rectWidth, int rectHeight, const glm::u8vec2& color) {
		// Error check
		assert(data != nullptr);
		assert(rectX >= 0 && "rectX < 0");
		assert(rectY >= 0 && "rectY < 0");
		assert(rectX < width && "rectX >= width");
		assert(rectY < height && "rectY >= height");
		assert(rectX + rectWidth <= width && "rectX + rectWidth is out of image bounds");
		assert(rectY + rectHeight <= height && "rectY + rectHeight is out of image bounds");

		for (int currentY = rectY, yEnd = rectY + rectHeight; currentY < yEnd; ++currentY) {
			for (int currentX = rectX, xEnd = rectX + rectWidth; currentX < xEnd; ++currentX) {
				size_t index = static_cast<size_t>(currentY) * static_cast<size_t>(width) + static_cast<size_t>(currentX);
				data[index] = color;
			}
		}
	}

	// class ImageRGB

	// class ImageRGB::RowView

	// Object | public

	// Operators | member access
	glm::u8vec3& ImageRGB::RowView::operator[](size_t x) {
		assert(data != nullptr && "data == nullptr");
		assert(width >= 0 && "rectX < 0 (rectWidth is a negative number)");
		assert(x < width && "rectX is out of bounds");
		return data[x];
	}
	const glm::u8vec3& ImageRGB::RowView::operator[](size_t x) const {
		assert(data != nullptr && "data == nullptr");
		assert(width >= 0 && "rectX < 0 (rectWidth is a negative number)");
		assert(x < width && "rectX is out of bounds");
		return data[x];
	}

	// Object | public

	// Constructor / Destructor
	ImageRGB::ImageRGB(int width, int height) {
		assert(width > 0 && "rectWidth must be greater than 0");
		assert(height > 0 && "rectHeight must be greater than 0");

		size_t bufferSize = static_cast<size_t>(width * height * 3);
		data = reinterpret_cast<glm::u8vec3*>(std::malloc(bufferSize));
		if (data == nullptr)
			return;
		this->width = width;
		this->height = height;
	}
	ImageRGB::ImageRGB(const std::filesystem::path& path) {
		load(path);
	}
	ImageRGB::ImageRGB(const ImageGray& other) {
		copy(other);
	}
	ImageRGB::ImageRGB(const ImageGrayAlpha& other, bool factorInAlpha) {
		copy(other, factorInAlpha);
	}
	ImageRGB::ImageRGB(const ImageRGB& other) {
		if (this == &other || other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return;

		size_t otherDataSize = other.dataSize();
		data = reinterpret_cast<glm::u8vec3*>(std::malloc(otherDataSize));
		if (data == nullptr)
			return;

		width = other.width;
		height = other.height;
		std::copy(other.data, other.data + otherDataSize, data);
	}
	ImageRGB::ImageRGB(const ImageRGBA& other, bool factorInAlpha) {
		copy(other, factorInAlpha);
	}
	ImageRGB::ImageRGB(ImageRGB&& other) noexcept {
		if (this == &other || other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return;

		width = other.width;
		height = other.height;
		data = other.data;

		other.width = 0;
		other.height = 0;
		other.data = nullptr;
	}
	ImageRGB::~ImageRGB() {
		free();
	}

	// Operators | assignment
	ImageRGB& ImageRGB::operator=(const ImageGray& other) {
		copy(other);
		return *this;
	}
	ImageRGB& ImageRGB::operator=(ImageGray&& other) noexcept {
		copy(other);
		other.free();
		return *this;
	}
	ImageRGB& ImageRGB::operator=(const ImageGrayAlpha& other) {
		copy(other);
		return *this;
	}
	ImageRGB& ImageRGB::operator=(ImageGrayAlpha&& other) noexcept {
		copy(other);
		other.free();
		return *this;
	}
	ImageRGB& ImageRGB::operator=(const ImageRGB& other) {
		copy(other);
		return *this;
	}
	ImageRGB& ImageRGB::operator=(ImageRGB&& other) noexcept {
		if (this == &other || other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return *this;

		width = other.width;
		height = other.height;
		data = other.data;

		other.width = 0;
		other.height = 0;
		other.data = nullptr;

		return *this;
	}
	ImageRGB& ImageRGB::operator=(const ImageRGBA& other) {
		copy(other);
		return *this;
	}
	ImageRGB& ImageRGB::operator=(ImageRGBA&& other) noexcept {
		copy(other);
		other.free();
		return *this;
	}

	// Operators | member access operator
	ImageRGB::RowView ImageRGB::operator[](size_t y) {
		assert(data != nullptr && "data == nullptr");
		assert(y < height && "rectY >= rectHeight");
		return RowView{ data + y * width, width };
	}
	const ImageRGB::RowView ImageRGB::operator[](size_t y) const {
		assert(data != nullptr && "data == nullptr");
		assert(y < height && "rectY >= rectHeight");
		return RowView{ data + y * width, width };
	}

	// Getters
	int ImageRGB::getWidth() const {
		return width;
	}
	int ImageRGB::getHeight() const {
		return height;
	}
	int ImageRGB::getChannels() const {
		return CHANNELS;
	}
	glm::u8vec3* ImageRGB::getData() const {
		return data;
	}

	// Functions | allocation / deallocation
	glm::u8vec3* ImageRGB::allocate(int width, int height) {
		// Free previous data if any
		free();

		if (width <= 0 || height <= 0)
			return nullptr;

		size_t bufferSize = static_cast<size_t>(width * height * 3);
		data = reinterpret_cast<glm::u8vec3*>(std::malloc(bufferSize));
		return data;
	}
	bool ImageRGB::isAllocated() const {
		return data != nullptr;
	}
	size_t ImageRGB::dataSize() const {
		return static_cast<size_t>(width) * static_cast<size_t>(height) * CHANNELS;
	}
	void ImageRGB::free() {
		width = 0;
		height = 0;
		if (data != nullptr) {
			stbi_image_free(data);
			data = nullptr;
		}
	}

	// Functions | file loading (allocates memory) / saving
	bool ImageRGB::load(const std::filesystem::path& path, bool flipImageOnLoad) {
		if (path.empty())
			return false; // No path set

		// Free previous data if any
		free();

		// Read file into memory
		std::ifstream ifstream{ path, std::ios::binary };
		if (!ifstream.is_open())
			return false; // Failed to open file

		ifstream.seekg(0, std::ios::end);
		size_t fileSize{ static_cast<size_t>(ifstream.tellg()) };
		ifstream.seekg(0, std::ios::beg);

		unsigned char* fileData{ static_cast<unsigned char*>(std::malloc(fileSize)) };
		if (fileData == nullptr)
			return false; // Memory allocation failed

		if (!ifstream.read(reinterpret_cast<char*>(fileData), fileSize)) {
			std::free(fileData);
			return false; // Failed to read file
		}
		ifstream.close();

		// Load image from memory
		bool success{ loadFromMemory(fileData, fileSize, flipImageOnLoad) };

		// Free temporary memory
		std::free(fileData);

		return success;
	}
	bool ImageRGB::loadFromMemory(const unsigned char* fileInMemory, size_t size, bool flipImageOnLoad) {
		// Error check
		if (fileInMemory == nullptr || size == 0)
			return false;

		// Free previous data if any
		free();

		// Set vertical flip
		stbi_set_flip_vertically_on_load(flipImageOnLoad);

		// Load image with one channel
		int unusedChannelParameter{ 0 }; // Reason: CHANNELS returns 3, enforcing it to always be 3 channel
		data = reinterpret_cast<glm::u8vec3*>(stbi_load_from_memory(fileInMemory, static_cast<int>(size), &width, &height, &unusedChannelParameter, CHANNELS));

		return data != nullptr;
	}
	bool ImageRGB::copy(const ImageGray& other) {
		// Error check
		if (other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return false;

		// Release existing data
		free();

		size_t pixelCount = other.pixelCount();
		data = reinterpret_cast<glm::u8vec3*>(std::malloc(pixelCount * sizeof(glm::u8vec3)));
		if (data == nullptr)
			return false;
		width = other.width;
		height = other.height;

		// Copy data
		for (size_t i = 0ULL; i < pixelCount; i++)
			data[i] = glm::u8vec3(other.data[i], other.data[i], other.data[i]);

		// Success
		return true;
	}
	bool ImageRGB::copy(const ImageGrayAlpha& other, bool factorInAlpha) {
		// Error check
		if (other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return false;

		// Release existing data
		free();

		size_t pixelCount = other.pixelCount();
		data = reinterpret_cast<glm::u8vec3*>(std::malloc(pixelCount * sizeof(glm::u8vec3)));
		if (data == nullptr)
			return false;
		width = other.width;
		height = other.height;

		// Copy data
		for (size_t i = 0ULL; i < pixelCount; i++) {
			unsigned char gray = other.data[i].x;   // grayscale value
			unsigned char alpha = other.data[i].y;  // alpha value

			if (factorInAlpha) {
				// Scale grayscale by alpha (normalized to 0�255)
				unsigned char scaled = static_cast<unsigned char>((gray * alpha) / 255);
				data[i] = glm::u8vec3(scaled, scaled, scaled);
			}
			else {
				// Just copy grayscale into ImageRGB
				data[i] = glm::u8vec3(gray, gray, gray);
			}
		}

		// Success
		return true;
	}
	bool ImageRGB::copy(const ImageRGB& other) {
		// Error check
		if (this == &other || other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return false;

		// Release existing data
		free();

		// Allocate memory for copy operation
		size_t dataSize = other.dataSize();
		data = reinterpret_cast<glm::u8vec3*>(std::malloc(dataSize));
		if (data == nullptr)
			return false;
		width = other.width;
		height = other.height;

		// Copy data
		std::memcpy(data, other.data, dataSize);

		// Success
		return true;
	}
	bool ImageRGB::copy(const ImageRGBA& other, bool factorInAlpha) {
		// Error check
		if (other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return false;

		// Release existing data
		free();

		size_t pixelCount = other.pixelCount();
		data = reinterpret_cast<glm::u8vec3*>(std::malloc(pixelCount * sizeof(glm::u8vec3)));
		if (data == nullptr)
			return false;
		width = other.width;
		height = other.height;

		// Copy data
		for (size_t i = 0ULL; i < pixelCount; i++) {
			unsigned char r = other.data[i].r;
			unsigned char g = other.data[i].g;
			unsigned char b = other.data[i].b;
			unsigned char a = other.data[i].a;

			if (factorInAlpha) {
				// Premultiply by alpha
				r = static_cast<unsigned char>((r * a) / 255);
				g = static_cast<unsigned char>((g * a) / 255);
				b = static_cast<unsigned char>((b * a) / 255);
			}

			data[i] = glm::u8vec3(r, g, b);
		}
		// Success
		return true;
	}
	bool ImageRGB::saveAsPNG(const std::filesystem::path& path) const {
		const int STRIDE = width * CHANNELS;
		return static_cast<bool>(stbi_write_png(path.string().c_str(), width, height, CHANNELS, data, STRIDE));
	}
	bool ImageRGB::saveAsJPEG(const std::filesystem::path& path, int quality) const {
		return static_cast<bool>(stbi_write_jpg(path.string().c_str(), width, height, CHANNELS, data, quality));
	}
	bool ImageRGB::saveAsBMP(const std::filesystem::path& path) const {
		return static_cast<bool>(stbi_write_bmp(path.string().c_str(), width, height, CHANNELS, data));
	}
	bool ImageRGB::saveAsTGA(const std::filesystem::path& path) const {
		return static_cast<bool>(stbi_write_tga(path.string().c_str(), width, height, CHANNELS, data));
	}
	bool ImageRGB::save(const std::filesystem::path& path, int quality) const {
		auto ext = path.extension().string();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

		if (ext == ".png") {
			return saveAsPNG(path);
		}
		else if (ext == ".jpg" || ext == ".jpeg") {
			return saveAsJPEG(path, quality);
		}
		else if (ext == ".bmp") {
			return saveAsBMP(path);
		}
		else if (ext == ".tga") {
			return saveAsTGA(path);
		}

		// Unsupported extension
		return false;
	}

	// Functions | pixel manipulation
	size_t ImageRGB::getPixelCount() const {
		return static_cast<size_t>(width) * static_cast<size_t>(height);
	}
	glm::u8vec3 ImageRGB::getPixel(int x, int y) const {
		// Error check
		assert(data != nullptr);
		assert(x >= 0 && "rectX < 0");
		assert(y >= 0 && "rectY < 0");
		assert(x < width && " rectX >= rectWidth");
		assert(y < height && "rectY >= rectHeight");
		if (!data || x < 0 || x >= width || y < 0 || y >= height)
			return glm::u8vec3(0u);

		// Get pixel
		size_t index = (static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x));
		return reinterpret_cast<glm::u8vec3*>(data)[index];
	}
	bool ImageRGB::setPixel(int x, int y, glm::u8vec3 pixel) {
		// Error check
		assert(data != nullptr);
		assert(x >= 0 && "rectX < 0");
		assert(y >= 0 && "rectY < 0");
		assert(x < width && " rectX >= rectWidth");
		assert(y < height && "rectY >= rectHeight");
		if (data == nullptr || x < 0 || y < 0 || x >= width || y >= height)
			return false;

		// Set pixel
		size_t index = (static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x));
		data[index] = pixel;

		// Success
		return true;
	}
	bool ImageRGB::setPixel(int x, int y, glm::vec3 pixel) {
		// Error check
		assert(data != nullptr);
		assert(x >= 0 && "rectX < 0");
		assert(y >= 0 && "rectY < 0");
		assert(x < width && " rectX >= rectWidth");
		assert(y < height && "rectY >= rectHeight");
		if (data == nullptr || x < 0 || y < 0 || x >= width || y >= height)
			return false;

		// Set pixel
		size_t index = static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x);
		data[index] = glm::u8vec3(
			static_cast<unsigned char>(std::clamp(pixel[0], 0.0f, 1.0f) * 255.0f),
			static_cast<unsigned char>(std::clamp(pixel[1], 0.0f, 1.0f) * 255.0f),
			static_cast<unsigned char>(std::clamp(pixel[2], 0.0f, 1.0f) * 255.0f)
		);

		// Success
		return true;
	}
	void ImageRGB::fillRect(int rectX, int rectY, int rectWidth, int rectHeight, const glm::u8vec3& color) {
		// Error check
		assert(data != nullptr);
		assert(rectX >= 0 && "rectX < 0");
		assert(rectY >= 0 && "rectY < 0");
		assert(rectX < width && "rectX >= width");
		assert(rectY < height && "rectY >= height");
		assert(rectX + rectWidth <= width && "rectX + rectWidth is out of image bounds");
		assert(rectY + rectHeight <= height && "rectY + rectHeight is out of image bounds");

		for (int currentY = rectY, yEnd = rectY + rectHeight; currentY < yEnd; ++currentY) {
			for (int currentX = rectX, xEnd = rectX + rectWidth; currentX < xEnd; ++currentX) {
				size_t index = static_cast<size_t>(currentY) * static_cast<size_t>(width) + static_cast<size_t>(currentX);
				data[index] = color;
			}
		}
	}

	// class ImageRGBA

	// class ImageRGBA::RowView

	// Object | public

	// Operators | member access
	glm::u8vec4& ImageRGBA::RowView::operator[](size_t x) {
		assert(data != nullptr && "data == nullptr");
		assert(width >= 0 && "rectX < 0 (rectWidth is a negative number)");
		assert(x < width && "rectX is out of bounds");
		return data[x];
	}
	const glm::u8vec4& ImageRGBA::RowView::operator[](size_t x) const {
		assert(data != nullptr && "data == nullptr");
		assert(width >= 0 && "rectX < 0 (rectWidth is a negative number)");
		assert(x < width && "rectX is out of bounds");
		return data[x];
	}

	// Object | public

	// Constructor / Destructor
	ImageRGBA::ImageRGBA(int width, int height) {
		assert(width > 0 && "rectWidth must be greater than 0");
		assert(height > 0 && "rectHeight must be greater than 0");

		size_t bufferSize = static_cast<size_t>(width * height * 4);
		data = reinterpret_cast<glm::u8vec4*>(std::malloc(bufferSize));
		if (data == nullptr)
			return;
		this->width = width;
		this->height = height;
	}
	ImageRGBA::ImageRGBA(const std::filesystem::path& path) {
		load(path);
	}
	ImageRGBA::ImageRGBA(const ImageGray& other) {
		copy(other);
	}
	ImageRGBA::ImageRGBA(const ImageGrayAlpha& other) {
		copy(other);
	}
	ImageRGBA::ImageRGBA(const ImageRGB& other) {
		copy(other);
	}
	ImageRGBA::ImageRGBA(const ImageRGBA& other) {
		if (this == &other || other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return;

		size_t otherDataSize = other.dataSize();
		data = reinterpret_cast<glm::u8vec4*>(std::malloc(otherDataSize));
		if (data == nullptr)
			return;

		width = other.width;
		height = other.height;
		std::copy(other.data, other.data + otherDataSize, data);
	}
	ImageRGBA::ImageRGBA(ImageRGBA&& other) noexcept {
		if (this == &other || other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return;

		width = other.width;
		height = other.height;
		data = other.data;

		other.width = 0;
		other.height = 0;
		other.data = nullptr;
	}
	ImageRGBA::~ImageRGBA() {
		free();
	}

	// Operators | assignment
	ImageRGBA& ImageRGBA::operator=(const ImageGray& other) {
		copy(other);
		return *this;
	}
	ImageRGBA& ImageRGBA::operator=(ImageGray&& other) noexcept {
		copy(other);
		other.free();
		return *this;
	}
	ImageRGBA& ImageRGBA::operator=(const ImageGrayAlpha& other) {
		copy(other);
		return *this;
	}
	ImageRGBA& ImageRGBA::operator=(ImageGrayAlpha&& other) noexcept {
		copy(other);
		other.free();
		return *this;
	}
	ImageRGBA& ImageRGBA::operator=(const ImageRGB& other) {
		copy(other);
		return *this;
	}
	ImageRGBA& ImageRGBA::operator=(ImageRGB&& other) noexcept {
		copy(other);
		other.free();
		return *this;
	}
	ImageRGBA& ImageRGBA::operator=(const ImageRGBA& other) {
		copy(other);
		return *this;
	}
	ImageRGBA& ImageRGBA::operator=(ImageRGBA&& other) noexcept {
		if (this == &other || other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return *this;

		width = other.width;
		height = other.height;
		data = other.data;

		other.width = 0;
		other.height = 0;
		other.data = nullptr;

		return *this;
	}

	// Operators | member access operator
	ImageRGBA::RowView ImageRGBA::operator[](size_t y) {
		assert(data != nullptr && "data == nullptr");
		assert(y < height && "rectY >= rectHeight");
		return RowView{ data + y * width, width };
	}
	const ImageRGBA::RowView ImageRGBA::operator[](size_t y) const {
		assert(data != nullptr && "data == nullptr");
		assert(y < height && "rectY >= rectHeight");
		return RowView{ data + y * width, width };
	}

	// Getters
	int ImageRGBA::getWidth() const {
		return width;
	}
	int ImageRGBA::getHeight() const {
		return height;
	}
	int ImageRGBA::getChannels() const {
		return CHANNELS;
	}
	glm::u8vec4* ImageRGBA::getData() const {
		return data;
	}

	// Functions | allocation / deallocation
	glm::u8vec4* ImageRGBA::allocate(int width, int height) {
		// Free previous data if any
		free();

		if (width <= 0 || height <= 0)
			return nullptr;

		size_t bufferSize = static_cast<size_t>(width * height * 4);
		data = reinterpret_cast<glm::u8vec4*>(std::malloc(bufferSize));
		return data;
	}
	bool ImageRGBA::isAllocated() const {
		return data != nullptr;
	}
	size_t ImageRGBA::dataSize() const {
		return static_cast<size_t>(width) * static_cast<size_t>(height) * CHANNELS;
	}
	void ImageRGBA::free() {
		width = 0;
		height = 0;
		if (data != nullptr) {
			stbi_image_free(data);
			data = nullptr;
		}
	}

	// Functions | file loading (allocates memory) / saving
	bool ImageRGBA::load(const std::filesystem::path& path, bool flipImageOnLoad) {
		if (path.empty())
			return false; // No path set

		// Free previous data if any
		free();

		// Read file into memory
		std::ifstream ifstream{ path, std::ios::binary };
		if (!ifstream.is_open())
			return false; // Failed to open file

		ifstream.seekg(0, std::ios::end);
		size_t fileSize{ static_cast<size_t>(ifstream.tellg()) };
		ifstream.seekg(0, std::ios::beg);

		unsigned char* fileData{ static_cast<unsigned char*>(std::malloc(fileSize)) };
		if (fileData == nullptr)
			return false; // Memory allocation failed

		if (!ifstream.read(reinterpret_cast<char*>(fileData), fileSize)) {
			std::free(fileData);
			return false; // Failed to read file
		}
		ifstream.close();

		// Load image from memory
		bool success{ loadFromMemory(fileData, fileSize, flipImageOnLoad) };

		// Free temporary memory
		std::free(fileData);

		return success;
	}
	bool ImageRGBA::loadFromMemory(const unsigned char* fileInMemory, size_t size, bool flipImageOnLoad) {
		// Error check
		if (fileInMemory == nullptr || size == 0)
			return false;

		// Free previous data if any
		free();

		// Set vertical flip
		stbi_set_flip_vertically_on_load(flipImageOnLoad);

		// Load image with one channel
		int unusedChannelParameter{ 0 }; // Reason: CHANNELS returns 4, enforcing it to always be 4 channel
		data = reinterpret_cast<glm::u8vec4*>(stbi_load_from_memory(fileInMemory, static_cast<int>(size), &width, &height, &unusedChannelParameter, CHANNELS));

		return data != nullptr;
	}
	bool ImageRGBA::copy(const ImageGray& other) {
		// Error check
		if (other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return false;

		// Release existing data
		free();

		// Allocate memory for copy operation
		size_t pixelCount = other.pixelCount();
		data = reinterpret_cast<glm::u8vec4*>(std::malloc(pixelCount * sizeof(glm::u8vec4)));
		if (data == nullptr)
			return false;
		width = other.width;
		height = other.height;

		// Copy data
		for (size_t i = 0ULL; i < pixelCount; i++)
			data[i] = glm::u8vec4(other.data[i], other.data[i], other.data[i], 255U);

		// Success
		return true;
	}
	bool ImageRGBA::copy(const ImageGrayAlpha& other) {
		// Error check
		if (other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return false;

		// Release existing data
		free();

		// Allocate memory for copy operation
		size_t pixelCount = other.pixelCount();
		data = reinterpret_cast<glm::u8vec4*>(std::malloc(pixelCount * sizeof(glm::u8vec4)));
		if (data == nullptr)
			return false;
		width = other.width;
		height = other.height;

		// Copy data
		for (size_t i = 0ULL; i < pixelCount; i++)
			data[i] = glm::u8vec4(other.data[i][0], other.data[i][0], other.data[i][0], other.data[i][1]);

		// Success
		return true;
	}
	bool ImageRGBA::copy(const ImageRGB& other) {
		// Error check
		if (other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return false;

		// Release existing data
		free();

		// Allocate memory for copy operation
		size_t pixelCount = other.getPixelCount();
		data = reinterpret_cast<glm::u8vec4*>(std::malloc(pixelCount * sizeof(glm::u8vec4)));
		if (data == nullptr)
			return false;
		width = other.width;
		height = other.height;

		// Copy data
		for (size_t i = 0ULL; i < pixelCount; i++)
			data[i] = glm::u8vec4(other.data[i].r, other.data[i].g, other.data[i].b, 255U);

		// Success
		return true;
	}
	bool ImageRGBA::copy(const ImageRGBA& other) {
		// Error check
		if (this == &other || other.width <= 0 || other.height <= 0 || other.data == nullptr)
			return false;

		// Release existing data
		free();

		// Allocate memory for copy operation
		size_t dataSize = other.dataSize();
		data = reinterpret_cast<glm::u8vec4*>(std::malloc(dataSize));
		if (data == nullptr)
			return false;
		width = other.width;
		height = other.height;

		// Copy data
		std::memcpy(data, other.data, dataSize);

		// Success
		return true;
	}
	bool ImageRGBA::saveAsPNG(const std::filesystem::path& path) const {
		const int STRIDE = width * CHANNELS;
		return static_cast<bool>(stbi_write_png(path.string().c_str(), width, height, CHANNELS, data, STRIDE));
	}
	bool ImageRGBA::saveAsJPEG(const std::filesystem::path& path, int quality) const {
		return static_cast<bool>(stbi_write_jpg(path.string().c_str(), width, height, CHANNELS, data, quality));
	}
	bool ImageRGBA::saveAsBMP(const std::filesystem::path& path) const {
		return static_cast<bool>(stbi_write_bmp(path.string().c_str(), width, height, CHANNELS, data));
	}
	bool ImageRGBA::saveAsTGA(const std::filesystem::path& path) const {
		return static_cast<bool>(stbi_write_tga(path.string().c_str(), width, height, CHANNELS, data));
	}
	bool ImageRGBA::save(const std::filesystem::path& path, int quality) const {
		auto ext = path.extension().string();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

		if (ext == ".png") {
			return saveAsPNG(path);
		}
		else if (ext == ".jpg" || ext == ".jpeg") {
			return saveAsJPEG(path, quality);
		}
		else if (ext == ".bmp") {
			return saveAsBMP(path);
		}
		else if (ext == ".tga") {
			return saveAsTGA(path);
		}

		// Unsupported extension
		return false;
	}

	// Functions | pixel manipulation
	size_t ImageRGBA::pixelCount() const {
		return static_cast<size_t>(width) * static_cast<size_t>(height);
	}
	glm::u8vec4 ImageRGBA::pixelAt(int x, int y) const {
		// Error check
		assert(data != nullptr);
		assert(x >= 0 && "rectX < 0");
		assert(y >= 0 && "rectY < 0");
		assert(x < width && " rectX >= rectWidth");
		assert(y < height && "rectY >= rectHeight");
		if (!data || x < 0 || x >= width || y < 0 || y >= height)
			return glm::u8vec4(0u);

		// Get pixel
		size_t index = (static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x));
		return reinterpret_cast<glm::u8vec4*>(data)[index];
	}
	bool ImageRGBA::paintPixel(int x, int y, glm::u8vec4 pixel) {
		// Error check
		assert(data != nullptr);
		assert(x >= 0 && "rectX < 0");
		assert(y >= 0 && "rectY < 0");
		assert(x < width && " rectX >= rectWidth");
		assert(y < height && "rectY >= rectHeight");
		if (data == nullptr || x < 0 || y < 0 || x >= width || y >= height)
			return false;

		// Set pixel
		size_t index = static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x);
		data[index] = pixel;

		// Success
		return true;
	}
	bool ImageRGBA::paintPixel(int x, int y, glm::vec4 pixel) {
		// Error check
		assert(data != nullptr);
		assert(x >= 0 && "rectX < 0");
		assert(y >= 0 && "rectY < 0");
		assert(x < width && " rectX >= rectWidth");
		assert(y < height && "rectY >= rectHeight");
		if (data == nullptr || x < 0 || y < 0 || x >= width || y >= height)
			return false;

		// Set pixel
		size_t index = static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x);
		data[index] = glm::u8vec4(
			static_cast<unsigned char>(std::clamp(pixel[0], 0.0f, 1.0f) * 255.0f),
			static_cast<unsigned char>(std::clamp(pixel[1], 0.0f, 1.0f) * 255.0f),
			static_cast<unsigned char>(std::clamp(pixel[2], 0.0f, 1.0f) * 255.0f),
			static_cast<unsigned char>(std::clamp(pixel[3], 0.0f, 1.0f) * 255.0f)
		);

		// Success
		return true;
	}
	void ImageRGBA::fillRect(int rectX, int rectY, int rectWidth, int rectHeight, const glm::u8vec4& color) {
		// Error check
		assert(data != nullptr);
		assert(rectX >= 0 && "rectX < 0");
		assert(rectY >= 0 && "rectY < 0");
		assert(rectX < width && "rectX >= width");
		assert(rectY < height && "rectY >= height");
		assert(rectX + rectWidth <= width && "rectX + rectWidth is out of image bounds");
		assert(rectY + rectHeight <= height && "rectY + rectHeight is out of image bounds");

		for (int currentY = rectY, yEnd = rectY + rectHeight; currentY < yEnd; ++currentY) {
			for (int currentX = rectX, xEnd = rectX + rectWidth; currentX < xEnd; ++currentX) {
				size_t index = static_cast<size_t>(currentY) * static_cast<size_t>(width) + static_cast<size_t>(currentX);
				data[index] = color;
			}
		}
	}

	// struct ImageView

	// Object | public

	// Constructors | Copy / conversions
	ImageView::ImageView(const ImageGray& other) {
		width = other.getWidth();
		height = other.getHeight();
		channels = other.getChannels();
		data = other.getData();
	}
	ImageView::ImageView(const ImageGrayAlpha& other) {
		width = other.getWidth();
		height = other.getHeight();
		channels = other.getChannels();
		data = reinterpret_cast<unsigned char*>(other.getData());
	}
	ImageView::ImageView(const ImageRGB& other) {
		width = other.getWidth();
		height = other.getHeight();
		channels = other.getChannels();
		data = reinterpret_cast<unsigned char*>(other.getData());
	}
	ImageView::ImageView(const ImageRGBA& other) {
		width = other.getWidth();
		height = other.getHeight();
		channels = other.getChannels();
		data = reinterpret_cast<unsigned char*>(other.getData());
	}

	// Operators | assignment
	ImageView& ImageView::operator=(const ImageGray& other) {
		width = other.getWidth();
		height = other.getHeight();
		channels = other.getChannels();
		data = other.getData();
		return *this;
	}
	ImageView& ImageView::operator=(const ImageGrayAlpha& other) {
		width = other.getWidth();
		height = other.getHeight();
		channels = other.getChannels();
		data = reinterpret_cast<unsigned char*>(other.getData());
		return *this;
	}
	ImageView& ImageView::operator=(const ImageRGB& other) {
		width = other.getWidth();
		height = other.getHeight();
		channels = other.getChannels();
		data = reinterpret_cast<unsigned char*>(other.getData());
		return *this;
	}
	ImageView& ImageView::operator=(const ImageRGBA& other) {
		width = other.getWidth();
		height = other.getHeight();
		channels = other.getChannels();
		data = reinterpret_cast<unsigned char*>(other.getData());
		return *this;
	}

	// Functions
	size_t ImageView::pixelCount() const {
		return static_cast<size_t>(width) * static_cast<size_t>(height) * static_cast<size_t>(channels);
	}
	size_t ImageView::dataSize() const {
		return sizeof(unsigned char) * pixelCount();
	}
	unsigned char* ImageView::pixelAt(int x, int y) const {
		// Error check
		assert(data != nullptr);
		assert(x >= 0 && "rectX < 0");
		assert(y >= 0 && "rectY < 0");
		assert(x < width && " rectX >= rectWidth");
		assert(y < height && "rectY >= rectHeight");
		if (data == nullptr || x < 0 || y < 0 || x >= width || y >= height)
			return nullptr;

		return data + (width * y + x) * channels;
	}
	bool ImageView::hasData() const {
		return data != nullptr;
	}

	// struct ImageViewGray

	// Object | public

	// Constructors | Copy / conversions
	ImageViewGray::ImageViewGray(const ImageGray& other) {
		width = other.getWidth();
		height = other.getHeight();
		channels = other.getChannels();
		data = other.getData();
	}

	// Operators | conversions
	ImageViewGray& ImageViewGray::operator=(const ImageGray& other) {
		width = other.getWidth();
		height = other.getHeight();
		channels = other.getChannels();
		data = other.getData();
		return *this;
	}

	// Functions
	size_t ImageViewGray::pixelCount() const {
		return static_cast<size_t>(width) * static_cast<size_t>(height);
	}
	size_t ImageViewGray::dataSize() const {
		return sizeof(unsigned char) * pixelCount();
	}
	unsigned char ImageViewGray::pixelAt(int x, int y) const {
		// Error check
		assert(data != nullptr);
		assert(x >= 0 && "rectX < 0");
		assert(y >= 0 && "rectY < 0");
		assert(x < width && " rectX >= rectWidth");
		assert(y < height && "rectY >= rectHeight");
		if (data == nullptr || x < 0 || y < 0 || x >= width || y >= height)
			return 0U;

		// Get pixel
		size_t index = (static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x));
		return data[index];
	}
	bool ImageViewGray::hasData() const {
		return data != nullptr;
	}

	// struct ImageViewGrayAlpha

	// Object | public

	// Constructors | Copy / conversions
	ImageViewGrayAlpha::ImageViewGrayAlpha(const ImageGrayAlpha& other) {
		width = other.getWidth();
		height = other.getHeight();
		channels = other.getChannels();
		data = other.getData();
	}

	// Operators | conversions
	ImageViewGrayAlpha& ImageViewGrayAlpha::operator=(const ImageGrayAlpha& other) {
		width = other.getWidth();
		height = other.getHeight();
		channels = other.getChannels();
		data = other.getData();
		return *this;
	}

	// Functions
	size_t ImageViewGrayAlpha::pixelCount() const {
		return static_cast<size_t>(width) * static_cast<size_t>(height);
	}
	size_t ImageViewGrayAlpha::dataSize() const {
		return sizeof(glm::u8vec2) * pixelCount();
	}
	glm::u8vec2 ImageViewGrayAlpha::pixelAt(int x, int y) const {
		// Error check
		assert(data != nullptr);
		assert(x >= 0 && "rectX < 0");
		assert(y >= 0 && "rectY < 0");
		assert(x < width && " rectX >= rectWidth");
		assert(y < height && "rectY >= rectHeight");
		if (data == nullptr || x < 0 || y < 0 || x >= width || y >= height)
			return glm::u8vec2(0u);

		// Get pixel
		size_t index = (static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x));
		return data[index];
	}
	bool ImageViewGrayAlpha::hasData() const {
		return data != nullptr;
	}

	// struct ImageViewGray

	// Object | public

	// Constructors | Copy / conversions
	ImageViewRGB::ImageViewRGB(const ImageRGB& other) {
		width = other.getWidth();
		height = other.getHeight();
		channels = other.getChannels();
		data = other.getData();
	}

	// Operators | conversions
	ImageViewRGB& ImageViewRGB::operator=(const ImageRGB& other) {
		width = other.getWidth();
		height = other.getHeight();
		channels = other.getChannels();
		data = other.getData();
		return *this;
	}

	// Functions
	size_t ImageViewRGB::pixelCount() const {
		return static_cast<size_t>(width) * static_cast<size_t>(height);
	}
	size_t ImageViewRGB::dataSize() const {
		return sizeof(unsigned char) * pixelCount();
	}
	glm::u8vec3 ImageViewRGB::pixelAt(int x, int y) const {
		// Error check
		assert(data != nullptr);
		assert(x >= 0 && "rectX < 0");
		assert(y >= 0 && "rectY < 0");
		assert(x < width && " rectX >= rectWidth");
		assert(y < height && "rectY >= rectHeight");
		if (data == nullptr || x < 0 || y < 0 || x >= width || y >= height)
			return glm::u8vec3(0u);

		// Get pixel
		size_t index = (static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x));
		return data[index];
	}
	bool ImageViewRGB::hasData() const {
		return data != nullptr;
	}

	// struct ImageViewGray

	// Object | public

	// Constructors | Copy / conversions
	ImageViewRGBA::ImageViewRGBA(const ImageRGBA& other) {
		width = other.getWidth();
		height = other.getHeight();
		channels = other.getChannels();
		data = other.getData();
	}

	// Operators | conversions
	ImageViewRGBA& ImageViewRGBA::operator=(const ImageRGBA& other) {
		width = other.getWidth();
		height = other.getHeight();
		channels = other.getChannels();
		data = other.getData();
		return *this;
	}

	// Functions
	size_t ImageViewRGBA::pixelCount() const {
		return static_cast<size_t>(width) * static_cast<size_t>(height);
	}
	size_t ImageViewRGBA::dataSize() const {
		return sizeof(unsigned char) * pixelCount();
	}
	glm::u8vec4 ImageViewRGBA::pixelAt(int x, int y) const {
		// Error check
		assert(data != nullptr);
		assert(x >= 0 && "rectX < 0");
		assert(y >= 0 && "rectY < 0");
		assert(x < width && " rectX >= rectWidth");
		assert(y < height && "rectY >= rectHeight");
		if (data == nullptr || x < 0 || y < 0 || x >= width || y >= height)
			return glm::u8vec4(0U);

		// Get pixel
		size_t index = (static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x));
		return data[index];
	}
	bool ImageViewRGBA::hasData() const {
		return data != nullptr;
	}
}
