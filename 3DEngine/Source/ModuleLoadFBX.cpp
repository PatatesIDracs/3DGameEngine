#include "Application.h"
#include "ModuleLoadFBX.h"
#include "ModuleRenderer3D.h"
#include "ModuleSceneIntro.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "Glew\include\glew.h"
#include "Globals.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Devil\include\il.h"
#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"

#include "Math.h"

#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "Material.h"
#include "Mesh.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

#pragma comment( lib, "Devil/libx86/DevIL.lib" )
#pragma comment( lib, "Devil/libx86/ILU.lib" )
#pragma comment( lib, "Devil/libx86/ILUT.lib" )

ModuleLoadFBX::ModuleLoadFBX(Application* app, bool start_enabled) : Module(app, "Assimp", start_enabled)
{}

ModuleLoadFBX::~ModuleLoadFBX(){}

bool ModuleLoadFBX::Init()
{
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	return true;
}

bool ModuleLoadFBX::CleanUp()
{
	ilShutDown();
	return true;
}

void ModuleLoadFBX::LoadFile(const char* file)
{
	if (file == nullptr) return;

	std::string	file_path = file;

	const aiScene* scene = aiImportFile(file_path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
	
		std::string file_name;
		std::string dot = ".";
		std::string slash = "\\";

		for (uint i = 0; (i < file_path.size()) && (file_path[i] != dot[0]) ; i++)
		{
			//Fill the name
			file_name.push_back(file_path[i]);

			//If a slash is met clear the name
			if (file_path[i] == slash[0])
			{
				file_name.clear();
			}
		}

		GameObject* object_parent = App->scene_intro->CreateNewGameObject(file_name.c_str());
	
		aiNode* parent = scene->mRootNode;
		aiMatrix4x4 rot = parent->mTransformation;	
		mat4x4 transform = mat4x4(rot.a1, rot.b1, rot.c1, rot.d1, rot.a2, rot.b2, rot.c2, rot.d2, rot.a3, rot.b3, rot.c3, rot.d3, rot.a4, rot.b4, rot.c4, rot.d4);

		// Create Transform Component from Current Scene Root Node
		Transform* rot_transform = new Transform(object_parent, transform);

		object_parent->AddComponent(rot_transform);
		
		// Search Absolute Path 
		std::string temp = "\\";
		while (file_path[file_path.size() - 1] != temp[temp.size() - 1] || temp.size() == 0)
		{
			file_path.pop_back();
		}
		temp.clear();

		// Loat Textures
		aiString path;
		std::vector<float3> textures;
		std::string directory = JOPE_DATA_DIRECTORY JOPE_TEXTURE_FOLDER;
		std::string fullpath = "";
		for (uint i = 0; i < scene->mNumMaterials; i++)
		{
			if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				int tex_id = 0;
				fullpath = file_path + path.data;
				tex_id = ilutGLLoadImage((char *)fullpath.c_str());
				if (tex_id == 0)
				{
					LOGC("Error: %s texture not found, search directory: %s", path.data, fullpath.c_str());
					fullpath = directory + path.data;
					tex_id = ilutGLLoadImage((char *)fullpath.c_str());
					if (tex_id == 0)
						LOGC("Error: %s texture not found, search directory: %s", path.data, fullpath.c_str());
				}

				if (tex_id != 0)
				{
					LOGC("Loaded %s texture from path: %s", path.data, fullpath.c_str());
				}
				else LOGC("Unable to load %s texture", path.data);

				textures.push_back(float3((float)tex_id,(float) ilGetInteger(IL_IMAGE_WIDTH), (float)ilGetInteger(IL_IMAGE_HEIGHT)));
			}
			else
			{
				LOGC("This mesh doesn't have a diffuse texture");
				textures.push_back(float3(0.f,0.f,0.f));
			}
		}
		directory.clear();
		fullpath.clear();
		path.Clear();

		// Iterate Meshes array
		uint n_meshes = scene->mNumMeshes;
		for (uint count = 0; count < n_meshes; count++)
		{
			const aiMesh* new_mesh = scene->mMeshes[count];

			RenderData*	mesh = new RenderData;
			GameObject* object_child = App->scene_intro->CreateNewGameObject(parent->mChildren[count]->mName.C_Str(), object_parent);
			
			// Add Transform To child
			aiMatrix4x4 crot = parent->mChildren[count]->mTransformation;
			//crot = crot.Transpose()*rot;
			mat4x4 transform = mat4x4(crot.a1, crot.b1, crot.c1, crot.d1, crot.a2, crot.b2, crot.c2, crot.d2, crot.a3, crot.b3, crot.c3, crot.d3, crot.a4, crot.b4, crot.c4, crot.d4);

			object_child->AddComponent(new Transform(object_child, transform));

			mesh->num_vertices = new_mesh->mNumVertices;
			mesh->vertices = new float[new_mesh->mNumVertices * 3];
			memcpy(mesh->vertices, new_mesh->mVertices, sizeof(float) * mesh->num_vertices * 3);
			LOGC("New mesh with %d vertices", mesh->num_vertices);
			
			if (new_mesh->HasFaces())
			{
				mesh->num_indices = new_mesh->mNumFaces * 3;
				mesh->indices = new uint[mesh->num_indices]; // assume each face is a triangle
				for (uint i = 0; i < new_mesh->mNumFaces; ++i)
				{
					if (new_mesh->mFaces[i].mNumIndices != 3)
					{
						LOGC("WARNING, geometry face with != 3 indices! %d", 0);
					}
					else {
						memcpy(&mesh->indices[i * 3], new_mesh->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
			}
			
			// Load Vertex Normals
			if (new_mesh->HasNormals())
			{
				mesh->num_normals = new_mesh->mNumVertices;
				mesh->normals = new float[mesh->num_normals * 3];
				memcpy(mesh->normals, new_mesh->mNormals, sizeof(float) * mesh->num_vertices * 3);
				LOGC("New mesh with %d normals", mesh->num_normals);
			}

			// Load Textures
			if (new_mesh->mTextureCoords[0] != NULL)
			{
				mesh->num_tex_vertices = new_mesh->mNumVertices;
				mesh->tex_vertices = new float[mesh->num_tex_vertices*3];
				memcpy(mesh->tex_vertices, new_mesh->mTextureCoords[0], sizeof(float)* mesh->num_tex_vertices*3);
				LOGC("Texture Coord loaded: %d texture coords", mesh->num_tex_vertices);

				// Set Texture ID
				Material* texture = nullptr;

				if (textures.size() != 0)
					texture = new Material(object_child, textures[new_mesh->mMaterialIndex]);
				else texture = new Material(object_child);
				object_child->AddComponent(texture);
			}

			// Load Vertices and Indices To Buffer and Set ID
			if (mesh->vertices != nullptr)
			{
				glGenBuffers(1, (GLuint*)&mesh->id_vertices);
				glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertices);
				glBufferData(GL_ARRAY_BUFFER, mesh->num_vertices * 3 * sizeof(float), &mesh->vertices[0], GL_STATIC_DRAW);
			}

			if (mesh->normals != nullptr)
			{
				glGenBuffers(1, (GLuint*)&mesh->id_normals);
				glBindBuffer(GL_ARRAY_BUFFER, mesh->id_normals);
				glBufferData(GL_ARRAY_BUFFER, mesh->num_normals * 3 * sizeof(float), &mesh->normals[0], GL_STATIC_DRAW);
			}

			if (mesh->indices != nullptr)
			{
				glGenBuffers(1, (GLuint*)&mesh->id_indices);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_indices);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->num_indices * sizeof(uint), &mesh->indices[0], GL_STATIC_DRAW);
			}

			// Load texture coords buffer
			if (mesh->tex_vertices != nullptr)
			{
				glGenBuffers(1, (GLuint*)&mesh->id_tex_vertices);
				glBindBuffer(GL_ARRAY_BUFFER, mesh->id_tex_vertices);
				glBufferData(GL_ARRAY_BUFFER, mesh->num_tex_vertices * 3 * sizeof(float), &mesh->tex_vertices[0], GL_STATIC_DRAW);
			}

			//Clean Buffer bind
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			object_child->AddComponent(new Mesh(object_child, mesh));
		}

		App->scene_intro->LookAtScene();

		aiReleaseImport(scene);
		textures.clear();
	}
	else
	{
		// Try to Load Droped file as Image
		int tex_id = ilutGLLoadImage((char*)file_path.c_str());
		if (tex_id != 0)
		{
			//App->scene_intro->ChangeTexture(tex_id);
			//App->scene_intro->SetTexSize(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
		}
		else LOGC("Error loading file %s", file_path.c_str());
	}

	file_path.clear();
}
	