#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "Mesh.h"
#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_sdl_gl3.h"

GameObject::GameObject(GameObject* parent, bool isactive) : parent(parent), name("GameObject"), isactive(isactive)
{
	LCG UUIDGen;
	UUID = UUIDGen.Int();
	parent_active = true;
	if (parent != nullptr)
		parent->AddChildren(this);

	// Create default Transform
	components.push_back(new Transform(this));

	boundary_box.SetNegativeInfinity();
}

GameObject::GameObject(GameObject * parent,const char * name, bool isactive) : parent(parent), name(name), isactive(isactive)
{
	LCG UUIDGen;
	UUID = UUIDGen.Int();
	parent_active = true;
	if (parent != nullptr)
		parent->AddChildren(this);

	// Create default Transform, this way we know the first component will always be the transform
	components.push_back(new Transform(this));

	boundary_box.SetNegativeInfinity();
}

GameObject::~GameObject()
{
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
	
	for (uint i = (int)isstatic; i < components.size(); i++)
	{
		components[i]->Update();
	}
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

AABB GameObject::GetBoundaryBox()
{
	math::AABB box;
	box.SetNegativeInfinity();

	for (uint i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() == COMP_MESH)
			box.Enclose(((Mesh*)components[i])->aabb_box);
	}

	for (uint i = 0; i < children.size(); i++)
	{
		box.Enclose(children[i]->GetBoundaryBox());
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
	
	// Return GameObject Selected
	return ret;
}

void GameObject::DrawProperties()
{
	// Draw GameObject Properties
	DrawGameObject();

	// Draw Components Properties
	for (uint i = 0; i < components.size(); i++)
	{
		components[i]->DrawComponent();
	}
}

void GameObject::DrawGameObject()
{
	ImGui::Checkbox("Active", &isactive);
	ImGui::SameLine();
	ImGui::InputText("", (char*)name.c_str(), name.size(), ImGuiInputTextFlags_ReadOnly);
	ImGui::Checkbox("Static", &isstatic);
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

void GameObject::Load(const char * buffer_data, char* cursor, int& bytes_copied)
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
