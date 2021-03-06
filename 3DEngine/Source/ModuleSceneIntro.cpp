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
#include "RigidBody.h"
#include "RbCollider.h"
#include "DistanceJoint.h"
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

	//Create the root GameObject
	root = new GameObject(nullptr, "Untitled Scene");
	current_object = root;

	LoadBasicGeometryResources();
	LoadDefaultScene();
	
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

	int selected_go_uuid = -1;

	scene_octree.Clear();

	return true;
}

void ModuleSceneIntro::Draw()
{
	uint texture_buffer = 0;

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
			if (texture_buffer != object->material->GetTextureID()) {
				// Clear texture buffer;
				glBindTexture(GL_TEXTURE_2D, 0);
				texture_buffer = object->material->GetTextureID();

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glBindTexture(GL_TEXTURE_2D, texture_buffer);
			}
		}
		else if (texture_buffer > 0){
			glBindTexture(GL_TEXTURE_2D, 0);
			texture_buffer = 0;
		}

		if (object->mesh != nullptr && object->mesh->IsActive())
		{
			const RenderData* mesh_render_data = object->mesh->GetRenderData();
			
			glEnableClientState(GL_VERTEX_ARRAY);

			if (mesh_render_data->normals != nullptr)
			{
				glEnableClientState(GL_NORMAL_ARRAY);
				glBindBuffer(GL_ARRAY_BUFFER, mesh_render_data->id_normals);
				glNormalPointer(GL_FLOAT, 0, NULL);
			}

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
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glPopMatrix();
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	render_this.clear();
}


// Update
update_status ModuleSceneIntro::Update(float dt)
{
	//Root should never be nullptr but check it just in case
	if (root != nullptr) 
		root->Update();

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

		
		if(App->renderer3D->show_octree)
			scene_octree.Draw(3.0f, float4(0.25f, 1.00f, 0.00f, 1.00f));
	}

	/*if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN && current_object != root) {
		DeleteGameObject(current_object);
		current_object = nullptr;
	}*/

	// Generate Basic Geometry file
	/*
	if (App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN && current_object != nullptr)
	{
		Mesh* mesh = (Mesh*)current_object->FindFirstComponent(COMP_MESH);
		const RenderData* data = mesh->mesh_resource->GetRenderData();
		ResourceMesh* r_mesh = (ResourceMesh*)App->resources->CreateNewResource(RESOURCE_MESH, PRIM_CYLINDER);
		r_mesh->SetRenderData((RenderData*)data);
		r_mesh->SaveResource();
	}*/

	CollectCandidates();

	return UPDATE_CONTINUE;
}


GameObject* ModuleSceneIntro::CreateBasicGeometry(PRIMITIVE_TYPES type)
{
	std::string name = "empty";
	ResourceMesh* resource = (ResourceMesh*)App->resources->GetFromUID(type);

	switch (type)
	{
	case PRIM_PLANE: name = "Plane";
		break;
	case PRIM_CUBE: name = "Cube";
		break;
	case PRIM_SPHERE: name = "Sphere";
		break;
	case PRIM_CYLINDER: name = "Cylinder";
		break;
	}

	if (resource != nullptr) {
		
		GameObject* new_primitive = nullptr;
		if (current_object != nullptr) {
			new_primitive = CreateNewGameObject(name.c_str(), current_object);
		}
		else new_primitive = CreateNewGameObject(name.c_str());

		Mesh* mesh = (Mesh*)new Mesh(new_primitive);
		mesh->SetMeshResource(resource);
		Material* mat = new Material(new_primitive,nullptr);
		MeshRenderer* render = new MeshRenderer(new_primitive);
		return new_primitive;
	}
	else {
		LOGC("Load Failed: Primitive Resource not found");
		return nullptr;
	}
}

