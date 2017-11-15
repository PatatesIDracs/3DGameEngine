#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"

#include "GameObject.h"
#include "Camera.h"
#include "Transform.h"
#include "Mesh.h"
#include "ResourceMesh.h"
#include "MeshRenderer.h"
#include "Material.h"

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
	if (App->clock.state != APP_PLAY) {
		Primitive a;
		a.axis = true;
		a.Render();

		oldPlane p(0, 1, 0, 0);
		if (App->renderer3D->show_grid) p.Render();

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
	bool update_octree = false;
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

void ModuleSceneIntro::SaveScene()
{
	LOGC("Saving the scene...");

	//TODO: Get the buffer size
	uint buffer_size = 0;
	root->GetOwnBufferSize(buffer_size);
	buffer_size += sizeof(int);
	LOGC("Buffer size is %i", buffer_size);

	//TODO: Create the buffer and cursor
	char* buffer_data = new char[buffer_size];
	char* cursor = buffer_data;

	//Pass the buffer and cursor and go saving data
	int bytes_copied = 0;
	root->Save(buffer_data, cursor, bytes_copied);
	cursor += bytes_copied;
	int identifier = ENDFILEIDENTIFIER;
	memcpy(cursor, &identifier, sizeof(identifier));
	
	std::ofstream new_file("../Data/Assets/TestScene.jope", std::ofstream::binary);
	new_file.write(buffer_data, buffer_size);
}

void ModuleSceneIntro::LoadScene(const char * file_path)
{
	LOGC("Loading the scence from %s", file_path);
	static_gameobjects.clear();
	dynamic_gameobjects.clear();

	char* buffer_data = nullptr;
	int buffer_size = 0;
	std::ifstream loaded_file(file_path, std::fstream::binary);
	if (loaded_file)
	{
		loaded_file.seekg(0, loaded_file.end);
		buffer_size = loaded_file.tellg();
		loaded_file.seekg(0, loaded_file.beg);

		buffer_data = new char[buffer_size];

		loaded_file.read(buffer_data, buffer_size);
		loaded_file.close();
	}
	if (buffer_data == nullptr)
	{
		LOGC("Couldn't load file from %s", file_path);
		return;
	}
	char* cursor = buffer_data;
	int bytes_copied = 0;
	uint bytes_to_copy = 0;
	int next_identifier = 0;

	std::vector<GameObject*> loaded_gameobjects;
	std::vector<Component*>  loaded_components;

	while (bytes_copied < buffer_size)
	{
		bytes_to_copy = sizeof(int);
		memcpy(&next_identifier, cursor, bytes_to_copy);
		cursor += bytes_to_copy;
		bytes_copied += bytes_to_copy;

		switch (next_identifier)
		{
		case GAMEOBJECTIDENTIFIER:
			{
			GameObject* new_gameobject = new GameObject(nullptr);
			int bytes_advanced = 0;
			new_gameobject->Load(buffer_data, cursor, bytes_advanced);
			cursor += bytes_advanced;
			bytes_copied += bytes_advanced;
			loaded_gameobjects.push_back(new_gameobject);
			if (new_gameobject->isstatic)
				static_gameobjects.push_back(new_gameobject);
			else
				dynamic_gameobjects.push_back(new_gameobject);
			}
			break;
		case COMPONENTIDENTIFIER:
			{
			COMP_TYPE new_comp_type = COMP_TYPE::COMP_UNKNOWN;
			bytes_to_copy = sizeof(COMP_TYPE);
			memcpy(&new_comp_type, cursor, bytes_to_copy);
			cursor += bytes_to_copy;
			bytes_copied += bytes_to_copy;
			
			Component* new_component = ComponentToLoad(new_comp_type);
			int bytes_advanced = 0;
			new_component->Load(buffer_data, cursor, bytes_advanced);
			cursor += bytes_advanced;
			bytes_copied += bytes_advanced;
			loaded_components.push_back(new_component);
			}
			break;
		case ENDFILEIDENTIFIER:
			LOGC("File loaded from %s", file_path);
			break;
		default:
			LOGC("File seems corrupted");
			break;
		}
	}

	for (uint i = 0; i < loaded_components.size(); i++)
	{
		for (uint j = 0; j < loaded_gameobjects.size(); j++)
		{
			if (loaded_gameobjects[j]->UUID == loaded_components[i]->GetParentUUID())
			{
				loaded_gameobjects[j]->AddComponent(loaded_components[i], true);
				if (loaded_components[i]->GetType() == COMP_CAMERA && ((Camera*)loaded_components[i])->IsActive())
					App->camera->SetMainCamera((Camera*)loaded_components[i], true);
			}
		}
	}

	for (uint i = 0; i < loaded_gameobjects.size(); i++)
	{
		for (uint j = 0; j < loaded_gameobjects.size(); j++)
		{
			if (loaded_gameobjects[j]->UUID == loaded_gameobjects[i]->parent_UUID)
				loaded_gameobjects[j]->AddChildren(loaded_gameobjects[i]);
		}
	}

	GameObject* new_root = nullptr;
	for (uint i = 0; i < loaded_gameobjects.size(); i++)
	{
		if (loaded_gameobjects[i]->parent == nullptr)
		{
			
			root = loaded_gameobjects[i];
			break;
		}
	}

}

Component * ModuleSceneIntro::ComponentToLoad(COMP_TYPE new_comp_type)
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
		ret = new Material();
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
