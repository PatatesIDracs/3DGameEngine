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

	bool IsActive()const { return isactive && parent_active; }
	bool IsRoot()const { return (parent == NULL ? true : false); }

	void SetTransform(float4x4 &transform);
	Transform* GetTransform();

	Component* FindUniqueComponent(COMP_TYPE type);

	GameObject* DrawHierarchy();

	void DrawProperties();
	void DrawGameObject();

	AABB GetBoundaryBox();


public:
	int UID = 0;
	std::string name;
	bool isactive = true;
	bool parent_active = true;

	bool isstatic = false;

	std::vector<Component*> components;

	GameObject* parent = nullptr;
	std::vector<GameObject*> children;

	AABB boundary_box;
};


#endif // !__GAMEOBJECT_H__
