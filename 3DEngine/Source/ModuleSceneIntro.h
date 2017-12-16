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
enum PRIMITIVE_TYPES;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	//bool Init();
	bool Start();
	update_status Update(float dt);
	bool CleanUp();
	void Draw();

	GameObject* CreateBasicGeometry(PRIMITIVE_TYPES type);
	void LoadBasicGeometryResources();

	GameObject* CreateNewGameObject(const char* name, GameObject* parent = nullptr);
	void LoadGameObjects(std::vector<GameObject*>* new_go_array, GameObject* array_root = nullptr, bool new_scene = false);
	bool DeleteGameObject(GameObject* to_delete);

	void CollectCandidates();
	void OrderCandidatesByTexture();

	bool AddGameObjectToOctree(const GameObject* object);
	void AddGameObjectToDynamic(const GameObject* object);
	void CheckDynamicGameObjectsState();
	void CheckStaticGameObjectsState();

	void CheckRayCastCollision(LineSegment& camera_ray);

	void LookAtScene() const;

	void DrawRootHierarchy();
	const char* GetRootName() const;
	bool ImSelected(int go_uuid) const;
	GameObject* GetSelectedGameObject() const;
	GameObject* LookForGameObject(int go_UUID) const;

	void SetProperties(GameObject* show_this);
	void DrawProperties() const;

	void SaveScene(const char* file_name);
	void LoadScene(int file_id);

	void LoadDefaultScene();

	void SaveToPlay();
	void LoadToStop();

	Component* NewOrphanComponent(COMP_TYPE new_comp_type);

public:
	GameObject* root = nullptr;
	
	ImGuizmo::OPERATION guizmo_op = ImGuizmo::OPERATION::TRANSLATE;
	GameObject* current_object = nullptr;

private:
	int selected_go_uuid = -1;
	
	std::vector<GameObject*> static_gameobjects;
	std::vector<GameObject*> dynamic_gameobjects;

	std::vector<MeshRenderer*> render_this;

	Octree<GameObject*>	scene_octree;

	LineSegment last_ray;

	//Simple id to store the resource for to play
	int temp_save_uid = 0;
};

#endif // !__MODULESCENEINTRO_H__