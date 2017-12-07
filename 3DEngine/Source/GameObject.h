#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <vector>
#include "Math.h"
#include "Globals.h"
 

class Component;
class Transform;
enum COMP_TYPE;

class GameObject
{
public:
	GameObject(GameObject* parent, bool isactive = true);
	GameObject(GameObject* parent,const char* go_name, bool isactive = true);
	~GameObject();

	void Update();

	//Set methods
	void AddToScene() const;
	void AddChildren(GameObject* new_child);
	void AddComponent(Component* new_component, bool overwrite = false);
	void SetTransform(float4x4 &transform);

	void RemoveChildren(GameObject* remove_child);

	//Get/Check state methods
	bool IsActive()const;
	bool IsRoot()const;
	Transform* GetTransform() const;
	AABB GetBoundaryBox() const;
	AABB GetGlobalBoundaryBox() const;
	Component* FindFirstComponent(COMP_TYPE type);
	std::vector<Component*>* FindAllTypeComponents(COMP_TYPE type);

	//UI Drawing methods
	GameObject* DrawHierarchy();
	void DrawProperties();
	void DrawGameObject();

	//Save and load methods
	void Save(const char* buffer_data, char* cursor, int& bytes_copied);
	void Load(char* cursor, int& bytes_copied);
	void GetOwnBufferSize(uint& buffer_size);
	void ReloadUUID();

	int GetparentUUID() const;
	int GetUUID() const;

private:
	void DrawAddComponentWindow();
	void DrawSetStaticGObjWindow();
	void ChangeChildsStatic(bool state);

public:
	std::string name;
	char new_name[50];	//Temp array use when changing names
	bool isactive = true;
	bool parent_active = true;
	bool isstatic = false;

	std::vector<Component*> components;
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;

	AABB boundary_box;

private:
	int UUID = 0;
	int parent_UUID = 0;
	bool creating_component = false;
	bool change_static = false;
};


#endif // !__GAMEOBJECT_H__