void ModuleSceneIntro::LoadBasicGeometryResources()
{
	//Load mesh resource
	for(uint i = 1; i < 5; i++){
		ResourceMesh* new_mesh = (ResourceMesh*)App->resources->CreateNewResource(RESOURCE_MESH, i);
		new_mesh->SetName("primitive" + std::to_string(i));
		new_mesh->SetLibraryFile(JOPE_PRIMITIVE_FOLDER + std::to_string(i) + MJOPE);
		new_mesh->LoadResource();
	}
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

void ModuleSceneIntro::LoadGameObjects(std::vector<GameObject*>* new_go_array, GameObject* array_root, bool new_scene)
{
	//Get the root of the array, if it's not provided try to fin one
	GameObject* loaded_root = array_root;
	if (loaded_root == nullptr)
	{
		for (uint i = 0; i < new_go_array->size(); i++)
		{
			if ((*new_go_array)[i]->parent == nullptr || (*new_go_array)[i]->GetparentUUID() == 0)
			{
				loaded_root = (*new_go_array)[i];
				break;
			}
		}
	}
	//if we didn't find a loaded root, do not continue loading the scene/prefab
	if (loaded_root == nullptr)
	{
		LOGC("Unable to load scene/prefab root GameObject not found");
		return;
	}

	//If root is nullptr don't load, scene is probably not ready yet
	//This may happen when opening the software, when the scene resources are loaded to the map they will try
	//to load their gameobjects to scene, this will prevent it. We still want to load it to check the file is not corrupted
	if (root == nullptr)
	{
		delete loaded_root;
		return;
	}

	//Reload the GameObjects UUID to avoid having the same id in multiple objects when loading more than 1 prefabs
	loaded_root->ReloadUUID();

	//If it's a new scene delete the current scene and load the new one
	//All objects from the vectors will be deleted when root is deleted
	if (new_scene)
	{
		dynamic_gameobjects.clear();
		static_gameobjects.clear();
		scene_octree.Reset();
		render_this.clear();

		if (root != nullptr)
			delete root;		
	}

	for (uint i = 0; i < new_go_array->size(); i++)
	{
		dynamic_gameobjects.push_back((*new_go_array)[i]);
	}

	if (new_scene)
		root = loaded_root;
	else
		root->AddChildren(loaded_root);

	current_object = nullptr;

	loaded_root->GetTransform()->update_transform = true;
	loaded_root->Update();
	CheckDynamicGameObjectsState();
}

bool ModuleSceneIntro::DeleteGameObject(GameObject* to_delete) {
	
	if (to_delete != nullptr) {
		delete to_delete;

		dynamic_gameobjects.clear();
		static_gameobjects.clear();
		scene_octree.Reset();

		root->AddToScene();
		CheckDynamicGameObjectsState();
		return true;
	}
	else return false;
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
		
			if (mesh != nullptr && mesh->IsActive()) {
				mesh->ready_to_draw = false;
				render_this.push_back(mesh);
			}
		}
	}
	
	OrderCandidatesByTexture();
}

