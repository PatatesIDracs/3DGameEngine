#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <vector>

class Component;
enum COMP_TYPE;

class GameObject
{
public:
	GameObject();
	~GameObject();

	void Update();

	void AddComponent(Component* new_component);

	Component* FindComponent(COMP_TYPE type);

public:
	std::string name;

	std::vector<Component*> components;

	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
};


#endif // !__GAMEOBJECT_H__
