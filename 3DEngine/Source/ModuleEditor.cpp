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
#include "PanelHierarchy.h"
#include "PanelProperties.h"
#include "PanelPlayButton.h"
#include "PanelSaveScene.h"
#include "PanelLoadScene.h"
#include "PanelResourceExplorer.h"

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
	panel_array.push_back(new PanelHierarchy());
	panel_array.push_back(new PanelProperties());
	panel_array.push_back(new PanelPlayButton());
	panel_array.push_back(new PanelSaveScene());
	panel_array.push_back(new PanelLoadScene());
	panel_array.push_back(new PanelResourceExplorer());

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
	if (App->clock.state == APP_PLAY)
	{
		//TODO: Do it right
		for (uint i = 0; i < panel_array.size(); i++)
		{
			if (strcmp(panel_array[i]->GetName(), "PlayButton") == 0)
				panel_array[i]->Draw();
		}
		return UPDATE_CONTINUE;
	}
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
		if (ImGui::MenuItem("Save scene ...")) ChangePanelState("SaveScene");
		if (ImGui::MenuItem("Load scene ...")) ChangePanelState("LoadScene");
		if (ImGui::MenuItem("Load Default Scene...")) App->scene_intro->LoadDefaultScene();
		//Interrupt update and close the app
		if (ImGui::MenuItem("Exit")) return UPDATE_STOP;
		ImGui::EndMenu();
	}


	if (ImGui::BeginMenu("Workspace"))
	{
		if (ImGui::MenuItem("Properties")) ChangePanelState("Properties");
		if (ImGui::MenuItem("Hierarchy")) ChangePanelState("Hierarchy");
		if (ImGui::MenuItem("Console")) showconsole = !showconsole;
		ImGui::EndMenu();
	}

	//GameObject Menu
	if (ImGui::BeginMenu("GameObject"))
	{
		if (ImGui::MenuItem("Create Empty")) App->scene_intro->CreateNewGameObject("GameObject");
		if (ImGui::MenuItem("Create Plane")) App->scene_intro->CreateBasicGeometry(PRIM_PLANE);
		if (ImGui::MenuItem("Create Cube")) App->scene_intro->CreateBasicGeometry(PRIM_CUBE);
		if (ImGui::MenuItem("Create Sphere")) App->scene_intro->CreateBasicGeometry(PRIM_SPHERE);
		if (ImGui::MenuItem("Create Cylinder")) App->scene_intro->CreateBasicGeometry(PRIM_CYLINDER);
		ImGui::EndMenu();
	}

	//Tools Menu: Profiler
	if (ImGui::BeginMenu("Tools"))
	{
		if (ImGui::MenuItem("Profiler")) ChangePanelState("Profiler");
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Debug"))
	{
		if (ImGui::MenuItem("ResourceExplorer")) ChangePanelState("ResourceExplorer");
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
		ImGui::SetNextWindowPos(ImVec2(250.f, (float)App->window->height - 210), 0);
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

void ModuleEditor::DrawConsole()
{
	ImGui::Begin("Console", &showconsole, ImGuiWindowFlags_NoTitleBar| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	ImGui::SetWindowPos(ImVec2(250.f, (float)App->window->height - 180), 0);
	ImGui::SetWindowSize(ImVec2((float)App->window->width - 500, 180.f), 0);
	
	for (uint count = 0; count < console_string.size(); count++)
	{
		ImGui::Text(console_string[count].c_str());
	}
	
	ImGui::End();
}

void ModuleEditor::DrawAssets()
{
	std::vector<Resource*>* to_show = &App->resources->all_resources_vec;

	ImGui::Begin("Assets", &showassets, ImGuiWindowFlags_NoTitleBar| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	ImGui::SetWindowPos(ImVec2(250.f, (float)App->window->height - 180), 0);
	ImGui::SetWindowSize(ImVec2((float)App->window->width - 500, 180), 0);

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
			else if (type == RESOURCE_MESH) 
			{
				GameObject* object = App->scene_intro->CreateNewGameObject((load_this->GetName()+std::to_string(App->resources->all_resources_vec.size())).c_str());
				Mesh* mesh = new Mesh(object, (ResourceMesh*)load_this);
				MeshRenderer* mesh_render = new MeshRenderer(object);
			}
			else if (type == RESOURCE_PREFAB)
			{
				load_this->LoadResource();
			}
		}
		else
			LOGC("Please select a file to load");
	}
	
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

Panel * ModuleEditor::GetUIPanel(const char* panel_name) const
{
	for (uint i = 0; i < panel_array.size(); i++)
	{
		if (strcmp(panel_array[i]->GetName(), panel_name) == 0)
			return panel_array[i];
	}
	return nullptr;
}

Resource* ModuleEditor::DrawResourceExplorer(RESOURCE_TYPE type, const char* folder)
{
	Resource* ret = nullptr;
	int icon_id = 0;

	std::vector<Resource*>* to_show = nullptr;
	switch (type)
	{
	case RESOURCE_MESH: to_show = &App->resources->mesh_vec;
		icon_id = App->resources->mesh_icon_id;
		break;
	case RESOURCE_TEXTURE: to_show = &App->resources->texture_vec;
		icon_id = App->resources->text_icon_id;
		break;
	case RESOURCE_SCENE: to_show = &App->resources->scene_vec;
		icon_id = App->resources->scene_icon_id;
		break;
	default: return nullptr;
	}

	ImGui::SetNextWindowSize(ImVec2(240.f, 385.f));
	ImGui::Begin(folder, 0, ImGuiWindowFlags_NoResize);
	ImGui::BeginChild("Files:", ImVec2(225.f, 350.f), true);

	int count = 0;
	for (uint i = 0; i < to_show->size(); i++)
	{
		count++;
		const char* file_name = (*to_show)[i]->GetAssetsPath();
		ImGui::BeginChild(file_name, ImVec2(90,90), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

		if(ImGui::IsWindowFocused())
			ret = (*to_show)[i];

		ImGui::SetCursorPosX(17);
		ImGui::Image((ImTextureID)(GLuint)icon_id, ImVec2(56.f, 63.f), ImVec2(0, 1), ImVec2(1, 0));
				
		ImGui::Text(("%.10s", file_name));
		
		ImGui::EndChild();
		if (ImGui::IsItemHovered()) 
			ImGui::SetTooltip(("%.s", file_name));

		if (count != 2) {
			ImGui::SameLine();
		}
		else count = 0;
	}

	ImGui::EndChild();
	ImGui::End();
	
	return ret;
}