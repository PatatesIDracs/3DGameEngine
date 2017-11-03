#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"
#include "Math.h"
 

#include "Primitive.h"

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
	Component() : parent(nullptr), type(COMP_UNKNOWN), active(false) 
	{
		LCG UUIDGen;
		UID = UUIDGen.Int();
	};
	Component(GameObject* parent, COMP_TYPE type, bool isactive = true) : parent(parent), type(type), active(isactive) 
	{
		LCG UUIDGen;
		UID = UUIDGen.Int();
	};
	virtual ~Component() {};

	virtual void Enable() { active = true; };
	virtual void Update() {};
	virtual void Disable() { active = false; };
	virtual void DrawComponent() {};

	virtual void UpdateTransform() {};

	COMP_TYPE GetType() const { return type; };
	bool IsActive() const { return active; };

public:
	bool unique = false;

protected:
	int UID = 0;
	bool active = false;
	GameObject* parent = nullptr;

private:
	COMP_TYPE type = COMP_UNKNOWN;

};


#endif // !__GAMEOBJECT_H__