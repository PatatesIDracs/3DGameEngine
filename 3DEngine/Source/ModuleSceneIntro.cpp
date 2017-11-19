#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"

#include "GameObject.h"
#include "Camera.h"
#include "Transform.h"
#include "Mesh.h"
#include "ResourceMesh.h"
#include "ResourceScene.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "ConfigJSON.h"
#include "parson.h"

#include "Glew\include\glew.h"

#include <fstream>


ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app,"Scene", start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOGC("Loading Intro assets");
	bool ret = true;
	
	AABB octree_limit;
	octree_limit.SetFromCenterAndSize(float3(0.0f, -4.0f, 0.0f), float3(128.0f, 128.0f, 128.0f));
	scene_octree.Create(octree_limit, 4);

	App->camera->Move(vec(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec(0, 0, 0));
	
	//Create the root GameObject
	root = new GameObject(nullptr, "root");

	//Set Up current object to show its properties
	current_object = root;

	App->camera->SetCameraEditor();

	GameObject* camera = CreateNewGameObject("Camera", nullptr);
	Camera* camera_test = new Camera(camera, true);
	camera_test->SetFrustumViewAngle();
	camera->AddComponent(camera_test);
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOGC("Unloading Intro scene");

	if (root != nullptr) delete root;

	dynamic_gameobjects.clear();
	static_gameobjects.clear();

	render_this.clear();


	return true;
}

void ModuleSceneIntro::Draw()
{
	MeshRenderer* object = nullptr;
	for (uint i = 0; i < render_this.size(); i++)
	{
		object = render_this[i];
		if (object->transform == nullptr)
			continue;

		glPushMatrix();
		glMultMatrixf(object->transform->GetGlobalTransform().Transposed().ptr());

		if (object->material != nullptr)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBindTexture(GL_TEXTURE_2D, object->material->GetTextureID());
		}

		if (object->mesh != nullptr && object->mesh->IsActive())
		{
			const RenderData* mesh_render_data = object->mesh->GetRenderData();
			
			glEnableClientState(GL_VERTEX_ARRAY);

			if (mesh_render_data->tex_vertices != nullptr)
			{
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);

				//Bind texture coords buffer
				glBindBuffer(GL_ARRAY_BUFFER, mesh_render_data->id_tex_vertices);
				glTexCoordPointer(3, GL_FLOAT, 0, NULL);
			}

			//Bind vertex buffer
			glBindBuffer(GL_ARRAY_BUFFER, mesh_render_data->id_vertices);
			glVertexPointer(3, GL_FLOAT, 0, NULL);

			//Bind and draw with indices
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_render_data->id_indices);
			glDrawElements(GL_TRIANGLES, mesh_render_data->num_indices, GL_UNSIGNED_INT, NULL);

			//Disable opengl states
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisable(GL_BLEND);
		}

		// Clear possible Binded buffers
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glPopMatrix();
	}
	render_this.clear();
}


// Update
update_status ModuleSceneIntro::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		LoadScene("Shame");

	if (App->clock.state != APP_PLAY) {
		Primitive a;
		a.axis = true;
		a.Render();

		oldPlane p(0, 1, 0, 0);
		if (App->renderer3D->show_grid) p.Render();

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
			guizmo_op = ImGuizmo::OPERATION::TRANSLATE;
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
			guizmo_op = ImGuizmo::OPERATION::ROTATE;
		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
			guizmo_op = ImGuizmo::OPERATION::SCALE;

		//Root should never be nullptr but check it just in case
		if (root != nullptr) {

			root->Update();
			scene_octree.Draw(3.0f, float4(0.25f, 1.00f, 0.00f, 1.00f));
		}
	}

	CollectCandidates();

	return UPDATE_CONTINUE;
}

GameObject * ModuleSceneIntro::CreateNewGameObject(const char* name, GameObject* parent)
{
	GameObject* ret = nullptr;

	if (parent == nullptr)
		ret = new GameObject(root, name);
	else
		ret = new GameObject(parent, name);

	dynamic_gameobjects.push_back(ret);

	current_object = ret;
	return ret;
}

void ModuleSceneIntro::LoadGameObjects(std::vector<GameObject*>* new_go_array, bool new_scene)
{
	//If root is nullptr don't load, scene is probably not ready yet
	//This may happen when opening the software, when the scene resources are loaded to the map they will try
	//to load their gameobjects to scene, this will prevent it. We still want to load it to check the file is not corrupted
	if (root == nullptr)
	{
		for (uint i = 0; i < new_go_array->size(); i++)
		{
			if ((*new_go_array)[i]->parent == nullptr || (*new_go_array)[i]->parent_UUID == 0)
			{
				delete (*new_go_array)[i];
				break;
			}
		}

		return;
	}

	GameObject* loaded_root = nullptr;
	//If it's a new scene delete the current scene and load the new one
	//All objects from the vectors will be deleted when root is deleted
	if (new_scene)
	{
		dynamic_gameobjects.clear();
		static_gameobjects.clear();
		if (root != nullptr)
			delete root;		
	}


	for (uint i = 0; i < new_go_array->size(); i++)
	{
		dynamic_gameobjects.push_back((*new_go_array)[i]);
		if ((*new_go_array)[i]->parent == nullptr || (*new_go_array)[i]->parent_UUID == 0)
			loaded_root = (*new_go_array)[i];
	}

	if (new_scene)
	{
		root = loaded_root;
	}
	else
		root->AddChildren(loaded_root);

	current_object = nullptr;
	CheckDynamicGameObjectsState();
}

