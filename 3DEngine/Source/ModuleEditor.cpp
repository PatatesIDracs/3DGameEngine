#include "ModuleEditor.h"
#include "Application.h"

#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_sdl_gl3.h"

#include "Glew\include\glew.h"

ModuleEditor::ModuleEditor(Application * app, bool start_enabled) : Module(app, start_enabled)
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
	
	///------------------
	//-------------------
	//-----MAIN MENU-----
	//-------------------
	//Since main menu may have a major impact on the application it's managed directly from the Update

	//Open a Gui window
	ImGui::BeginMainMenuBar();

	//Buttons to close the app and show the test window
	if (ImGui::BeginMenu("File"))
	{
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


		//temp
		if (ImGui::MenuItem("hardware")) showhardware = !showhardware;


		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
	///--------------------------------------------------------------

	///--------------------------------------------------------------
	//-----------------------
	//----MODULES UI DRAW----
	//-----------------------
	
	//Get a const module list from app
	const std::list<Module*>* module_list = App->GetModulesList();
	std::list<Module*>::const_iterator item = module_list->begin();
		
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


//Except from the main menu the other Ui elements of ModuleEditor will be put here
void ModuleEditor::DrawImGui()
{
	//Show the ImGui test window if requested
	if (showtestwindow) ImGui::ShowTestWindow();

	//Draw about window when requested
	if (showaboutwindow) DrawAboutWindow();

	if (showhardware) HardwareDetection();

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

void ModuleEditor::HardwareDetection()
{

	ImGui::Begin("Hardware");

	ImGui::Text("CPU: "); ImGui::SameLine(); 
	ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%i cores", SDL_GetCPUCount());  ImGui::SameLine();
 	ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "(cache: %i bytes)", SDL_GetCPUCacheLineSize());

	ImGui::Text("System RAM: "); ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%0.1f Gb", ((float)SDL_GetSystemRAM()/1024.0f));

	ImGui::Separator();

	ImGui::Text((char*) glGetString(GL_VERSION));
	
	ImGui::End();
}
