#include "GameObject.h"
#include "Component.h"
#include "Transform.h"


GameObject::GameObject(GameObject* parent) : parent(parent)
{
	if (parent != nullptr)
		parent->AddChildren(this);
}

GameObject::GameObject(GameObject * parent, char * name) : parent(parent), name(name)
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

Component * GameObject::FindComponent(COMP_TYPE type)
{
	return nullptr;
}
