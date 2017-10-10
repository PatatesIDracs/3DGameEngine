#include "GameObject.h"
#include "Component.h"


GameObject::GameObject()
{

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

void GameObject::AddComponent(Component * new_component)
{
	components.push_back(new_component);
}

Component * GameObject::FindComponent(COMP_TYPE type)
{
	return nullptr;
}
