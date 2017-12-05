#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "Mesh.h"

#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_sdl_gl3.h"

#include "Material.h"
#include "MeshRenderer.h"
#include "Camera.h"

GameObject::GameObject(GameObject* parent, bool isactive) : parent(parent), name("GameObject"), isactive(isactive)
{
	
	UUID = App->GetIntUUID();
	parent_active = true;
	if (parent != nullptr)
		parent->AddChildren(this);

	// Create default Transform
	components.push_back(new Transform(this));

	boundary_box.SetNegativeInfinity();
}

GameObject::GameObject(GameObject * parent,const char * name, bool isactive) : parent(parent), name(name), isactive(isactive)
{
	UUID = App->GetIntUUID();
	parent_active = true;
	if (parent != nullptr)
		parent->AddChildren(this);

	// Create default Transform, this way we know the first component will always be the transform
	components.push_back(new Transform(this));

	boundary_box.SetNegativeInfinity();
}

GameObject::~GameObject()
{
	if (parent != nullptr)
		parent->RemoveChildren(this);

	for (uint i = 0; i < children.size(); i++)
	{
		delete	children[i];
	}
	children.clear();

	for (uint i = 0; i < components.size(); i++)
	{
		delete components[i];
	}
	components.clear();		
}

void GameObject::Update()
{
	if (parent != nullptr) parent_active = parent->IsActive();

	if (!IsActive()) return;

	//Check if the GameObject has a gameobject list
	if (children.size() != 0)
	{
		for (uint i = 0; i < children.size(); i++)
		{
			children[i]->Update();
		}
	}
	
	//for (uint i = (int)isstatic; i < components.size(); i++)
	for (uint i = 0; i < components.size(); i++)
	{
		components[i]->Update();
	}
}

void GameObject::AddToScene() const
{
	App->scene_intro->AddGameObjectToDynamic(this);

	for (uint i = 0; i < children.size(); i++)
		children[i]->AddToScene();
}

//Set methods
void GameObject::AddChildren(GameObject * new_child)
{
	children.push_back(new_child);
	new_child->parent_UUID = UUID;
	new_child->parent = this;
}

void GameObject::AddComponent(Component * new_component, bool overwrite)
{
	//If the new component is unique check if we have a component of the same type
	if (new_component->IsUnique())
	{
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->GetType() == new_component->GetType())
			{
				if (overwrite)
				{
					delete components[i];
					components[i] = new_component;
					new_component->ChangeParent(this);
				}
				else
					LOGC("%s already has this component", name.c_str());
				return;
			}
		}
	}

	components.push_back(new_component);
	new_component->ChangeParent(this);
}

void GameObject::SetTransform(float4x4 &transform)
{
	((Transform*)components[0])->SetTransform(transform);
}

void GameObject::RemoveChildren(GameObject * remove_child)
{
	for (uint i = 0; i < children.size(); i++) {
		if (children[i] == remove_child) {
			for (uint j = i; j < children.size() - 1; j++) {
				children[j] = children[j + 1];
			}
			children.pop_back();
		}
	}
}


//Get/Check state methods
bool GameObject::IsActive() const
{
	return isactive && parent_active;
}

bool GameObject::IsRoot() const
{
	return (parent == NULL ? true : false);
}

Transform * GameObject::GetTransform() const
{
	return (Transform*)components[0];
}

AABB GameObject::GetBoundaryBox() const
{
	return boundary_box;
}

AABB GameObject::GetGlobalBoundaryBox() const
{
	math::AABB box = boundary_box;

	for (uint i = 0; i < children.size(); i++)
	{
		box.Enclose(children[i]->GetGlobalBoundaryBox());
	}

	return box;
}

Component* GameObject::FindFirstComponent(COMP_TYPE type)
{
	Component* ret = nullptr;

	for (uint i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() == type)
		{
			ret = components[i];
			break;
		}
	}

	return ret;
}

std::vector<Component*>* GameObject::FindAllTypeComponents(COMP_TYPE type)
{
	std::vector<Component*>* ret = new std::vector<Component*>;

	for (uint i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() == type)
		{
			ret->push_back(components[i]);
		}
	}

	return ret;
}

GameObject* GameObject::DrawHierarchy()
{
	GameObject* ret = nullptr;

	ImGuiTreeNodeFlags game_object_node_flags;

	//Set which flags will be used (Has children or not)
	if (children.size() != 0)
		game_object_node_flags = 0;
	else
		game_object_node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	ImGui::PushID(UUID);
	//Draw the node
	if (ImGui::TreeNodeEx(name.c_str(), game_object_node_flags))
	{
		if (ImGui::IsItemClicked()) ret = this;
		
		for (uint i = 0; i < children.size(); i++)
		{
			GameObject* child_ret =	children[i]->DrawHierarchy();
		
			if (child_ret != nullptr) ret = child_ret;
		}

		if (children.size() != 0)
			ImGui::TreePop();
	}
	ImGui::PopID();
	
	// Return GameObject Selected
	return ret;
}

void GameObject::DrawProperties()
{
	// Draw GameObject Properties
	DrawGameObject();
	boundary_box.Draw(3.0f, float4(1.0f, 0.0f, 0.0f, 1.0f));

	// Draw Components Properties
	for (uint i = 0; i < components.size(); i++)
	{
		components[i]->DrawComponent();
	}

	if (ImGui::Button("AddComponent")) creating_component = !creating_component;

	if (creating_component) DrawAddComponentWindow();
}