void ModuleSceneIntro::OrderCandidatesByTexture()
{
	int count = 0;
	uint texture_id = 0;
	uint render_size = render_this.size();

	MeshRenderer* object_i = nullptr;
	MeshRenderer* object_j = nullptr;

	std::vector<MeshRenderer*> render_order;
	render_order.reserve(render_size);
	render_order = render_this;

	for (uint i = 0; i < render_size; i++) {
		object_i = render_order[i];
		if (!object_i->ready_to_draw) {
			
			// Add object to temporal render vector
			render_this[count] = object_i;
			object_i->ready_to_draw = true;
			count++;

			// If resource Loaded only once skip next step
			if (object_i->material != nullptr && object_i->material->GetResourceLoaded() != 1) {
				// Set current texture id
				texture_id = object_i->material->GetTextureID();

				// Look for objects that share same texture
				for (uint j = 0; j < render_size; j++) {
					object_j = render_order[j];
					if (object_j->ready_to_draw) continue;
					
					if ((object_j->material && texture_id == object_j->material->GetTextureID())|| (!object_j->material && texture_id == 0)) {
						render_this[count] = object_j;
						object_j->ready_to_draw = true;
						count++;
					}
				}
			}
		}
	}
	render_order.clear();
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

void ModuleSceneIntro::AddGameObjectToDynamic(const GameObject * object)
{
	if(object != nullptr)
		dynamic_gameobjects.push_back((GameObject*)object);
}


void ModuleSceneIntro::CheckDynamicGameObjectsState()
{
	GameObject* object = nullptr;
	for (int i = dynamic_gameobjects.size()-1; i >= 0; i--)
	{
		object = dynamic_gameobjects[i];
		if (object->isstatic) {
			for (uint j = i; j + 1 < dynamic_gameobjects.size(); j++)
				dynamic_gameobjects[j] = dynamic_gameobjects[j + 1];
			dynamic_gameobjects.pop_back();
			static_gameobjects.push_back(object);
			AddGameObjectToOctree(object);
		}
	}
}

void ModuleSceneIntro::CheckStaticGameObjectsState()
{
	bool reset_octree = false;
	GameObject* object = nullptr;
	for (int i = static_gameobjects.size() - 1; i >= 0; i--) 
	{
		object = static_gameobjects[i];
		if (!object->isstatic) {
			for (uint j = i; j + 1 < static_gameobjects.size(); j++)
				static_gameobjects[j] = static_gameobjects[j + 1];
			static_gameobjects.pop_back();
			dynamic_gameobjects.push_back(object);
			reset_octree = true;
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
	else current_object = nullptr;
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
	GameObject* ret = nullptr;

	for (uint i = 0; i < root->children.size(); i++)
	{
		ret = root->children[i]->DrawHierarchy();
		if (ret != nullptr)
		{
			current_object = ret;
			selected_go_uuid = current_object->GetUUID();
		}
	}
}

const char * ModuleSceneIntro::GetRootName() const
{
	return root->name.c_str();
}

bool ModuleSceneIntro::ImSelected(int go_uuid) const
{

	if (current_object != nullptr && go_uuid == current_object->GetUUID())
		return true;
	else
		return false;
}

GameObject * ModuleSceneIntro::GetSelectedGameObject() const
{
	return current_object;
}

GameObject * ModuleSceneIntro::LookForGameObject(int go_UUID) const
{
	for (uint i = 0; i < dynamic_gameobjects.size(); i++)
	{
		if (dynamic_gameobjects[i]->GetUUID() == go_UUID)
			return dynamic_gameobjects[i];
	}
	for (uint i = 0; i < static_gameobjects.size(); i++)
	{
		if (static_gameobjects[i]->GetUUID() == go_UUID)
			return static_gameobjects[i];
	}

	return nullptr;
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
	root->name = file_name;
	save_file_name.append(SCENEFORMAT);
	ResourceScene* save_scene = (ResourceScene*)App->resources->CreateNewResource(RESOURCE_TYPE::RESOURCE_SCENE);
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

	App->resources->GotFocus(true);
}

void ModuleSceneIntro::LoadScene(int file_id)
{
	LOGC("Loading the scene ...");
	ResourceScene* load_scene = (ResourceScene*)App->resources->GetFromUID(file_id);
	load_scene->LoadResource();

}

void ModuleSceneIntro::LoadDefaultScene()
{
	if (root != nullptr) {
		delete root;
		
		scene_octree.Reset();
		dynamic_gameobjects.clear();
		static_gameobjects.clear();
		render_this.clear();

		//Create the root GameObject
		root = new GameObject(nullptr, "root");

		//Set Up current object to show its properties
		current_object = nullptr;
		selected_go_uuid = -1;

		GameObject* camera = CreateNewGameObject("Camera", nullptr);
		Camera* camera_test = new Camera(camera, true);
		camera_test->SetFrustumViewAngle();
		camera->AddComponent(camera_test);
	}
}

void ModuleSceneIntro::SaveToPlay()
{
	ResourceScene* play_save = (ResourceScene*)App->resources->CreateNewResource(RESOURCE_TYPE::RESOURCE_SCENE);
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
	case COMP_RIGIDBODY:
		ret = new RigidBody(nullptr);
		break;
	case COMP_RBSHAPE:
		ret = new RbCollider(nullptr);
		break;
	case COMP_DISTANCEJOINT:
		ret = new DistanceJoint(nullptr);
			break;
	default:
		break;
	}
	return ret;
}
