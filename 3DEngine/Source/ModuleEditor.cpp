#include "ModuleEditor.h"
#include "Application.h"

#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_sdl_gl3.h"

#include "Glew\include\glew.h"

#include "Profiler.h"
#include "GameObject.h"
#include "Component.h"

ModuleEditor::ModuleEditor(Application * app, bool start_enabled) : Module(app, "UI Editor", start_enabled)
{
}

ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Start()
{
	
	glewInit();
	ImGui_ImplSdlGL3_Init(App->window->window);
	ImGuiIO& io = ImGui::GetIO();

	app_profiler = App->GetProfilerVect();

	LoadHardwareSoftwareInfo();

	showconfig = true;
	showconsole = true;
	showpropertieswindow = true;
	showhierarchy = true;

	// Define IMGUI Style
	ImGuiStyle * style = &ImGui::GetStyle();

	style->WindowRounding = 0.0f;
	style->ScrollbarSize = 12.0f;
	style->ScrollbarRounding = 6.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
	style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.25f, 1.00f, 0.00f, 0.90f);
	style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.25f, 1.00f, 0.00f, 0.80f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);


	return true;
}

update_status ModuleEditor::PreUpdate(float dt)
{
	ImGui_ImplSdlGL3_NewFrame(App->window->window);


	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update(float dt)
{
	
	///--------------------------------------------------
	//-----MAIN MENU-----
	//Open a Gui window
	ImGui::BeginMainMenuBar();

	//Buttons to close the app and show the test window
	if (ImGui::BeginMenu("File"))
	{
		//Open Setting window
		if (ImGui::MenuItem("Configuration")) showconfig = !showconfig;

		//Interrupt update and close the app
		if (ImGui::MenuItem("Exit")) return UPDATE_STOP;
		ImGui::EndMenu();
	}


	if (ImGui::BeginMenu("Workspace"))
	{
		if (ImGui::MenuItem("Properties")) showpropertieswindow = !showpropertieswindow;
		if (ImGui::MenuItem("Hierachy")) showhierarchy = !showhierarchy;
		if (ImGui::MenuItem("Console")) showconsole = !showconsole;

		ImGui::EndMenu();
	}

	//Tools Menu: Profiler
	if (ImGui::BeginMenu("Tools"))
	{
		if (ImGui::MenuItem("Profiler")) showprofiler = !showprofiler;
	

		ImGui::EndMenu();
	}

	//Help Menu
	if (ImGui::BeginMenu("Help"))
	{
		//Show about info 
		if (ImGui::MenuItem("About")) showaboutwindow = !showaboutwindow;

		//Show ImGui demo
		if (ImGui::MenuItem("Show Demo")) showtestwindow = !showtestwindow;

		if (ImGui::MenuItem("Documentation(WIP)")) App->OpenBrowser("https://github.com/PatatesIDracs/3DGameEngine/wiki");

		if (ImGui::MenuItem("Download lastest")) App->OpenBrowser("https://github.com/PatatesIDracs/3DGameEngine/releases");

		if (ImGui::MenuItem("Report a bug")) App->OpenBrowser("https://github.com/PatatesIDracs/3DGameEngine/issues");
		
		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
	///--------------------------------------------------------------

	//------CONFIG DRAW------
	//Get a const module list from app
	//We'll do it regardless of the showconfig value since we may need the list for other Draw methods
	const std::list<Module*>* module_list = App->GetModulesList();
	std::list<Module*>::const_iterator item = module_list->begin();

	//Check if we need to draw the UI
	if (showconfig)
	{
		ImGui::Begin("Configuration", &showconfig, ImGuiWindowFlags_NoMove);
		ImGui::SetWindowPos(ImVec2(App->window->width - 250, App->window->height * 2 / 5 + 19), 0);
		ImGui::SetWindowSize(ImVec2(250, App->window->height * 3 / 5 - 19), 0);

		ApplicationConfig();

		while (item != module_list->end())
		{
			if (ImGui::CollapsingHeader(item._Ptr->_Myval->GetName()))
				item._Ptr->_Myval->DrawConfig();
	
			item++;
		}

		HardwareDetection();

		ImGui::End();
		
	}

	//Show the ImGui test window if requested
	if (showtestwindow) ImGui::ShowTestWindow();

	//Draw about window when requested
	if (showaboutwindow) DrawAboutWindow();

	if (showconsole) DrawConsole();

	//Draw Profiler
	if (showprofiler) DrawProfilerWindow();

	//Show Game Object properties
	if (showpropertieswindow) DrawPropertiesWindow();

	if (showhierarchy) DrawHierarchy();
	
	return UPDATE_CONTINUE;
}


//Except from the main menu the other UI elements of ModuleEditor will be put here
void ModuleEditor::Draw()
{

	//Make sure that the UI is draw in fill mode not Wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Render all UI
	ImGui::Render();

}



bool ModuleEditor::CleanUp()
{
	LOGC("Cleaning Module Editor");

	ImGui_ImplSdlGL3_Shutdown();

	return true;
}

void ModuleEditor::LogToConsole(std::string * log_string)
{
	if (console_string.size() >= CONSOLE_LIMIT)
	{
		for (uint count = 0; count < console_string.size() - 1; count++)
		{
			console_string[count] = console_string[count + 1];
		}
		console_string.pop_back();
	}
	console_string.push_back(*log_string);
}

//Small function just to make the code cleaner
void ModuleEditor::LoadHardwareSoftwareInfo()
{
//	devil_version = App->assimp->GetDevilVersion();

	cpu_cores = SDL_GetCPUCount();
	cpu_cache_size = SDL_GetCPUCacheLineSize();
	ram = ((float)SDL_GetSystemRAM() / 1024.0f);
	has_3Dnow = SDL_Has3DNow();
	has_AVX = SDL_HasAVX();
	has_AVX2 = SDL_HasAVX2();
	has_AltiVec = SDL_HasAltiVec();
	has_MMX = SDL_HasMMX();
	has_RDTSC = SDL_HasRDTSC();
	has_SSE = SDL_HasSSE();
	has_SSE2 = SDL_HasSSE2();
	has_SSE3 = SDL_HasSSE3();
	has_SSE41 = SDL_HasSSE41();
	has_SSE42 = SDL_HasSSE42();
}


//About Window 
void ModuleEditor::DrawAboutWindow()
{

	ImGui::Begin("About JoPe", &showaboutwindow, ImGuiWindowFlags_NoResize);

	ImGui::Text("JoPe Engine");
	ImGui::Text("Very simple and limited game engine,\nmade for educational purposes");

	ImGui::Text("\nMade by: Joan Pareja, Pere Rifa");

	//Libraries: bullet, SDL, glew, glut, dear imgui, json(Parson), MathGeoLib
	ImGui::Text("\nMade with: SDL, glew, dear imgui, Parson, MathGeoLib");

	ImGui::Text("\nLicensed under the MIT license");

	ImGui::End();
}

void ModuleEditor::DrawHierarchy()
{
	ImGui::Begin("Game Object hierarchy", &showhierarchy);
	ImGui::SetWindowPos(ImVec2(0, 19), 0);
	ImGui::SetWindowSize(ImVec2(250, App->window->height - 19), 0);

	App->scene_intro->root->DrawHierarchy();	

	ImGui::End();
}

void ModuleEditor::ApplicationConfig()
{
	if (ImGui::CollapsingHeader("Application"))
	{
		if (ImGui::InputText("App name", str_name, (int)sizeof(str_name)))
			App->window->SetTitle(str_name);

		static char str_org[150] = "Patates i Dracs";
		ImGui::InputText("Organitzation", str_org, (int)sizeof(str_org));

		if (ImGui::Checkbox("Unlimited", &uncapped_fps)) App->SetFpsCap(uncapped_fps);
		if (ImGui::SliderInt("Max FPS", &App->fps, 15, 120)) App->SetFpsCap(uncapped_fps);


		ImGui::PlotHistogram("Framerate", &App->GetFPS()->front(), App->GetFPS()->size(), 0, NULL, 0.0f, 120.0f, ImVec2(0, 80));
		ImGui::PlotHistogram("Miliseconds", &App->GetMs()->front(), App->GetMs()->size(), 0, NULL, 0.0f, 50.0f, ImVec2(0, 80));

	}
}

void ModuleEditor::HardwareDetection()
{

	if (ImGui::CollapsingHeader("Hardware"))
	{
		ImVec4	cyan(0.0f, 1.0f, 1.0f, 1.0f);

		//----------------------
		//CPU info--------------
		ImGui::Text("CPU: "); ImGui::SameLine();
		ImGui::TextColored(cyan, "%i cores", SDL_GetCPUCount());  ImGui::SameLine();
		ImGui::TextColored(cyan, "(cache: %i bytes)", SDL_GetCPUCacheLineSize());

		ImGui::Text("System RAM: "); ImGui::SameLine();
		ImGui::TextColored(cyan, "%0.1f Gb", ((float)SDL_GetSystemRAM() / 1024.0f));

		ImGui::Text("Caps: "); ImGui::SameLine();
		if (SDL_Has3DNow() == SDL_TRUE)		ImGui::TextColored(cyan, "3DNow!, ");	ImGui::SameLine();
		if (SDL_HasAVX() == SDL_TRUE)		ImGui::TextColored(cyan, "AVX, ");		ImGui::SameLine();
	//	if (SDL_HasAVX2() == SDL_TRUE)		ImGui::TextColored(cyan, "AVX2, ");		ImGui::SameLine();
		if (SDL_HasAltiVec() == SDL_TRUE)	ImGui::TextColored(cyan, "AltiVec, ");	ImGui::SameLine();
		if (SDL_HasMMX() == SDL_TRUE)		ImGui::TextColored(cyan, "MMX, ");		ImGui::SameLine();
		if (SDL_HasRDTSC() == SDL_TRUE)		ImGui::TextColored(cyan, "RDTSC, ");	ImGui::SameLine();
		if (SDL_HasSSE() == SDL_TRUE)		ImGui::TextColored(cyan, "SSD, ");		ImGui::SameLine();
		if (SDL_HasSSE2() == SDL_TRUE)		ImGui::TextColored(cyan, "SSD2, ");		ImGui::SameLine();
		if (SDL_HasSSE3() == SDL_TRUE)		ImGui::TextColored(cyan, "SSD3, ");		ImGui::SameLine();
		if (SDL_HasSSE41() == SDL_TRUE)		ImGui::TextColored(cyan, "SSD41, ");	ImGui::SameLine();
		if (SDL_HasSSE42() == SDL_TRUE)		ImGui::TextColored(cyan, "SSD42, ");


		ImGui::Separator();
		//---------------------
		//GPU info-------------
		ImGui::Text("Vendor: "); ImGui::SameLine();
		ImGui::Text((const char*)glGetString(GL_VENDOR));

		ImGui::Text("Brand: "); ImGui::SameLine();
		ImGui::Text((const char*)glGetString(GL_RENDERER));

		ImGui::Text("OpenGL Supported: "); ImGui::SameLine();
		ImGui::Text((const char*)glGetString(GL_VERSION));
	}
	
}

void ModuleEditor::DrawConsole()
{
	ImGui::Begin("Console", &showconsole);
	ImGui::SetWindowPos(ImVec2(250, App->window->height - 200), 0);
	ImGui::SetWindowSize(ImVec2(App->window->width - 500, 200), 0);
	
	for (uint count = 0; count < console_string.size(); count++)
	{
		ImGui::Text(console_string[count].c_str());
	}
	
	ImGui::End();
}

void ModuleEditor::DrawPropertiesWindow()
{
	ImGui::Begin("Properties", &showpropertieswindow, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	ImGui::SetWindowPos(ImVec2(App->window->width - 250, 19), 0);
	ImGui::SetWindowSize(ImVec2(250, App->window->height * 2 / 5), 0);

	App->scene_intro->DrawProperties();

	ImGui::End();
}

// Profiler Interface ==============================
void ModuleEditor::DrawProfilerWindow()
{
	ImGui::Begin("Profiler Test", &showprofiler, ImGuiWindowFlags_NoResize);
	{
		if (ImGui::Button("Start Profiler")) {
			if (recordpaused)
			{
				recordpaused = false;
				App->DoRecord();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Pause Profiler"))
		{
			if (!recordpaused) App->DoRecord();
			recordpaused = true;
			
		}
		
		//Show Ms from functions Real Time
		ImGui::Separator();
		
			ImGui::Text("Modules: ");
			const std::list<Module*>* app = App->GetModulesList();
			std::list<Module*>::const_iterator app_modules = app->begin();
			
			int count = 0;
			for (; app_modules != app->end(); app_modules++)
			{
				ImGui::RadioButton(app_modules._Ptr->_Myval->GetName(), (int*)&current_module, count);
				count++;
			}
			ImGui::Separator();

			std::vector<char*>* fnames = app_profiler->begin()[current_module]->GetFunctionNames();
			std::vector<float>* item = nullptr;

			for (uint i = 0; i < fnames->size(); i++)
			{
				item = app_profiler->begin()[current_module]->GetFunctionTimeline(fnames->at(i));
				ImGui::PlotHistogram(fnames->at(i), &item->at(0), 60, 0, NULL, 0.0f, 5.0f, ImVec2(0, 80));
			}	
		
	}
	ImGui::End();

	// When Closing DrawProfilerWindow Stops Profiler
	if (!showprofiler && !recordpaused)
	{
		recordpaused = !recordpaused;
		App->DoRecord();
	}

}

