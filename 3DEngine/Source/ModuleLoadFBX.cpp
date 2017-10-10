#include "Application.h"
#include "ModuleLoadFBX.h"
#include "ModuleRenderer3D.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "Glew\include\glew.h"
#include "Globals.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Devil\include\il.h"
#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"

#include "3DModel.h"
#include "Math.h"

ModuleLoadFBX::ModuleLoadFBX(Application* app, bool start_enabled) : Module(app, "Assimp", start_enabled)
{}

ModuleLoadFBX::~ModuleLoadFBX(){}


bool ModuleLoadFBX::CleanUp()
{
	file_name.clear();
	return true;
}

void ModuleLoadFBX::SetUpFile(const char * file_name)
{
	if (file_name != nullptr)
	{
		this->file_name = file_name;
		LoadFile();
	}
}

bool ModuleLoadFBX::LoadFile()
{
	const aiScene* scene = aiImportFile(file_name.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		App->renderer3D->ClearBody3DArray();

		// Set Scene Transform
		aiMatrix4x4 rot = scene->mRootNode->mTransformation;	
		aiVector3D scale;
		aiQuaternion q;
		aiVector3D angle;
		aiVector3D position;
		rot.Decompose(scale, q, position);
		//angle = q.GetEuler();
		mat4x4 transform = mat4x4(rot.a1, rot.b1, rot.c1, rot.d1, rot.a2, rot.b2, rot.c2, rot.d2, rot.a3, rot.b3, rot.c3, rot.d3, rot.a4, rot.b4, rot.c4, rot.d4);

		// Loat Textures
		aiString path;
		int texIndex = 0;
		std::vector<int> textures;
		std::string directory = JOPE_DATA_DIRECTORY JOPE_TEXTURE_FOLDER;
		std::string fullpath = "";
		for (uint i = 0; i < scene->mNumMaterials; i++)
		{
			if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path) == AI_SUCCESS)
			{
				fullpath = directory + path.data;
				textures.push_back(ilutGLLoadImage((char *)fullpath.c_str()));
			}
		}
		directory.clear();
		fullpath.clear();
		path.Clear();

		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		uint n_meshes = scene->mNumMeshes;
		for (uint count = 0; count < n_meshes; count++)
		{
			body_mesh mesh;
			const aiMesh* new_mesh = scene->mMeshes[count];
			mesh.num_vertices = new_mesh->mNumVertices;
			mesh.vertices = new float[new_mesh->mNumVertices * 3];
			memcpy(mesh.vertices, new_mesh->mVertices, sizeof(float) * mesh.num_vertices * 3);
			LOGC("New mesh with %d vertices", mesh.num_vertices);
			
			if (new_mesh->HasFaces())
			{
				mesh.num_indices = new_mesh->mNumFaces * 3;
				mesh.indices = new uint[mesh.num_indices]; // assume each face is a triangle
				for (uint i = 0; i < new_mesh->mNumFaces; ++i)
				{
					if (new_mesh->mFaces[i].mNumIndices != 3)
					{
						LOGC("WARNING, geometry face with != 3 indices! %d", 0);
					}
					else {
						memcpy(&mesh.indices[i * 3], new_mesh->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
			}
			
			if (new_mesh->HasNormals())
			{
				mesh.num_normals = new_mesh->mNumVertices;
				mesh.normals = new float[mesh.num_normals * 2];
				memcpy(mesh.normals, new_mesh->mNormals, sizeof(float) * mesh.num_vertices * 2);
				LOGC("New mesh with %d normals", mesh.num_normals);

			}

			// Load Textures
			if (new_mesh->mTextureCoords[0] != NULL)
			{
				mesh.num_tex_vertices = new_mesh->mNumVertices;
				mesh.tex_vertices = new float[mesh.num_tex_vertices*3];
				memcpy(mesh.tex_vertices, new_mesh->mTextureCoords[0], sizeof(float)* mesh.num_tex_vertices*3);
				LOGC("Texture Coord loaded: %d texture coords", mesh.num_tex_vertices);

				mesh.id_texture = textures[new_mesh->mMaterialIndex];
			}

			glGenBuffers(1, (GLuint*)&mesh.id_vertices);
			glBindBuffer(GL_ARRAY_BUFFER, mesh.id_vertices);
			glBufferData(GL_ARRAY_BUFFER, mesh.num_vertices*3*sizeof(float), &mesh.vertices[0], GL_STATIC_DRAW);

			glGenBuffers(1, (GLuint*)&mesh.id_indices);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.id_indices);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.num_indices*sizeof(uint), &mesh.indices[0], GL_STATIC_DRAW);

			//Load texture coords buffer
			glGenBuffers(1, (GLuint*)&mesh.id_tex_vertices);
			glBindBuffer(GL_ARRAY_BUFFER, mesh.id_tex_vertices);
			glBufferData(GL_ARRAY_BUFFER, mesh.num_tex_vertices*3 * sizeof(float), &mesh.tex_vertices[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			App->renderer3D->AddBody3D(new Body3D(mesh, transform));
		}

		aiReleaseImport(scene);
		textures.clear();
	}
	else
		LOGC("Error loading scene %s", file_name.c_str());

	file_name.clear();
	return true;
}
	