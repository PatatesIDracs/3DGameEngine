#include "Application.h"

#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_sdl_gl3.h"

#include "Glew\include\glew.h"

#include "parson.h"
#include "ConfigJSON.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	scene_intro = new ModuleSceneIntro(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	physics = new ModulePhysics3D(this);
	editor = new ModuleEditor(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);
	AddModule(physics);

	// Scenes
	AddModule(scene_intro);

	// Renderer last!
	AddModule(renderer3D);
	//Render UI last
	AddModule(editor);
}

Application::~Application()
{
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

	Config_Json config("../Game/config.json");
	LoadConfig(config);

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

	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	total_frame_count++;
	last_sec_frame_count++;

	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	///---------------------------
	//----------------------------
	//---FRAMERATE CALCULATIONS---
	//----------------------------

	//When a second has passed...
	if (last_sec_timer.Read() > 1000)
	{
		last_sec_timer.Start();							//Reset the timer
		last_sec_frame_count = curr_sec_frame_count;	//Pass the current sec frame count to the last sec frame count
		curr_sec_frame_count = 0;						//Reset frame count
	}

	avg_fps = float (total_frame_count) / start_up_time.ReadSec();
	last_frame_time = ms_timer.Read();
	///---------------------------
}	

// ---------------------------------------------
void Application::LoadConfig(Config_Json& config)
{
	
	
	int		test_int = config.GetInt("number", 0);
	float	test_float = config.GetFloat("number", 0.0f);
	std::string test_string = config.GetString("name", "Load String Failed");
	bool    test_bool = config.GetBool("isTrue", false);

	test_string.clear();
}

void Application::SaveConfig()
{
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	std::list<Module*>::iterator item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = item._Ptr->_Myval->PreUpdate(dt);
		item++;
	}

	item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = item._Ptr->_Myval->Update(dt);
		item++;
	}

	item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = item._Ptr->_Myval->PostUpdate(dt);
		item++;
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
	return ret;
}

void Application::OpenBrowser(const char* url)
{
	ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWMAXIMIZED);
}

const std::list<Module*>* Application::GetModulesList()
{
	return &list_modules;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}