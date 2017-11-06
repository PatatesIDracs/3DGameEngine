#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"

#include "GameObject.h"
#include "Camera.h"
#include "Transform.h"
#include "Mesh.h"
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
	render_camera_test = new Camera(camera, true);
	render_camera_test->SetFrustumViewAngle();
	camera->AddComponent(render_camera_test);
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOGC("Unloading Intro scene");

	if (root != nullptr) delete root;

	return true;
}

void ModuleSceneIntro::Draw()
{
	if (render_camera_test != nullptr) {
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
			}

			// Clear possible Binded buffers
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glPopMatrix();
		}
		render_this.clear();
	}
	
}


// Update
update_status ModuleSceneIntro::Update(float dt)
{

	Primitive a;
	a.axis = true;
	a.Render();
		
	oldPlane p(0, 1, 0, 0);
	if (App->renderer3D->show_grid) p.Render();

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		LookAtScene();
	}

	//Root should never be nullptr but check it just in case
	if (root != nullptr) {
		root->Update();

		// Add Objcts to Octree Test
		CheckDynamicGameObjectsState();
		// Get Meshes to Render;
		/*if (render_camera_test != nullptr) {
			render_camera_test->GetFrustumGameObjecs(root, render_this);
		}*/
		CollectCandidates(); 
		scene_octree.Draw(3.0f, float4(0.25f, 1.00f, 0.00f, 1.00f));
	}
	
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
	std::vector<GameObject*> candidates;
	scene_octree.Inersect(candidates, this->render_camera_test->GetFrustum());

	for (uint i = 0; i < candidates.size(); i++) {
		render_this.push_back((MeshRenderer*)(candidates[i]->FindFirstComponent(COMP_MESHRENDERER)));
	}
}

bool ModuleSceneIntro::AddGameObjectToOctree(const GameObject* object)
{
	if (object != nullptr && object->isstatic) {
		if (scene_octree.Insert((GameObject*)object, object->boundary_box)) {
			/*for (uint i = 0; i < object->children.size(); i++) {
			AddGameObjectToOctree(object->children[i]);
			}*/
			return true;
		}
	}
	return false;
}


void ModuleSceneIntro::CheckDynamicGameObjectsState()
{
	GameObject* object = nullptr;
	std::list<GameObject*>::const_iterator item = dynamic_gameobjects.begin();
	while (item != dynamic_gameobjects.end())
	{
		object = item._Ptr->_Myval;
		item++;
		if (object->isstatic) {
			dynamic_gameobjects.remove(object);
			static_gameobjects.push_back(object);
			AddGameObjectToOctree(object);
		}
	}
}

void ModuleSceneIntro::CheckStaticGameObjectsState()
{
	GameObject* object = nullptr;
	std::list<GameObject*>::const_iterator item = static_gameobjects.begin();
	for (; item != static_gameobjects.end(); item++)
	{
		object = item._Ptr->_Myval;
		if (!object->isstatic) {
			static_gameobjects.remove(object);
			dynamic_gameobjects.push_back(object);
			// TODO----------------
			//RemoveGameObjectFromOctree(object);
		}
	}
}

void ModuleSceneIntro::LookAtScene() const
{
	if (root != nullptr)
	{
		math::AABB box = root->GetBoundaryBox();
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
