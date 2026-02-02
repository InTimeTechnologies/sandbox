// Dependencies | std
#include <iostream>
#include <string>

// Dependencies | media
#include <media/Image.h>

int main() {
	std::cout << "Program operating." << std::endl;

	{
		it::ImageRGBA imageRGBA = it::ImageRGBA(100, 100);
		imageRGBA.fillRect(0, 0, 100, 100, glm::vec4(55, 255, 55, 255));
		imageRGBA.fillRect(25, 0, 50, 50, glm::vec4(255, 255, 0, 255));
		imageRGBA.saveAsPNG(std::filesystem::current_path() / "fill_test.png");
	}

	std::cout << "Program terminating." << std::endl;
}
