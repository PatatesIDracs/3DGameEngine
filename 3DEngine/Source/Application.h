#ifndef __APPLICATION_H__
#define __APPLICATION_H__

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
#include "ModuleResources.h"


class Config_Json;
class Profiler;

enum APPSTATE
{
	APP_PLAY,
	APP_PAUSE,
	APP_TICK,
	APP_STOP
};

struct ApplicationTime
{
	// Real Time
	Timer real_time;
	uint frame_count = 0;
	float real_delta_time = 0.0f;

	// Game Time
	float game_time = 0.0f;
	float time_scale = 1.0f;
	float delta_time = 0.0f;

	// Current App State
	APPSTATE state = APP_STOP;

	void ChangeState(APPSTATE new_state) {
		state = new_state;
		
		if (state == APP_PLAY) game_time = 0.0f;
	};

	void Updatedt(float& dt) {

		real_delta_time = dt;
		frame_count++;

		if (state != APP_PAUSE && state != APP_STOP) { 
			dt = time_scale*dt; 
			game_time += dt;

			if (state == APP_TICK)
				state = APP_PAUSE;
		}
		else dt = 0.0f;

		delta_time = dt;
	}
};

// Application --------------------------
class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleSceneIntro* scene_intro;
	ModuleResources* resources;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleEditor* editor;

	ApplicationTime clock;
	int fps = 60;

private:

	Timer	ms_timer;
	float	dt;
	std::list<Module*> list_modules;

	std::string app_name;

	LCG UUIDGen;

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

	bool want_to_save_scene = false;
	bool want_to_load_scene = false;
	std::string file_to_load;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	const char* GetAppName() const;

	int GetIntUUID();

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

	bool AreFpsCapped();
	void SetFpsCap(bool fps_uncapped);

	void SaveScene();
	void LoadScene();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

	// JSON Save/Load Configuration
	void LoadConfig(const char* filename);
	void SaveConfig(const char* filename);

	void LoadModuleConfig(Config_Json& config);
	void SaveModuleConfig(Config_Json& config);

	void LoadSceneNow();
	void SaveSceneNow();
	
};

extern Application* App;

#endif // !__APPLICATION_H__