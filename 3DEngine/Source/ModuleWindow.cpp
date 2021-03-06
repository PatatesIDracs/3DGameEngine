#include "Application.h"
#include "ModuleWindow.h"
#include "Imgui\imgui.h"
#include "ConfigJSON.h"

ModuleWindow::ModuleWindow(Application* app, bool start_enabled) : Module(app,"Window",start_enabled)
{
	window = NULL;
	screen_surface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOGC("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOGC("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(fullscreen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(resizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(borderless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(fullscreen_desktop == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(App->GetAppName(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width*SCREEN_SIZE, height*SCREEN_SIZE, flags);

		if(window == NULL)
		{
			LOGC("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}



	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOGC("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

void ModuleWindow::DrawConfig()
{

	if (ImGui::Combo("Resolution", &resolution, res_array, (int)(sizeof(res_array) / sizeof(*res_array))))
	{
		SetRes(resolution);
		SDL_SetWindowSize(window, width, height);
	}
	if (ImGui::Checkbox("Fullscren", &fullscreen))
	{
		if (fullscreen)
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		else
			SDL_SetWindowFullscreen(window, 0);
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Full Desktop", &fullscreen_desktop))
	{
		if (fullscreen_desktop)
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		else
			SDL_SetWindowFullscreen(window, 0);
	}

	if (ImGui::Checkbox("Resizable", &resizable))
		SDL_SetWindowResizable(window, (SDL_bool)resizable);
	ImGui::SameLine();
	if (ImGui::Checkbox("Borderless", &borderless))
		SDL_SetWindowBordered(window, (SDL_bool)!borderless);
}

// ----------------------------------------------
void ModuleWindow::LoadModuleConfig(Config_Json & config)
{
	// Window Configuration
	width = config.GetInt("Width", SCREEN_WIDTH);
	height = config.GetInt("Height", SCREEN_HEIGHT);
	resolution = config.GetInt("Resolution", 1);
	fullscreen = config.GetBool("Fullscreen", WIN_FULLSCREEN);
	resizable = config.GetBool("Resizable", WIN_RESIZABLE);
	borderless = config.GetBool("Borderless", WIN_BORDERLESS);
	fullscreen_desktop = config.GetBool("Fullscreen Descktop", WIN_FULLSCREEN_DESKTOP);
}

void ModuleWindow::SaveModuleConfig(Config_Json & config)
{
	Config_Json window_config = config.AddJsonObject(this->GetName());
	window_config.SetInt("Width", width);
	window_config.SetInt("Height", height);
	window_config.SetInt("Resolution", resolution);
	window_config.SetBool("Fullscreen", fullscreen);
	window_config.SetBool("Resizable", resizable);
	window_config.SetBool("Borderless", borderless);
	window_config.SetBool("Fullscreen Descktop", fullscreen_desktop);
}

void ModuleWindow::SetRes(int index)
{
	switch (index)
	{
	case 0:
		width = 1024;
		height = 768;
		break;
	case 1:
		width = 1280;
		height = 720;
		break;
	case 2:
		width = 1280;
		height = 1024;
		break;
	case 3:
		width = 1920;
		height = 1080;
		break;
	default:
		break;
	}
}