void GameObject::DrawGameObject()
{
	ImGui::Checkbox("Active", &isactive);
	ImGui::SameLine();
	ImGui::InputText("", (char*)name.c_str(), name.size(), ImGuiInputTextFlags_ReadOnly);
	if (ImGui::Checkbox("Static", &isstatic)) {
		if(children.size() > 0) change_static = true;
		else {
			if (isstatic) App->scene_intro->CheckDynamicGameObjectsState();
			else App->scene_intro->CheckStaticGameObjectsState();
		}
	}

	if (change_static) DrawSetStaticGObjWindow();
}


void GameObject::Save(const char* buffer_data, char* cursor, int& bytes_copied)
{
	LOGC("Saving %s", name.c_str());

	int identifier = GAMEOBJECTIDENTIFIER;
	uint bytes_to_copy = sizeof(identifier);
	memcpy(cursor, &identifier, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//UUID and parent UUID
	bytes_to_copy = sizeof(UUID);
	memcpy(cursor, &UUID, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	bytes_to_copy = sizeof(parent_UUID);
	memcpy(cursor, &parent_UUID, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//name
	bytes_to_copy = sizeof(int);
	int name_size = name.size();
	memcpy(cursor, &name_size, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	bytes_to_copy = name.size();
	memcpy(cursor, name.c_str(), bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//isactive + isstatic
	bytes_to_copy = sizeof(isactive);
	memcpy(cursor, &isactive, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	bytes_to_copy = sizeof(isstatic);
	memcpy(cursor, &isstatic, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//Save components
	for (uint i = 0; i < components.size(); i++)
	{
		int bytes_advanced = 0;
		components[i]->Save(buffer_data, cursor, bytes_advanced);
		cursor += bytes_advanced;
		bytes_copied += bytes_advanced;
	}
	//Save child objects
	for (uint i = 0; i < children.size(); i++)
	{
		int bytes_advanced = 0;
		children[i]->Save(buffer_data, cursor, bytes_advanced);
		cursor += bytes_advanced;
		bytes_copied += bytes_advanced;
	}
}

void GameObject::Load(char* cursor, int& bytes_copied)
{
	uint bytes_to_copy = sizeof(int);
	memcpy(&UUID, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&parent_UUID, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//copy name
	int name_size = 0;
	memcpy(&name_size, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	char* name_c_str = new char[name_size+1];
	bytes_to_copy = name_size;
	memcpy(name_c_str, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	name_c_str[name_size] = 0x00;
	name = name_c_str;
	delete[] name_c_str;

	bytes_to_copy = sizeof(bool);
	memcpy(&isactive, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&isstatic, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	LOGC("%s game object loaded", name.c_str());
}

void GameObject::GetOwnBufferSize(uint& buffer_size)
{
	buffer_size += sizeof(int);			//identifier
	buffer_size += sizeof(UUID);
	buffer_size += sizeof(parent_UUID);
	buffer_size += sizeof(int);			//Save name size
	buffer_size += name.size();
	buffer_size += sizeof(isactive);
	buffer_size += sizeof(isstatic);

	for (uint i = 0; i < children.size(); i++)
	{
		children[i]->GetOwnBufferSize(buffer_size);
	}

	for (uint i = 0; i < components.size(); i++)
	{
		components[i]->GetOwnBufferSize(buffer_size);
	}
}

void GameObject::ReloadUUID()
{
	UUID = App->GetIntUUID();

	if (parent != nullptr)
		parent_UUID = parent->UUID;
	else
		parent_UUID = 0;

	for (uint i = 0; i < children.size(); i++)
	{
		children[i]->ReloadUUID();
	}
	for (uint i = 0; i < components.size(); i++)
	{
		components[i]->ReloadUUID();
	}
}

int GameObject::GetparentUUID() const
{
	return parent_UUID;
}

int GameObject::GetUUID() const
{
	return UUID;
}



void GameObject::DrawAddComponentWindow()
{
	COMP_TYPE new_comp_type = COMP_TYPE::COMP_UNKNOWN;

	ImGui::Begin("Add Component", &creating_component);

	if (ImGui::Button("Transform")) new_comp_type = COMP_TYPE::COMP_TRANSFORM;
	if (ImGui::Button("Mesh")) new_comp_type = COMP_TYPE::COMP_MESH;
	if (ImGui::Button("Material")) new_comp_type = COMP_TYPE::COMP_MATERIAL;
	if (ImGui::Button("Mesh Renderer")) new_comp_type = COMP_TYPE::COMP_MESHRENDERER;
	if (ImGui::Button("Camera")) new_comp_type = COMP_TYPE::COMP_CAMERA;

	ImGui::End();

	if (new_comp_type != COMP_TYPE::COMP_UNKNOWN)
	{
		AddComponent(App->scene_intro->NewOrphanComponent(new_comp_type));
		creating_component = false;
	}
}

void GameObject::DrawSetStaticGObjWindow()
{
	ImGui::Begin("Game Object changed static state", &change_static, ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse);
	
	if (isstatic) {
		if (ImGui::Button("Set childs as static")) {
			ChangeChildsStatic(isstatic);
			App->scene_intro->CheckDynamicGameObjectsState();
			change_static = false;
		}
	}
	else {
		if (ImGui::Button("Set childs as dynamic")) {
			ChangeChildsStatic(isstatic);
			App->scene_intro->CheckStaticGameObjectsState();
			change_static = false;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		if (isstatic) App->scene_intro->CheckDynamicGameObjectsState();
		else App->scene_intro->CheckStaticGameObjectsState();
		change_static = false;
	}
	ImGui::End();
}

void GameObject::ChangeChildsStatic(bool state)
{
	isstatic = state;

	for (uint i = 0; i < children.size(); i++)
	{
		children[i]->ChangeChildsStatic(state);
	}
}
