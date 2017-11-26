#include "Application.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_sdl_gl3.h"

#include "Glew/include/glew.h"

#include "parson.h"
#include "ConfigJSON.h"

#include "Profiler.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	resources = new ModuleResources(this, true);
	scene_intro = new ModuleSceneIntro(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	editor = new ModuleEditor(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);
	AddModule(resources);	//Must be loaded before scene

	// Scenes
	AddModule(scene_intro);

	// Renderer last!
	AddModule(renderer3D);
	//Render UI last
	AddModule(editor);

}

Application::~Application()
{
	app_name.clear();

	std::list<Module*>::iterator item = list_modules.begin();

	while (item != list_modules.end())
	{
		delete item._Ptr->_Myval;
		item++;
	}

	list_modules.clear();
}

bool Application::Init()
{
	bool ret = true;

	// Realtime Start
	clock.real_time.Start();

	// Load Gloval Configuration
	LoadConfig(JOPE_DATA_DIRECTORY JOPE_CONFIG_FILENAME);

	// Call Init() in all modules
	std::list<Module*>::iterator item = list_modules.begin();

	while (item != list_modules.end() && ret == true)
	{
		ret = item._Ptr->_Myval->Init();
		item++;
	}

	// After all Init calls we call Start() in all modules
	LOGC("Application Start --------------");
	item = list_modules.begin();

	while (item != list_modules.end() && ret == true)
	{
		ret = item._Ptr->_Myval->Start();
		item++;
	}

	// Create Profilers for Modules
	profiler.reserve(list_modules.size());
	for (uint i = 0; i < list_modules.size(); i++)
	{
		profiler.push_back(new Profiler());
	}

	ms_timer.Start();
	return ret;
}



// ---------------------------------------------
void Application::LoadConfig(const char* filename)
{
	Config_Json config(filename);

	LoadModuleConfig(config.GetJsonObject("Application"));

	// Call LoadModuleConfig() in all modules
	std::list<Module*>::iterator item = list_modules.begin();

	while (item != list_modules.end())
	{
		item._Ptr->_Myval->LoadModuleConfig(config.GetJsonObject(item._Ptr->_Myval->GetName()));
		item++;
	}
}

void Application::SaveConfig(const char* filename)
{
	Config_Json config(filename);

	SaveModuleConfig(config);

	// Call SaveModuleConfig() in all modules
	std::list<Module*>::iterator item = list_modules.begin();

	while (item != list_modules.end())
	{
		item._Ptr->_Myval->SaveModuleConfig(config);
		item++;
	}

	// Serialize Data to file
	config.SaveToFile(filename);
}

void Application::DoRecord()
{
	init_record = true;
}

void Application::SwitchProfilerState()
{
	int p_size = profiler.size();
	for (int count = 0; count < p_size; count++)
		profiler[count]->PauseRecord();
}

Profiler * Application::GetProfiler(Module * module)
{
	int count = 0;
	std::list<Module*>::iterator item = list_modules.begin();
	
	for(; item != list_modules.end(); item++)
	{ 
		if (module == item._Ptr->_Myval) return profiler[count];
		count++;
	}
	
	return nullptr;
}

const std::vector<Profiler*>* Application::GetProfilerVect()
{
	return &profiler;
}

void Application::LoadModuleConfig(Config_Json & config)
{
	app_name = config.GetString("Name");
	capped = config.GetBool("Is Capped", true);
	fps = config.GetInt("Capped FPS", 60);
}

void Application::SaveModuleConfig(Config_Json & config)
{
	Config_Json app_config = config.AddJsonObject("Application");
	app_config.SetString("Name", "JoPe Engine");
	app_config.SetBool("Is Capped", capped);
	app_config.SetInt("Capped FPS", fps);
	app_config.SetBool("Is Active", true);
}


bool Application::AreFpsCapped()
{
	return capped;
}

void Application::SetFpsCap(bool fps_uncapped)
{
	capped = !fps_uncapped;
	if (capped)
		capped_ms = (1000 / fps);
}

void Application::SaveScene(const char* file_name)
{
	file_to_save = file_name;
	want_to_save_scene = true;
}

void Application::LoadScene(int file_id)
{
	file_to_load_uid = file_id;
	want_to_load_scene = true;
}

