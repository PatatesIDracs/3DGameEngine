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
		//Show ImGui demo
		if (ImGui::MenuItem("Show Demo")) showtestwindow = !showtestwindow;

		if (ImGui::MenuItem("Documentation(WIP)")) App->OpenBrowser("https://github.com/PatatesIDracs/3DGameEngine/wiki");

		if (ImGui::MenuItem("Download lastest")) App->OpenBrowser("https://github.com/PatatesIDracs/3DGameEngine/releases");

		if (ImGui::MenuItem("Report a bug")) App->OpenBrowser("https://github.com/PatatesIDracs/3DGameEngine/issues");

		ImGui::EndMenu();
	}

	//Show the ImGui test window fi requested
	if (showtestwindow) ImGui::ShowTestWindow();

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

update_status ModuleEditor::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleEditor::CleanUp()
{
	LOGC("Cleaning Module Editor");

	ImGui_ImplSdlGL3_Shutdown();

	return true;
}
