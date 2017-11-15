#ifndef __MODULESCENEINTRO_H__
#define __MODULESCENEINTRO_H__


#include "Module.h"
#include "Math.h"
#include "OcTree.h"
#include "Imgui\ImGuizmo.h"

class GameObject;
class MeshRenderer;
class Camera;
class Component;
enum COMP_TYPE;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();
	void Draw();

	GameObject* CreateNewGameObject(const char* name, GameObject* parent = nullptr);

	void CollectCandidates();
	bool AddGameObjectToOctree(const GameObject* object);
	void CheckDynamicGameObjectsState();
	void CheckStaticGameObjectsState();

	void CheckRayCastCollision(LineSegment& camera_ray);

	void LookAtScene() const;

	void DrawRootHierarchy();
	GameObject* GetSelectedGameObject() const;

	void SetProperties(GameObject* show_this);
	void DrawProperties() const;

	void SaveScene();
	void LoadScene(const char* file_path);

private:
	Component* ComponentToLoad(COMP_TYPE new_comp_type);

public:
	GameObject* root = nullptr;
	
	ImGuizmo::OPERATION guizmo_op = ImGuizmo::OPERATION::TRANSLATE;

private:
	GameObject* current_object = nullptr;
	
	std::vector<GameObject*> static_gameobjects;
	std::vector<GameObject*> dynamic_gameobjects;

	std::vector<MeshRenderer*> render_this;

	Octree<GameObject*>	scene_octree;

	LineSegment last_ray;
};

#endif // !__MODULESCENEINTRO_H__