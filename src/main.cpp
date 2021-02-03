
#include <iostream>

#include "cpu.hpp"
#include "screenView.hpp"

const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;

// FIXME: TODO: Refactor using OpenGl
// FIXME: TODO: COMMENTS
// FIXME: TODO: Use constants everywhere

/// Keyboard is mapped as followed
/// Original Chip8 keyboard -> Chip8 Emulator Keyboard
///
/// | 1, 2, 3, 4 | -> | 1, 2, 3, 4 |
/// | 4, 5, 6, D | -> | Q, W, E, R |
/// | 7, 8, 9, E | -> | A, S, D, F |
/// | A, 0, B, F | -> | Z, X, C, V |
///
/// Consider finding a better keyboard

void checkExtension(char const* filename) {
    std::string temp(filename);
    std::cout << temp << std::endl;
    if(temp.substr(temp.find_last_of(".") + 1) != "ch8") {
        std::cerr << "=======================================================" << std::endl;
        std::cerr << "=======================================================" << std::endl;
        std::cerr << "Error: The extension of the file should be of type .ch8" << std::endl;
        std::cerr << "=======================================================" << std::endl;
        std::cerr << "=======================================================" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[]) {
    // FIXME: TODO: Use function, move to class Chip8Controller,
    /// and call functions

    if (argc != 4) {
        std::cerr << "Usage: "
                  << argv[0]
                  << " ScaleNumber DelayNumber PathToROM"
                  << std::endl;
		std::exit(EXIT_FAILURE);
	}
 
	int videoScale = std::stoi(argv[1]);
	int cycleDelay = std::stoi(argv[2]);
	char const* romFilename = argv[3];

    checkExtension(romFilename);
    SDLWindowSpecification sdlWindowSpecification;
    sdlWindowSpecification.screenTitle = "Chip-8 Emulator";
    sdlWindowSpecification.width = VIDEO_WIDTH * videoScale;
    sdlWindowSpecification.height = VIDEO_HEIGHT * videoScale;
    sdlWindowSpecification.textureWidth = VIDEO_WIDTH;
    sdlWindowSpecification.textureHeight = VIDEO_HEIGHT;

    ScreenView screenView(sdlWindowSpecification);
    screenView.initSDL();

	CPU* chip8 = new CPU();
	chip8->loadROM(romFilename);

	int pitch = sizeof(chip8->screen[0]) * VIDEO_WIDTH;

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

	while (!quit) {
		quit = screenView.inputKeys(chip8->keyboard);

		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > cycleDelay) {
			lastCycleTime = currentTime;

			chip8->runCycle();
            screenView.draw(chip8->screen, pitch);
		}
	}
 
    screenView.destorySDL();
    delete chip8;

    return 0;
}
