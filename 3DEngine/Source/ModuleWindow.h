#ifndef __MODULEWINDOW_H__
#define __MODULEWINDOW_H__

#include "Module.h"
#include "SDL/include/SDL.h"


//class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	void SetTitle(const char* title);

	void DrawConfig();

	// JSON Save/Load Configuration
	void LoadModuleConfig(Config_Json& config);
	void SaveModuleConfig(Config_Json& config);

private:
	void SetRes(int index);

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

	const char* res_array[4] = { "1024x768" , "1280x720" , "1280x1024" , "1920x1080" };
	
	int		width = SCREEN_WIDTH;
	int		height = SCREEN_HEIGHT;
	int		resolution = 1;
	bool	fullscreen = false;	
	bool	resizable = false;
	bool	borderless = false;
	bool	fullscreen_desktop = false;
};

#endif // __MODULEWINDOW_H__