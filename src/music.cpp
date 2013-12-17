#include "music.hpp"

using namespace std;

Music::Music(
	      const unsigned short background_volume, // percent
              const unsigned short chunk_volume, // percent
	      const char* background_file,
	      const char* chunk_file)
{

    if ( SDL_Init(SDL_INIT_AUDIO) < 0 ) 
    {
        cout << "Failed to initialize sound module." << endl;
    }

    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);

    background = Mix_LoadMUS(background_file);
    if (!background)
    {
        cout << "Unable to open background sound file: " << background_file << endl;
    }
    
    chunk = Mix_LoadWAV(chunk_file);
    if (!chunk)
    {
        cout << "Unable to open chunk sound file: " << chunk_file << endl;
    }
    
    set_background_volume(background_volume);
    set_chunk_volume(chunk_volume);
        
}

void Music::play_background()
{
    Mix_PlayMusic(background,-1);
}

void Music::pause_background()
{
    Mix_PauseMusic();
}

void Music::resume_background()
{
    Mix_ResumeMusic();
}
 
void Music::play_chunk()
{
    Mix_PlayChannel(-1,chunk,0); 
}

unsigned short Music::get_background_volume() 
{
    return background_volume;
}

unsigned short Music::get_chunk_volume()
{
    return chunk_volume;
}

void Music::set_background_volume(const unsigned short volume)
{
    if ( (volume >= 0) && (volume <= 100) && (background))
    {
        background_volume = volume;
        Mix_VolumeMusic((background_volume * MIX_MAX_VOLUME) / 100);
    }
    else
    {
 
    }     
}

void Music::set_chunk_volume(const unsigned short volume)
{
    if ( (volume >= 0) && (volume <= 100) && (chunk))
    {
        chunk_volume = volume;
        Mix_VolumeChunk(chunk,(chunk_volume * MIX_MAX_VOLUME) / 100);
    }
    else
    {
 
    }     
}