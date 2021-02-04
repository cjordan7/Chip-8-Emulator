//
//  Chip-8
//
//  Created by Cosme Jordan on 12.09.20.
//  Copyright © 2020 Cosme Jordan. All rights reserved.
//


#ifndef sound_hpp
#define sound_hpp

#include <queue>
#include <cmath>

#include <SDL.h>
#include <SDL_audio.h>

struct SoundPair {
    double frequency;
    int samplesLeft;
};

class SimpleSound {
private:
    static const int AMPLITUDE = 28000;
    static const int FREQUENCY = 4400;
    static const int NUMBER_SAMPLES = 256;
    static const int NUMBER_CHANNELS = 1;
    static const int SDL_DELAY = 10;
    
    double linearSpeed;
    std::queue<SoundPair> sounds;

public:
    SimpleSound();
    ~SimpleSound();

public:
    void play(double frequency, int duration);
    void generateWave(Sint16 *stream, int length);

private:
    void emitSound(double frequency, int duration);
    void wait();
};

#endif /* sound_hpp */

