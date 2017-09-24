#include "ModuleEditor.h"
#include "Application.h"

#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_sdl_gl3.h"

#include "Glew\include\glew.h"

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
	//-------------------
	//-----MAIN MENU-----
	//-------------------
	//Since main menu may have a major impact on the application it's managed directly from the Update

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

	///--------------------------------------------------------------
	//-----------------------
	//------CONFIG DRAW------
	//-----------------------

	//Get a const module list from app
	//We'll do it regardless of the showconfig value since we may need the list for other Draw methods
	const std::list<Module*>* module_list = App->GetModulesList();
	std::list<Module*>::const_iterator item = module_list->begin();

	//Check if we need to draw the UI
	if (showconfig)
	{
		ImGui::Begin("Configuration");
		
		ApplicationConfig();

		while (item != module_list->end())
		{
			item._Ptr->_Myval->DrawConfig();
			item++;
		}
		
		HardwareDetection();

		ImGui::End();
	}
	///--------------------------------------------------------------

	///--------------------------------------------------------------
	//-----------------------
	//----MODULES UI DRAW----
	//-----------------------
	
	//We'll reuse the module_list and item from the configdraw
	//Make sure that the item is the start of the list
	item = module_list->begin();
		
	//Iterate the list and call the DrawImGui from all modules
	//ImGui::Begin() and ImGui::End() needs to be called in the DragImGui of each module
	//This way we can decide more easily the name of the new UI window
	while (item != module_list->end())
	{

		item._Ptr->_Myval->DrawImGui();
		item++;
	}	
	///--------------------------------------------------------------
	

	//Render all UI
	ImGui::Render();
	
	return UPDATE_CONTINUE;
}


//Except from the main menu the other UI elements of ModuleEditor will be put here
void ModuleEditor::DrawImGui()
{
	//Show the ImGui test window if requested
	if (showtestwindow) ImGui::ShowTestWindow();

	//Draw about window when requested
	if (showaboutwindow) DrawAboutWindow();
}



bool ModuleEditor::CleanUp()
{
	LOGC("Cleaning Module Editor");

	ImGui_ImplSdlGL3_Shutdown();

	return true;
}


//About Window 
void ModuleEditor::DrawAboutWindow()
{

	ImGui::Begin("About JoPe");

	ImGui::Text("JoPe Engine");
	ImGui::Text("Very simple and limited game engine,\nmade for educational purposes");

	ImGui::Text("\nMade by: Joan Pareja, Pere Rifa");

	//Libraries: bullet, SDL, glew, glut, dear imgui, json(Parson), MathGeoLib
	ImGui::Text("\nMade with: SDL, glew, glut, dear imgui, Parson, MathGeoLib");

	ImGui::Text("\nLicensed under the MIT license");

	ImGui::End();
}

void ModuleEditor::ApplicationConfig()
{
	if (ImGui::CollapsingHeader("Application"))
	{
		static char str_name[150] = "JoPe Engine";
		ImGui::InputText("App name", str_name, (int)sizeof(str_name));

		static char str_org[150] = "Patates i Dracs";
		ImGui::InputText("Organitzation", str_org, (int)sizeof(str_org));

		static int fps = 0;
		ImGui::SliderInt("Max FPS", &fps, 15, 120);


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
		//NOT DONE
		ImGui::Text((char*)glGetString(GL_VERSION));
	}
	
}

