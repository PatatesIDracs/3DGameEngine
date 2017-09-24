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

	// Load Gloval Configuration
	LoadConfig(CONFIG_FILENAME);

	//Test
	
	ms_counter.push_back(1);
	ms_counter.push_back(2);
	ms_counter.push_back(3);
	ms_counter.push_back(4);
	ms_counter.push_back(5);
	ms_counter.push_back(6);
	ms_counter.push_back(7);
	ms_counter.push_back(8);
	ms_counter.push_back(9);
	ms_counter.push_back(10);
	ms_counter.push_back(11);
	ms_counter.push_back(12);
	ms_counter.push_back(13);
	ms_counter.push_back(14);
	ms_counter.push_back(15);
	ms_counter.push_back(16);
	ms_counter.push_back(17);
	ms_counter.push_back(18);
	ms_counter.push_back(19);
	ms_counter.push_back(21);
	ms_counter.push_back(22);
	ms_counter.push_back(23);
	ms_counter.push_back(24);
	ms_counter.push_back(25);
	ms_counter.push_back(26);
	ms_counter.push_back(27);
	ms_counter.push_back(28);
	ms_counter.push_back(29);
	ms_counter.push_back(30);
	ms_counter.push_back(31);
	ms_counter.push_back(32);
	ms_counter.push_back(33);
	ms_counter.push_back(34);
	ms_counter.push_back(35);
	ms_counter.push_back(36);
	ms_counter.push_back(37);
	ms_counter.push_back(38);
	ms_counter.push_back(39);
	ms_counter.push_back(40);
	ms_counter.push_back(41);
	ms_counter.push_back(42);
	ms_counter.push_back(43);
	ms_counter.push_back(44);
	ms_counter.push_back(45);


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
void Application::LoadConfig(const char* filename)
{
	Config_Json config(filename);

	//tests
	int		test_int = config.GetInt("number", 0);
	float	test_float = config.GetFloat("number", 0.0f);
	std::string test_string = config.GetString("name", "Load String Failed");
	bool    test_bool = config.GetBool("isTrue", false);

	json_object_clear(config.config_obj);
	test_string.clear();
}

void Application::SaveConfig(const char* filename)
{
	Config_Json config(filename);

	//tests
	config.SetString("Save Result?", "One Point to Grifindor");
	config.SetBool("Realy?", true);
	config.SetFloat("WoW", 0.0);
	config.SetInt("can't believe it", 0);

	config.SaveToFile(filename);
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	total_frame_count++;
	curr_sec_frame_count++;

	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{

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
		for (int i = 0; i < fps_counter.size() - 1; i++)
		{
			fps_counter[i] = fps_counter[i + 1];
		}
		fps_counter.pop_back();
	}
	fps_counter.push_back(last_sec_frame_count);

	//List of the las 50 frames time
	if (ms_counter.size() > 50)
	{
		for (int i = 0; i < ms_counter.size() - 1; i++)
		{
			ms_counter[i] = ms_counter[i+1];
		}
		ms_counter.pop_back();
	}
	ms_counter.push_back(last_frame_time);





	//Frame limit TODO: read the capped_ms from config
	if (capped_ms > 0 && last_frame_time < capped_ms)
	{
		SDL_Delay(capped_ms - last_frame_time);
	}

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

	// Save Configuration to File
	SaveConfig(CONFIG_FILENAME);

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

int Application::GetAvgFrameRate()
{
	return avg_fps;
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