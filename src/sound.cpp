//
//  Chip-8
//
//  Created by Cosme Jordan on 12.09.20.
//  Copyright © 2020 Cosme Jordan. All rights reserved.
//

#include "sound.hpp"

void callback(void *_beeper, Uint8 *_stream, int _length);

SimpleSound::SimpleSound() {
    SDL_AudioSpec specification;
    SDL_AudioSpec finalSpecification;

    // Set elements of the specification
    specification.freq = FREQUENCY;
    specification.format = AUDIO_S16SYS;
    specification.callback = callback;
    specification.userdata = this;
    specification.channels = NUMBER_CHANNELS;
    specification.samples = NUMBER_SAMPLES;

    SDL_OpenAudio(&specification, &finalSpecification);

    // Start playing audio
    SDL_PauseAudio(0);
}

void callback(void *simpleSound, Uint8 *stream, int length) {
    SimpleSound* newSimpleSound = (SimpleSound*) simpleSound;
    Sint16 *newStream = (Sint16*) stream;

    newSimpleSound->generateWave(newStream, length/2);
}

SimpleSound::~SimpleSound() {
    SDL_CloseAudio();
}

void SimpleSound::generateWave(Sint16 *stream, int length) {
    int counter = 0;
    while(counter < length) {
        if(sounds.empty()) {
            while (counter < length) {
                stream[counter] = 0;
                counter++;
            }
            return;
        }

        SoundPair& soundPair = sounds.front();

        
        int samplesLeft = std::min(counter + soundPair.samplesLeft, length);
        soundPair.samplesLeft -= samplesLeft - counter;

        while(counter < samplesLeft) {
            stream[counter] = AMPLITUDE * std::sin(2 * M_PI * linearSpeed / FREQUENCY);
            linearSpeed += soundPair.frequency;
            counter++;
        }

        if(soundPair.samplesLeft == 0) {
            sounds.pop();
        }
    }
}

void SimpleSound::emitSound(double frequency, int duration) {
    SoundPair soundPair;
    soundPair.frequency = frequency;
    soundPair.samplesLeft = duration * FREQUENCY / 1000;

    SDL_LockAudio();
    sounds.push(soundPair);
    SDL_UnlockAudio();
}

void SimpleSound::wait() {
    int soundSize;

    do {
        SDL_Delay(SDL_DELAY);
        SDL_LockAudio();
        soundSize = sounds.size();
        SDL_UnlockAudio();
    } while (soundSize > 0);

}

void SimpleSound::play(double frequency, int duration) {
    emitSound(frequency, duration);
    wait();
}
