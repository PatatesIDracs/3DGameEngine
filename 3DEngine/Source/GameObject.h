#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <vector>
#include "Math.h"

class Component;
enum COMP_TYPE;

class GameObject
{
public:
	GameObject(GameObject* parent);
	GameObject(GameObject* parent,const char* name);
	~GameObject();

	void Update();

	void AddChildren(GameObject* new_child);
	void AddComponent(Component* new_component);

	Component* FindComponent(COMP_TYPE type);

	void DrawHierarchy();

	AABB GetBoundaryBox();


public:
	std::string name;

	std::vector<Component*> components;

	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
};


#endif // !__GAMEOBJECT_H__
