#include "GameObject.h"
#include "Component.h"
#include "Transform.h"


GameObject::GameObject(GameObject* parent) : parent(parent)
{
	if (parent != nullptr)
		parent->AddChildren(this);
}

GameObject::~GameObject()
{
}

void GameObject::Update()
{
	//Check if the GameObject has a gameobject list
	if (children.size() != 0)
	{
		for (int i = 0; i < children.size(); i++)
		{
			children[i]->Update();
		}
	}
	
	for (int i = 0; i < components.size(); i++)
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
