#include "ModuleEditor.h"
#include "Application.h"

#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_sdl_gl3.h"
#include "Imgui\ImGuizmo.h"

#include "Glew\include\glew.h"

#include "Profiler.h"
#include "GameObject.h"
#include "Component.h"
#include "Importer.h"
#include "Resource.h"
#include "ResourceMesh.h"
#include "Mesh.h"

#include "Panel.h"
#include "PanelConfig.h"
#include "PanelAbout.h"
#include "PanelProfiler.h"

namespace fs = std::experimental::filesystem;

ModuleEditor::ModuleEditor(Application * app, bool start_enabled) : Module(app, "UI Editor", start_enabled)
{
}

ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Init()
{
	glewInit();
	ImGui_ImplSdlGL3_Init(App->window->window);
	
	return true;
}

bool ModuleEditor::Start()
{
	//Load all panels
	//Done here so the panels are able to get variables from the modules they need
	panel_array.push_back(new PanelConfig());
	panel_array.push_back(new PanelAbout());
	panel_array.push_back(new PanelProfiler());

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

	it_library_path = JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER;

	App->renderer3D->CheckConfig();

	return true;
}

update_status ModuleEditor::PreUpdate(float dt)
{
	ImGui_ImplSdlGL3_NewFrame(App->window->window);
	ImGuizmo::BeginFrame();

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update(float dt)
{
	
	//-------PLAY DRAW-------
	DrawPlayButton();

	if (App->clock.state == APP_PLAY) return UPDATE_CONTINUE;
	///--------------------------------------------------
	//-----MAIN MENU-----
	//Open a Gui window
	ImGui::BeginMainMenuBar();

	//Buttons to close the app and show the test window
	if (ImGui::BeginMenu("File"))
	{
		//Open Setting window
		if (ImGui::MenuItem("Configuration")) ChangePanelState("Configuration");
		//Save and Load Scene
		if (ImGui::MenuItem("Save scene ...")) savewindow = !savewindow;
		if (ImGui::MenuItem("Load scene ...")) loadwindow = !loadwindow;
		if (ImGui::MenuItem("Load Default Scene...")) App->scene_intro->LoadDefaultScene();
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

	//GameObject Menu
	if (ImGui::BeginMenu("GameObject"))
	{
		if (ImGui::MenuItem("Create Empty")) App->scene_intro->CreateNewGameObject("GameObject");
		ImGui::EndMenu();
	}

	//Tools Menu: Profiler
	if (ImGui::BeginMenu("Tools"))
	{
		if (ImGui::MenuItem("Profiler")) ChangePanelState("Profiler");
		ImGui::EndMenu();
	}

	//Help Menu
	if (ImGui::BeginMenu("Help"))
	{
		//Show about info 
		if (ImGui::MenuItem("About")) ChangePanelState("About");
		//Show ImGui demo
		if (ImGui::MenuItem("Show Demo")) showtestwindow = !showtestwindow;
		if (ImGui::MenuItem("Documentation(WIP)")) App->OpenBrowser("https://github.com/PatatesIDracs/3DGameEngine/wiki");
		if (ImGui::MenuItem("Download lastest")) App->OpenBrowser("https://github.com/PatatesIDracs/3DGameEngine/releases");
		if (ImGui::MenuItem("Report a bug")) App->OpenBrowser("https://github.com/PatatesIDracs/3DGameEngine/issues");
		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
	///--------------------------------------------------------------

	//Show the ImGui test window if requested
	if (showtestwindow) ImGui::ShowTestWindow();

	if (showassets || showconsole) {
		ImGui::SetNextWindowPos(ImVec2(250.f, (float)App->window->height - 230), 0);
		ImGui::SetNextWindowSize(ImVec2((float)App->window->width - 500, 30.f), 0);
		ImGui::Begin("Assets/Console Bar", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		bool change = showassets;
		if (change)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 1.00f, 0.00f, 1.00f));
		if (ImGui::Button("Assets", ImVec2(56.f, 18.f))) {
			showassets = !showassets;
			showconsole = !showconsole;
		}
		if (change)
			ImGui::PopStyleColor();
		ImGui::SameLine();
		change = showconsole;
		if (change)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 1.00f, 0.00f, 1.00f));
		if (ImGui::Button("Console", ImVec2(56.f, 18.f))) {
			showassets = !showassets;
			showconsole = !showconsole;
		}
		if (change)
			ImGui::PopStyleColor();

		if (showassets) on_assets = true;
		else on_assets = false;

		ImGui::End();
	}

	if (on_assets && showassets) DrawAssets();
	else if (showconsole) DrawConsole();

	//Show Game Object properties
	if (showpropertieswindow) DrawPropertiesWindow();
	if (showhierarchy) DrawHierarchy();

	//Save and load
	if (savewindow) DrawSaveWindow();
	if (loadwindow) DrawLoadWindow();
	
	for (uint i = 0; i < panel_array.size(); i++)
	{
		if (panel_array[i]->IsActive())
			panel_array[i]->Draw();
	}

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

	ImGuizmo::Enable(false);
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

void ModuleEditor::DrawSaveWindow()
{
	ImGui::OpenPopup("Save File");
	if (ImGui::BeginPopupModal("Save File", &savewindow))
	{
		static char file_name[200];
		ImGui::BeginChild("test", ImVec2(300,300),true);

		ImGui::EndChild();

		ImGui::InputText("", file_name, (int)sizeof(file_name));
		ImGui::SameLine();

		if (ImGui::Button("Save"))
		{
			App->SaveScene(file_name);
			savewindow = false;
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
			savewindow = false;

		ImGui::EndPopup();		
	}
}

void ModuleEditor::DrawLoadWindow()
{
	std::vector<Resource*>* to_show = &App->resources->resources_vec;

	ImGui::OpenPopup("Load File");
	if (ImGui::BeginPopupModal("Load File", &loadwindow))
	{
		static int file_uid = -1;
		static std::string file_name;
		ImGui::BeginChild("test", ImVec2(250, 300), true);
	
		for (uint i = 0; i < to_show->size(); i++)
		{
			if (ImGui::Button(("%s", (*to_show)[i]->GetAssetsPath())))
			{
				file_uid = (*to_show)[i]->GetUID();
				file_name = (*to_show)[i]->GetAssetsPath();
			};
		}

		ImGui::EndChild();

		ImGui::InputText("", (char*)file_name.c_str(), file_name.size(), ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();

		if (ImGui::Button("Load"))
		{
			if (file_uid != -1)
				App->LoadScene(file_uid);
			else
				LOGC("Please select a file to load");
			loadwindow = false;
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			loadwindow = false;
		}

		ImGui::EndPopup();
	}
}

void ModuleEditor::DrawHierarchy()
{
	ImGui::Begin("Game Object hierarchy", &showhierarchy);
	ImGui::SetWindowPos(ImVec2(0.f, 19.f), 0);
	ImGui::SetWindowSize(ImVec2((float)250, (float)App->window->height - 19), 0);

	App->scene_intro->DrawRootHierarchy();

	ImGui::End();
}

void ModuleEditor::DrawPlayButton()
{
	APPSTATE app_state = App->clock.state;

	ImGui::SetNextWindowPos(ImVec2((float)App->window->width*0.5f - 135.0f, 19.0f));
	ImGui::SetNextWindowSize(ImVec2(270.f, 30.0f));

	ImGui::Begin("Play Stop Tick", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	
		if (app_state == APP_PLAY || app_state == APP_PAUSE)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 1.00f, 0.00f, 1.00f));
		if (ImGui::Button("Play", ImVec2(56.f, 18.f))) {
			if (app_state != APP_PLAY && app_state != APP_PAUSE) {
				App->clock.ChangeState(APP_PLAY);
				App->camera->ChangeCamera(false);
				// Serialize Scene before first play;
				App->scene_intro->SaveToPlay();
			}
			else {
				App->clock.ChangeState(APP_STOP);
				App->camera->ChangeCamera(true);
				// Load Serialized Scene after play
				App->scene_intro->LoadToStop();
			}
		}
		if (app_state == APP_PLAY || app_state == APP_PAUSE)
			ImGui::PopStyleColor();
		
		if(app_state == APP_PAUSE)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 1.00f, 0.00f, 1.00f));
		ImGui::SameLine();
		if (ImGui::Button("Pause", ImVec2(56.f, 18.f))) {
			if (app_state == APP_PAUSE) {
				App->clock.state = APP_PLAY;
				App->camera->ChangeCamera(false);
			}
			else if(app_state == APP_PLAY) {
				App->clock.state = APP_PAUSE;
				App->camera->ChangeCamera(true);
			}
		}
		if (app_state == APP_PAUSE)
			ImGui::PopStyleColor();

		ImGui::SameLine();
		if (ImGui::Button("Tick", ImVec2(56.f, 18.f))) {
			App->clock.ChangeState(APP_TICK);
		}
		ImGui::SameLine();
		ImGui::Checkbox("Clock", &showclock);

	ImGui::End();

	if (showclock)
		DrawClock();
}

