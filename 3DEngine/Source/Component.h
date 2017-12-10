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
	COMP_MESHRENDERER,
	COMP_CAMERA,

	END_UNIQUE_COMP,

	COMP_MATERIAL,

	COMP_RIGIDBODY,
	COMP_RBSHAPE,
};

class Component
{
public:
	Component();
	Component(GameObject* parent, COMP_TYPE type, bool isactive = true);
	virtual ~Component() {};

	virtual void Enable() { active = true; };
	virtual void Update() {};
	virtual void Disable() { active = false; };
	virtual void DrawComponent() {};

	virtual void UpdateTransform() {};

	//Save and Load methods
	virtual void Save(const char* buffer_data, char* cursor, int& bytes_copied) {};
	virtual void Load(char* cursor, int& bytes_copied) {};
	virtual void GetOwnBufferSize(uint& buffer_size);

	const char* TypeToString();
	COMP_TYPE GetType() const { return type; };
	bool IsActive() const { return active; };
	bool IsUnique() const;

	virtual void ChangeParent(GameObject* new_parent) { parent = new_parent; parent_UUID = new_parent->GetUUID(); };

	int GetParentUUID() const { return parent_UUID; };
	GameObject* GetParent() const { return parent; };
	void ReloadUUID();

protected:
	int UUID = 0;
	int parent_UUID = 0;
	bool active = false;
	bool unique = false;
	GameObject* parent = nullptr;

	COMP_TYPE type = COMP_UNKNOWN;

};


#endif // !__GAMEOBJECT_H__