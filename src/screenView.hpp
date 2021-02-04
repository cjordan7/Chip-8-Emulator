//
//  ScreenView.hpp
//  Chip-8
//
//  Created by Cosme Jordan on 12.09.20.
//  Copyright © 2020 Cosme Jordan. All rights reserved.
//

#ifndef screenView_hpp
#define screenView_hpp

#include <iostream>
#include <SDL.h>
#include <string>

struct SDLWindowSpecification {
    int height;
    int width;
    int textureHeight;
    int textureWidth;
    
    char const* screenTitle;
};

class ScreenView {
private:
    SDLWindowSpecification sdlWindowSpecification;
    
    SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Texture* texture = NULL;

public:
    ScreenView(SDLWindowSpecification& sdlWindowSpecification);
    ~ScreenView();
    void initSDL();
    void destorySDL();
    
    void draw(void const* buffer, int pitch);
    bool inputKeys(uint8_t* keys);
};

#endif /* screenView_hpp */
