#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"

#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"

#include "Glew\include\glew.h"



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
	octree_limit.SetFromCenterAndSize(float3(0.0f, 0.0f, 0.0f), float3(512.0f, 512.0f, 512.0f));
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
		// Get Meshes to Render;
		if (render_camera_test != nullptr) {
			render_camera_test->GetFrustumGameObjecs(root, render_this);
		}
	}

	/* // Add Objcts to Octree Test
	if (!scene_added && current_object->isstatic) {
	scene_added = AddGameObjectToOctree(current_object);
	}
	*/

	return UPDATE_CONTINUE;
}

GameObject * ModuleSceneIntro::CreateNewGameObject(const char* name, GameObject* parent)
{
	GameObject* ret = nullptr;

	if (parent == nullptr)
		ret = new GameObject(root, name);
	else
		ret = new GameObject(parent, name);

	current_object = ret;
	return ret;
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
	LOGC("Buffer size is %i", buffer_size);

	//TODO: Create the buffer and cursor


	//TODO: Pass the buffer and cursor and go saving data
	root->Save();

	//TODO: Put and ending signature to the file to check if it could be corrupted
}

void ModuleSceneIntro::LoadScene(const char * file_path)
{
	LOGC("Loading the scence from %s", file_path);
}
