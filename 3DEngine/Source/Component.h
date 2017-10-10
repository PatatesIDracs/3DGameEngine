#ifndef __COMPONENT_H__
#define __COMPONENT_H__

class GameObject;

enum COMP_TYPE
{
	TRANSFORM = 0,
	MESH,
	MATERIAL
};


class Component
{
public:
	Component();
	~Component();

	virtual void Enable();
	virtual void Update();
	virtual void Disable();

public:
	
	bool active = false;

	GameObject* parent = nullptr;

};


#endif // !__GAMEOBJECT_H__