void ModuleEditor::DrawClock()
{
	if (App->clock.state == APP_PLAY)
		ImGui::SetNextWindowPos(ImVec2((float)App->window->width - 300.0f, (float)App->window->height - 150.0f));
	else ImGui::SetNextWindowPos(ImVec2((float)App->window->width - 550.0f, (float)App->window->height - 350.0f));

	ImGui::SetNextWindowSize(ImVec2(300.f, 150.0f));

	ImGui::Begin("Clock Window", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

	float time = (float)App->clock.real_time.Read() / 1000.f;
	ImGui::InputFloat("Time", &time, 0, 0, 2, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputInt("Frame", (int*)&App->clock.frame_count, 0, 0, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat("Game Time", &App->clock.game_time, 0, 0, 2, ImGuiInputTextFlags_ReadOnly);
	ImGui::SliderFloat("Time Scale", &App->clock.time_scale, 0.1f, 4.0f, "%.1f");
	ImGui::Separator();
	ImGui::InputFloat("dt", &App->clock.real_delta_time, 0, 0, 3, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat("Game dt", &App->clock.delta_time, 0, 0, 3, ImGuiInputTextFlags_ReadOnly);

	ImGui::End();
}

void ModuleEditor::DrawConsole()
{
	ImGui::Begin("Console", &showconsole, ImGuiWindowFlags_NoTitleBar);
	ImGui::SetWindowPos(ImVec2(250.f, (float)App->window->height - 200), 0);
	ImGui::SetWindowSize(ImVec2((float)App->window->width - 500, 200.f), 0);
	
	for (uint count = 0; count < console_string.size(); count++)
	{
		ImGui::Text(console_string[count].c_str());
	}
	
	ImGui::End();
}

// Did Assets Window in 1 HOUR!! Forgive us (for the code) xD
void ModuleEditor::DrawAssets()
{
	std::vector<Resource*>* to_show = &App->resources->all_resources_vec;

	ImGui::Begin("Assets", &showassets, ImGuiWindowFlags_NoTitleBar);
	ImGui::SetWindowPos(ImVec2(250.f, (float)App->window->height - 200), 0);
	ImGui::SetWindowSize(ImVec2((float)App->window->width - 500, 200.f), 0);

	static int file_uid = -1;
	static std::string file_name;
	ImGui::BeginChild("test", ImVec2(App->window->width - 550.f, 140), true);

	for (uint i = 0; i < to_show->size(); i++)
	{
		if (ImGui::Button(("%s", (*to_show)[i]->GetAssetsPath())))
		{
			file_uid = (*to_show)[i]->GetUID();
			file_name = (*to_show)[i]->GetAssetsPath();
		};
	}

	ImGui::EndChild();

	ImGui::InputText("", (char*)file_name.c_str(), file_name.size(), ImGuiInputTextFlags_ReadOnly);
	ImGui::SameLine();

	if (ImGui::Button("Load"))
	{
		if (file_uid != -1) {
			Resource* load_this = App->resources->GetFromUID(file_uid);
			RESOURCE_TYPE type = load_this->GetType();

			if(type == RESOURCE_SCENE)		
				App->LoadScene(file_uid);
			else if (type == RESOURCE_MESH) {
				GameObject* object = App->scene_intro->CreateNewGameObject((load_this->GetName()+std::to_string(App->resources->all_resources_vec.size())).c_str());
				Mesh* mesh = new Mesh(object, (ResourceMesh*)load_this);
				MeshRenderer* mesh_render = new MeshRenderer(object);
			}
		}
		else
			LOGC("Please select a file to load");
		loadwindow = false;
	}
	
	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
	{
		loadwindow = false;
	}
	ImGui::End();
}

void ModuleEditor::DrawPropertiesWindow()
{
	ImGui::Begin("Properties", &showpropertieswindow, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	ImGui::SetWindowPos(ImVec2((float)App->window->width - 250, 19.f), 0);
	ImGui::SetWindowSize(ImVec2(250.f, (float)App->window->height * 2 / 5), 0);

	App->scene_intro->DrawProperties();

	ImGui::End();
}

void ModuleEditor::ChangePanelState(const char * panel_name)
{
	for (uint i = 0; i < panel_array.size(); i++)
	{
		if (strcmp(panel_array[i]->GetName(),panel_name) == 0)
			panel_array[i]->ChangeState();
	}
}

bool ModuleEditor::DrawFixedExplorer(std::string& output, const char* path)
{
	bool ret = false;
	//Get to the library path
	fs::directory_iterator it{ path };
	fs::directory_iterator end{};
	
	ImGui::Begin("Explorer");
	ImGui::Text("Other files test");
	for (; it != end; it++)
	{
		if (fs::is_regular_file(it->path()))
		{
			static char file_name[150];
			std::wcstombs(file_name, it->path().filename().c_str(), sizeof(file_name));
			if (ImGui::Button(file_name))
			{
				char file[150];
				std::wcstombs(file, it->path().c_str(), 150);
				output = file;
				LOGC("Loaded file from: %s\\%S", it_library_path.c_str(), it->path().filename().c_str());
				it_library_path = JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER;
				ret = true;
				break;
			}
		}
	}
	ImGui::End();
	return ret;
}

bool ModuleEditor::DrawExplorer(std::string* output_file)
{
	bool ret = false;
	//Get to the library path
	fs::directory_iterator it{ it_library_path.c_str() };
	fs::directory_iterator end{};

	ImGui::Begin("test");
	ImGui::Columns(2);
	ImGui::Text("directory explorer test");


	ImGui::NextColumn();
	ImGui::Text("Other files test");
	for (; it != end; it++)
	{
		if (fs::is_directory(it->path()))
		{
			static char dir_name[150];
			std::wcstombs(dir_name, it->path().filename().c_str(), sizeof(dir_name));
			if (ImGui::Button(dir_name))
			{
				static char new_path[150];
				std::wcstombs(new_path, it->path().c_str(), 150);
				it_library_path = new_path;
			}
		}
		else
		{
			static char file_name[150];
			std::wcstombs(file_name, it->path().filename().c_str(), sizeof(file_name));
			if (ImGui::Button(file_name))
			{
				char file[150];
				std::wcstombs(file, it->path().c_str(), 150);
				//output = file;
				LOGC("Loaded file from: %s\\%S", it_library_path.c_str(), it->path().filename().c_str());
				it_library_path = JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER;
				ret = true;
				break;
			}
		}
	}
	ImGui::End();


	return ret;
}