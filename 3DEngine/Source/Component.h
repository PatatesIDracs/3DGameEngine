#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"
#include "Math.h"
#include "glmath.h"

#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_sdl_gl3.h"

#include "GameObject.h"

enum COMP_TYPE
{
	COMP_UNKNOWN,
	COMP_TRANSFORM,
	COMP_MESH,
	COMP_MATERIAL,
	COMP_MESHRENDERER,
	COMP_CAMERA
};

class Component
{
public:
	Component() : parent(nullptr), type(COMP_UNKNOWN), active(false) {};
	Component(GameObject* parent, COMP_TYPE type, bool isactive = true) : parent(parent), type(type), active(isactive) {};
	virtual ~Component() {};

	virtual void Enable() { active = true; };
	virtual void Update() {};
	virtual void Disable() { active = false; };
	virtual void DrawComponent() {};

	COMP_TYPE GetType() const { return type; };

public:
	bool unique = false;

protected:
	
	bool active = false;
	GameObject* parent = nullptr;

private:
	COMP_TYPE type = COMP_UNKNOWN;

};


#endif // !__GAMEOBJECT_H__