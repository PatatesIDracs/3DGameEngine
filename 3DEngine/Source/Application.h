#pragma once

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"
#include "ModuleEditor.h"

#include <list>

class Config_Json;

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleSceneIntro* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModulePhysics3D* physics;
	ModuleEditor* editor;

private:

	Timer	ms_timer;
	float	dt;
	std::list<Module*> list_modules;

	float avg_fps = 0;					//Average fps
	float last_frame_time = 0.0f;		//ms of the last frame
	Timer start_up_time;				//Time since startup
	Timer last_sec_timer;				//Timer to control the seconds between each drame
	Uint64 total_frame_count = 0;		//Total frames from the application start
	Uint32 curr_sec_frame_count = 0;	//Frames in the current second (we can't display them as they are constatnly increasing during the second)
	Uint32 last_sec_frame_count = 0;	//Frames in the last second (can be displayed)

	//fps plot array
	std::queue<float>	ms_counter;
	std::queue<int>		fps_counter;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void OpenBrowser(const char* url);

	const std::list<Module*>* GetModulesList();

	int		GetAvgFrameRate();
	Uint32	GetLastSecFrames();
	float	GetLastFrameTime();

	std::queue<float>*	GetMs();
	std::queue<int>*	GetFPS();


private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

	// JSON Save/Load Configuration
	void LoadConfig(const char* filename);
	void SaveConfig(const char* filename);

};