void ModuleSceneIntro::CollectCandidates()
{
	// Get Dynamic Meshes to render
	App->camera->GetMainCamera()->GetFrustumGameObjecs(dynamic_gameobjects, render_this);

	// Get Static Meshes to render
	std::vector<GameObject*> candidates;
	scene_octree.Intersect(candidates, App->camera->GetMainCamera()->GetFrustum());

	for (uint i = 0; i < candidates.size(); i++) {
		if (candidates[i]->IsActive()) {
			MeshRenderer* mesh = (MeshRenderer*)(candidates[i]->FindFirstComponent(COMP_MESHRENDERER));
		
			if (mesh != nullptr) render_this.push_back(mesh);
		}
	}
	
}

bool ModuleSceneIntro::AddGameObjectToOctree(const GameObject* object)
{
	if (object != nullptr && object->isstatic) {
		if (scene_octree.Insert((GameObject*)object, object->GetBoundaryBox())) {
			return true;
		}
	}
	return false;
}


void ModuleSceneIntro::CheckDynamicGameObjectsState()
{
	GameObject* object = nullptr;
	for (uint i = 0; i < dynamic_gameobjects.size(); i++)
	{
		object = dynamic_gameobjects[i];
		if (object->isstatic) {
			for (uint j = i; j + 1 < dynamic_gameobjects.size(); j++)
				dynamic_gameobjects[j] = dynamic_gameobjects[j + 1];
			dynamic_gameobjects.pop_back();
			static_gameobjects.push_back(object);
			AddGameObjectToOctree(object);
			i--;
		}
	}
}

void ModuleSceneIntro::CheckStaticGameObjectsState()
{
	bool reset_octree = false;
	GameObject* object = nullptr;
	for (uint i = 0; i < static_gameobjects.size(); i++) {
		object = static_gameobjects[i];
		if (!object->isstatic) {
			for (uint j = i; j + 1 < static_gameobjects.size(); j++)
				static_gameobjects[j] = static_gameobjects[j + 1];
			static_gameobjects.pop_back();
			dynamic_gameobjects.push_back(object);
			reset_octree = true;
			i--;
		}
	}

	if (reset_octree) {
		scene_octree.Reset();
			
		for (uint i = 0; i < static_gameobjects.size(); i++)
				scene_octree.Insert(static_gameobjects[i], static_gameobjects[i]->GetBoundaryBox());
		
	}
}

void ModuleSceneIntro::CheckRayCastCollision(LineSegment & camera_ray)
{
	// Need a lot of Polish;
	last_ray = camera_ray;
	std::vector<float3> order;
	float ndist = 0, fdist = 0;
	// Test Dynamic GameObjects
	for (uint i = 0; i < dynamic_gameobjects.size(); i++) {
		if (dynamic_gameobjects[i]->IsActive() && dynamic_gameobjects[i]->GetBoundaryBox().IsFinite())
		{
			if (camera_ray.Intersects(dynamic_gameobjects[i]->GetBoundaryBox(), ndist, fdist))
				order.push_back(float3(ndist, (float)i, 0));
		}
	}

	// Test Static GameObjects with octree
	std::vector<GameObject*> static_hits;
	std::vector<float> hit_distance;
	scene_octree.Intersect(static_hits, camera_ray, hit_distance);
	
	for (uint i = 0; i < static_hits.size(); i++) {
			order.push_back(float3(hit_distance[i], (float)i, 1));
	}
	hit_distance.clear();

	// Order GameObjects by hit Distance
	uint order_size = order.size();
	if (order_size > 0) {
		std::vector<MeshRenderer*> meshes;
		meshes.reserve(order_size);

		MeshRenderer* mesh = nullptr;

		float3 curr_min = order[0];
		int curr_minim_order = 0;;
		for (uint i = 0; i < order_size; i++) {
			for (uint j = 0; j < order_size; j++) {
				if (curr_min.z == -1 && order[j].z != -1) curr_min = order[j];

				if (curr_min.x >= order[j].x && order[j].z != -1) {
					curr_min = order[j];
					curr_minim_order = j;
				}
			}

			// Add Hit
			if (curr_min.z == 0) {
				mesh = (MeshRenderer*)dynamic_gameobjects[(int)curr_min.y]->FindFirstComponent(COMP_MESHRENDERER);
			}
			else {
				mesh = (MeshRenderer*)static_hits[(int)curr_min.y]->FindFirstComponent(COMP_MESHRENDERER);
			}
			order[curr_minim_order].z = -1;
			curr_min.z = -1;

			if (mesh != nullptr)meshes.push_back(mesh);
			
		}
		static_hits.clear();
		order.clear();

		// Find Closest Intersection with Mesh
		ndist = -1;
		float3 intersection;
		for (uint i = 0; i < order_size; i++) {
			if (meshes[i]->mesh->CheckRayCollision(last_ray, ndist, intersection)) {
				current_object = meshes[i]->GetParent();
			}
		}
		meshes.clear();
	}

}

