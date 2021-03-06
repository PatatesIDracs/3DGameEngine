
#include "Application.h"
#include "ModuleAudio.h"
#include "ConfigJSON.h"

#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

ModuleAudio::ModuleAudio(Application* app, bool start_enabled) : Module(app, "Audio", start_enabled), music(NULL)
{}

// Destructor
ModuleAudio::~ModuleAudio()
{}

// Called before render is available
bool ModuleAudio::Init()
{
	LOGC("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOGC("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	// load support for the OGG format
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if((init & flags) != flags)
	{
		LOGC("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		ret = false;
	}

	//Initialize SDL_mixer
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOGC("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		ret = true;
	}

	return ret;
}

// Called before quitting
bool ModuleAudio::CleanUp()
{
	LOGC("Freeing sound FX, closing Mixer and Audio subsystem");

	if(music != NULL)
	{
		Mix_FreeMusic(music);
	}

	/*p2List_item<Mix_Chunk*>* item;

	for(item = fx.getFirst(); item != NULL; item = item->next)
	{
		Mix_FreeChunk(item->data);
	}
	*/
	fx.clear();
	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	return true;
}

bool ModuleAudio::StopMusic()
{
	bool ret = true;
	if (music != NULL)
	{
		Mix_PauseMusic();
		if (Mix_PausedMusic == 0)
		{
			LOGC("Music wasn't paused. Mix_GetError(): %s", Mix_GetError());
			ret = false;
		}
		else
		{
			LOGC("Music Paused");
		}
	}
	return ret;
}

bool ModuleAudio::ResumeMusic()
{
	bool ret = true;
	if (music != NULL)
	{
		Mix_ResumeMusic();
		if (Mix_Playing(-1) == 0)
		{
			LOGC("Music is now playing.");
			ret = false;
		}
		else
		{
			LOGC("Music dosent play. Mix_GetError(): %s", Mix_GetError());
		}
	}
	return ret;
}

void ModuleAudio::VolumeMusic(int volume)
{
	if (music != NULL)
	{
		LOGC("volume was    : %d\n", Mix_VolumeMusic(MIX_MAX_VOLUME / 2));
		Mix_VolumeMusic(volume);
		LOGC("volume is now : %d\n", Mix_VolumeMusic(-1));
	}
}

// ----------------------------------------------
void ModuleAudio::LoadModuleConfig(Config_Json & config)
{
	int test = config.GetInt("NoNumber", 0);
}

void ModuleAudio::SaveModuleConfig(Config_Json & config)
{
	Config_Json audio_config = config.AddJsonObject(this->GetName());
	audio_config.SetBool("Is Active", true);
}

// ----------------------------------------------
// Play a music file
bool ModuleAudio::PlayMusic(const char* path, float fade_time)
{
	bool ret = true;
	
	if(music != NULL)
	{
		if(fade_time > 0.0f)
		{
			Mix_FadeOutMusic((int) (fade_time * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS(path);

	if(music == NULL)
	{
		LOGC("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if(fade_time > 0.0f)
		{
			if(Mix_FadeInMusic(music, -1, (int) (fade_time * 1000.0f)) < 0)
			{
				LOGC("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if(Mix_PlayMusic(music, -1) < 0)
			{
				LOGC("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	LOGC("Successfully playing %s", path);
	return ret;
}

// Load WAV
unsigned int ModuleAudio::LoadFx(const char* path)
{
	unsigned int ret = 0;

	Mix_Chunk* chunk = Mix_LoadWAV(path);

	if(chunk == NULL)
	{
		LOGC("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
	//	fx.add(chunk);
	//	ret = fx.count();
	}

	return ret;
}

// Play WAV
bool ModuleAudio::PlayFx(unsigned int id, int repeat)
{
	bool ret = false;

	Mix_Chunk* chunk = NULL;
	/*
	if(fx.at(id-1, chunk) == true)
	{
		Mix_PlayChannel(-1, chunk, repeat);
		ret = true;
	}
	*/
	return ret;
}