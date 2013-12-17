#ifndef MUSIC_HPP
#define MUSIC_HPP

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// SDL headers
#include <SDL/SDL.h>
#include <SDL2/SDL_mixer.h>

class Music
{

    unsigned short chunk_volume, background_volume;
    Mix_Music *background;
    Mix_Chunk *chunk;

public:

    Music(unsigned short chunk_volume, 
          unsigned short background_volume,
	  const char* background_file,
	  const char* chunk_file);
    void play_background();
    void pause_background();
    void resume_background();
    void play_chunk();
    unsigned short get_background_volume();
    unsigned short get_chunk_volume();
    void set_background_volume(const unsigned short volume);
    void set_chunk_volume(const unsigned short volume);


};


#endif