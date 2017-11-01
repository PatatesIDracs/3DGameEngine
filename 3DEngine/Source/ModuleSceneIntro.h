#ifndef __MODULESCENEINTRO_H__
#define __MODULESCENEINTRO_H__


#include "Module.h"
#include "Math.h"


class GameObject;
class MeshRenderer;
class Camera;

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

	void LookAtScene() const;

	void DrawRootHierarchy();

	void SetProperties(GameObject* show_this);
	void DrawProperties() const;

public:
	GameObject* root = nullptr;

private:
	GameObject* current_object = nullptr;
	Camera* render_camera_test = nullptr;

	std::vector<MeshRenderer*> render_this;

};

#endif // !__MODULESCENEINTRO_H__