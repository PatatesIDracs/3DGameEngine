#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <vector>
#include "Math.h"
 

class Component;
class Transform;
enum COMP_TYPE;

class GameObject
{
public:
	GameObject(GameObject* parent, bool isactive = true);
	GameObject(GameObject* parent,const char* name, bool isactive = true);
	~GameObject();

	void Update();

	void AddChildren(GameObject* new_child);
	void AddComponent(Component* new_component);

	bool IsActive()const { return isactive; }

	void SetTransform(float4x4 &transform);
	Transform* GetTransform();

	Component* FindUniqueComponent(COMP_TYPE type);

	GameObject* DrawHierarchy();
	void DrawProperties();

	AABB GetBoundaryBox();


public:
	std::string name;
	bool isactive = true;

	std::vector<Component*> components;

	GameObject* parent = nullptr;
	std::vector<GameObject*> children;

	AABB boundary_box;
};


#endif // !__GAMEOBJECT_H__