void ModuleSceneIntro::LookAtScene() const
{
	if (current_object != nullptr)
	{
		math::AABB box = current_object->GetGlobalBoundaryBox();
		if (box.IsFinite())
		{
			vec pos = box.CenterPoint();
			float dist = box.HalfSize().Length();

			App->camera->MoveTo(vec(pos.x, pos.y, pos.z), dist);
		}
	}
}

void ModuleSceneIntro::DrawRootHierarchy()
{
	GameObject* ret = root->DrawHierarchy();
	if (ret != nullptr) current_object = ret;
}

GameObject * ModuleSceneIntro::GetSelectedGameObject() const
{
	return current_object;
}

void ModuleSceneIntro::SetProperties(GameObject * show_this)
{
	current_object = show_this;
}

void ModuleSceneIntro::DrawProperties() const
{
	if (current_object != nullptr) current_object->DrawProperties();
}

void ModuleSceneIntro::SaveScene(const char* file_name)
{
	LOGC("Saving the scene...");
	std::string save_file_name = file_name;
	save_file_name.append(SCENEFORMAT);
	ResourceScene* save_scene = (ResourceScene*)App->resources->CreateNewResource(RESOURCE_TYPE::RESOURCE_SCENE);
	save_scene->SetAsRoot(true);
	//std::ofstream new_file(save_file_name.c_str(), std::ofstream::binary);
	save_scene->SetAssetFile(save_file_name);
	save_scene->SaveResource(root);
	save_file_name = JOPE_DATA_DIRECTORY JOPE_ASSETS_FOLDER + save_file_name;
	App->resources->GetImporter()->CopyFileToFolder((JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER + std::to_string(save_scene->GetUID()) + SCENEFORMAT).c_str(),save_file_name.c_str());

	App->resources->GetImporter()->FoundMetaFile((save_file_name + METAFORMAT).c_str());
	Config_Json meta_file((save_file_name + METAFORMAT).c_str());
	meta_file.SetInt("UUID", save_scene->GetUID());
	meta_file.SetInt("Creation Time", App->resources->GetImporter()->GetLastTimeWritten(save_file_name.c_str()));
	meta_file.SaveToFile((save_file_name + METAFORMAT).c_str());
}

void ModuleSceneIntro::LoadScene(const char * assets_file_path)
{
	LOGC("Loading the scene ...");
	ResourceScene* load_scene = (ResourceScene*)App->resources->GetFromUID(1169667521);
	load_scene->LoadResource();

}

void ModuleSceneIntro::SaveToPlay()
{
	ResourceScene* play_save = (ResourceScene*)App->resources->CreateNewResource(RESOURCE_TYPE::RESOURCE_SCENE);
	play_save->SetAsRoot(true);
	play_save->SaveResource(root);
	temp_save_uid = play_save->GetUID();
	render_this.clear();
}

void ModuleSceneIntro::LoadToStop()
{
	//Load the scene
	ResourceScene* play_save = (ResourceScene*)App->resources->GetFromUID(temp_save_uid);
	play_save->LoadResource();
	App->resources->DeleteFileFromUID(temp_save_uid);
	int temp_save_id = 0;
	render_this.clear();
}

Component * ModuleSceneIntro::NewOrphanComponent(COMP_TYPE new_comp_type)
{
	Component* ret = nullptr;

	switch (new_comp_type)
	{
	case COMP_UNKNOWN:
		LOGC("File seems corrupted");
		break;
	case COMP_TRANSFORM:
		ret = new Transform(nullptr);
		break;
	case COMP_MESH:
		ret = new Mesh(nullptr, nullptr);
		break;
	case COMP_MATERIAL:
		ret = new Material(nullptr, nullptr);
		break;
	case COMP_MESHRENDERER:
		ret = new MeshRenderer(nullptr);
		break;
	case COMP_CAMERA:
		ret = new Camera(nullptr);
		break;
	default:
		break;
	}
	return ret;
}
