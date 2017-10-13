#include "Application.h"
#include "ModuleLoadFBX.h"

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

#include "3DModel.h"

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

	std::string	file_name = file;
		
	const aiScene* scene = aiImportFile(file_name.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		App->scene_intro->ClearBody3DArray();
		
		// Set Scene Transform
		aiMatrix4x4 rot = scene->mRootNode->mTransformation;	
		mat4x4 transform = mat4x4(rot.a1, rot.b1, rot.c1, rot.d1, rot.a2, rot.b2, rot.c2, rot.d2, rot.a3, rot.b3, rot.c3, rot.d3, rot.a4, rot.b4, rot.c4, rot.d4);
	
		// Search Absolute Path 
		std::string temp = "\\";
		while (file_name[file_name.size()-1] != temp[temp.size()-1] || temp.size() == 0)
		{
			file_name.pop_back();
		}
		temp.clear();


		// Loat Textures
		aiString path;
		std::vector<int> textures;
		std::string directory = JOPE_DATA_DIRECTORY JOPE_TEXTURE_FOLDER;
		std::string fullpath = "";
		for (uint i = 0; i < scene->mNumMaterials; i++)
		{
			if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				int tex_id = 0;
				fullpath = file_name + path.data;
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
					App->scene_intro->SetTexSize(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
				}
				else LOGC("Unable to load %s texture", path.data);

				textures.push_back(tex_id);	
			}
		}
		directory.clear();
		fullpath.clear();
		path.Clear();
		
		
		// Iterate Meshes array
		uint n_meshes = scene->mNumMeshes;
		for (uint count = 0; count < n_meshes; count++)
		{
			BodyMesh* mesh = new BodyMesh;
			const aiMesh* new_mesh = scene->mMeshes[count];
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
				mesh->id_texture = textures[new_mesh->mMaterialIndex];
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

			// Add new Body to Scene
			App->scene_intro->AddBody3D(new Body3D(mesh, transform));
		}
		
		aiReleaseImport(scene);
		textures.clear();
		
	}
	else
	{
		// Try to Load Droped file as Image
		int tex_id = ilutGLLoadImage((char*)file_name.c_str());
		if (tex_id != 0)
		{
			App->scene_intro->ChangeTexture(tex_id);
			App->scene_intro->SetTexSize(ilGetInteger(IL_IMAGE_WIDTH),ilGetInteger(IL_IMAGE_HEIGHT));
		}
		else LOGC("Error loading file %s", file_name.c_str());
	}

	file_name.clear();
}

int ModuleLoadFBX::GetDevilVersion()
{
	return IL_VERSION;
}
