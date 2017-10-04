#pragma once


#include <cstdlib>

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "ModuleLoadFBX.h"


class Config_Json;
class Profiler;

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleSceneIntro* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleEditor* editor;
	ModuleLoadFBX* assimp;

private:

	Timer	ms_timer;
	float	dt;
	std::list<Module*> list_modules;

	std::string app_name;

	float avg_fps = 0;					//Average fps
	float last_frame_time = 0.0f;		//ms of the last frame
	Timer start_up_time;				//Time since startup
	Timer last_sec_timer;				//Timer to control the seconds between each drame
	Uint64 total_frame_count = 0;		//Total frames from the application start
	Uint32 curr_sec_frame_count = 0;	//Frames in the current second (we can't display them as they are constatnly increasing during the second)
	Uint32 last_sec_frame_count = 0;	//Frames in the last second (can be displayed)

	//fps plot array
	std::vector<float>	ms_counter;
	std::vector<float>	fps_counter;

	// Vector of Profilers (One for each Module)
	std::vector<Profiler*> profiler;
	bool init_record = false;

	int capped_ms = 16;
	bool capped = true;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	const char* GetAppName() const;

	void OpenBrowser(const char* url);

	const std::list<Module*>* GetModulesList();

	int		GetAvgFrameRate();
	Uint32	GetLastSecFrames();
	float	GetLastFrameTime();

	std::vector<float>*	GetMs();
	std::vector<float>*	GetFPS();

	void DoRecord();
	void SwitchProfilerState();
	Profiler* GetProfiler(Module* module);
	const std::vector<Profiler*>* GetProfilerVect();

	void SetFpsCap(int cap);

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

	// JSON Save/Load Configuration
	void LoadConfig(const char* filename);
	void SaveConfig(const char* filename);

	void LoadModuleConfig(Config_Json& config);
	void SaveModuleConfig(Config_Json& config);
	
};

extern Application* App;