void Application::LoadSceneNow()
{
	scene_intro->LoadScene(file_to_load_uid);
	want_to_load_scene = false;
}

void Application::SaveSceneNow()
{
	if (!file_to_save.empty())
		scene_intro->SaveScene(file_to_save.c_str());
	want_to_save_scene = false;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	total_frame_count++;
	curr_sec_frame_count++;

	if (init_record) {
		SwitchProfilerState();
		init_record = false;
	}
	dt = ((float)ms_timer.Read())/ 1000.0f;
	clock.Updatedt(dt);
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	//Scene Save and Load
	if (want_to_save_scene)
		SaveSceneNow();
	if (want_to_load_scene)
		LoadSceneNow();


	//---FRAMERATE CALCULATIONS---
	//When a second has passed...
	if (last_sec_timer.Read() > 1000)
	{
		last_sec_timer.Start();							//Reset the timer
		last_sec_frame_count = curr_sec_frame_count;	//Pass the current sec frame count to the last sec frame count
		curr_sec_frame_count = 0;						//Reset frame count
	}

	avg_fps = (float)total_frame_count / start_up_time.ReadSec();
	last_frame_time = (float) ms_timer.Read();

	//List of the last 
	if (fps_counter.size() > 50)
	{
		for (uint i = 0; i < fps_counter.size() - 1; i++)
		{
			fps_counter[i] = fps_counter[i + 1];
		}
		fps_counter.pop_back();
	}
	fps_counter.push_back((float)last_sec_frame_count);


	//Frame limit
	if (capped)
	{
		if (capped_ms > 0 && last_frame_time < capped_ms)
		{
			SDL_Delay(capped_ms - (int)last_frame_time);
		}
	}

	//List of the las 50 frames time
	if (ms_counter.size() > 50)
	{
		for (uint i = 0; i < ms_counter.size() - 1; i++)
		{
			ms_counter[i] = ms_counter[i + 1];
		}
		ms_counter.pop_back();
	}
	ms_counter.push_back((float)ms_timer.Read());

}	

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();

	std::list<Module*>::iterator item = list_modules.begin();

	int count = 0;
	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		if (profiler[count]->StartTimer()) profiler[count]->SetTitle("PreUpdate");
		ret = item._Ptr->_Myval->PreUpdate(dt);
		profiler[count]->AddTimeToFrame();
		item++;
		count++;
	}

	item = list_modules.begin();

	count = 0;
	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		if (profiler[count]->StartTimer()) profiler[count]->SetTitle("Update");
		ret = item._Ptr->_Myval->Update(dt);
		profiler[count]->AddTimeToFrame();
		item++;
		count++;
	}

	item = list_modules.begin();

	count = 0;
	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		if (profiler[count]->StartTimer()) profiler[count]->SetTitle("PostUpdate");
		ret = item._Ptr->_Myval->PostUpdate(dt);
		profiler[count]->AddTimeToFrame();
		item++;
		count++;
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	std::list<Module*>::iterator item = list_modules.end();

	while (item != list_modules.begin() && ret == true)
	{
		item--;
		ret = item._Ptr->_Myval->CleanUp();
	}

	// Clear profile vector;
	for (uint i = 0; i < profiler.size(); i++)
	{
		delete profiler[i];
	}
	profiler.clear();

	// Save Configuration to File
	SaveConfig(JOPE_DATA_DIRECTORY JOPE_CONFIG_FILENAME);

	return ret;
}

const char * Application::GetAppName() const
{
	return app_name.c_str();
}

//Get a new UUID
int Application::GetIntUUID()
{
	return UUIDGen.Int();
}

void Application::OpenBrowser(const char* url)
{
	ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWMAXIMIZED);
}

const std::list<Module*>* Application::GetModulesList()
{
	return &list_modules;
}

int Application::GetAvgFrameRate()
{
	return (int)avg_fps;
}

Uint32 Application::GetLastSecFrames()
{
	return last_sec_frame_count;
}

float Application::GetLastFrameTime()
{
	return 0.0f;
}

std::vector<float>* Application::GetMs()
{
	return &ms_counter;
}

std::vector<float>* Application::GetFPS()
{
	return &fps_counter;
}


void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}