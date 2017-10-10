#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"
#include "glmath.h"

class GameObject;

enum COMP_TYPE
{
	COMP_UNKNOWN,
	COMP_TRANSFORM,
	COMP_MESH,
	COMP_MATERIAL
};

class Component
{
public:
	Component() : parent(nullptr), type(COMP_UNKNOWN), active(false){};
	Component(GameObject* parent, COMP_TYPE type, bool isactive = true) : parent(parent), type(type), active(isactive){};
	~Component() {};

	virtual void Enable() { active = true; };
	virtual void Update() {};
	virtual void Disable() { active = false; };

protected:
	
	bool active = false;
	GameObject* parent = nullptr;

private:
	COMP_TYPE type = COMP_UNKNOWN;

};


#endif // !__GAMEOBJECT_H__