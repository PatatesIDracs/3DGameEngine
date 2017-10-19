#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "Mesh.h"
#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_sdl_gl3.h"

GameObject::GameObject(GameObject* parent) : parent(parent)
{
	if (parent != nullptr)
		parent->AddChildren(this);
}

GameObject::GameObject(GameObject * parent,const char * name) : parent(parent), name(name)
{
	if (parent != nullptr)
		parent->AddChildren(this);
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
	//Check if the GameObject has a gameobject list
	if (children.size() != 0)
	{
		for (uint i = 0; i < children.size(); i++)
		{
			children[i]->Update();
		}
	}
	
	for (uint i = 0; i < components.size(); i++)
	{
		components[i]->Update();
	}
}

void GameObject::AddChildren(GameObject * new_child)
{
	children.push_back(new_child);
}

void GameObject::AddComponent(Component * new_component)
{
	components.push_back(new_component);
}

Component* GameObject::FindUniqueComponent(COMP_TYPE type)
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
	for (uint i = 0; i < components.size(); i++)
	{
		components[i]->DrawComponent();
	}
}

AABB GameObject::GetBoundaryBox()
{
	math::AABB box;
	box.SetNegativeInfinity();

	for (uint i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() == COMP_MESH)
			box.Enclose(((Mesh*)components[i])->bounding_box);
	}

	for (uint i = 0; i < children.size(); i++)
	{
		box.Enclose(children[i]->GetBoundaryBox());
	}

	return box;
}
