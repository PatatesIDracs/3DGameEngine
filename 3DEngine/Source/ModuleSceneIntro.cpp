#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"

#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_sdl_gl3.h"

#include "Glew\include\glew.h"

#include "Devil\include\il.h"
#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"

#pragma comment( lib, "Devil/libx86/DevIL.lib" )
#pragma comment( lib, "Devil/libx86/ILU.lib" )
#pragma comment( lib, "Devil/libx86/ILUT.lib" )


//#include "Math.h"
#include "NewPrimitives.h"




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

	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	vec origin = { 0, 1, 0 };
	col_test_BodyA = new oldSphere(1.0f);
	col_test_BodyA->SetPos(-5,1,0);

	origin = { -5,2,0 };
	col_test_BodyB = new Sphere(origin, 1.5f);
	origin = { -7,2,0 };
	col_test_BodyC = new Sphere(origin, 1.0f);

	random_test = false;
	collision_test = false;

	GLubyte checkImage[128][128][4];
	for (int i = 0; i < 128; i++) {
		for (int j = 0; j < 128; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	lennaTest = ilutGLLoadImage("../Game/Lenna.png");
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &ImageName);
	glBindTexture(GL_TEXTURE_2D, ImageName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128,	0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOGC("Unloading Intro scene");

	delete col_test_BodyA;
	delete col_test_BodyB;
	delete col_test_BodyC;

	//ImGui_ImplSdlGL3_Shutdown();
	ilShutDown();

	return true;
}


// Update
update_status ModuleSceneIntro::Update(float dt)
{

	oldPlane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();
	

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::Draw()
{
	
	GLfloat vertices[] = { -1.f, 2.f, 1.f,		-1.f, 0.f, 1.f,		1.f, 0.f, 1.f,
							-1.f, 2.f, 1.f,		1.f, 0.f, 1.f,		1.f, 2.f, 1.f,
							1.f, 2.f, 1.f,		1.f, 0.f, 1.f,		1.f, 0.f, -1.f,
							1.f, 2.f, 1.f,		1.f, 0.f, -1.f,		1.f, 2.f, -1.f,
							1.f, 2.f, -1.f,		1.f, 0.f, -1.f,		-1.f, 0.f, -1.f,
							1.f, 2.f, -1.f,		-1.f, 0.f, -1.f,	-1.f, 2.f, -1.f,
							-1.f, 2.f, -1.f,	-1.f, 0.f, -1.f,	-1.f, 0.f, 1.f,
							-1.f, 2.f, -1.f,	-1.f, 0.f, 1.f,		-1.f, 2.f, 1.f,
							-1.f, 2.f, 1.f,		1.f, 2.f, 1.f,		1.f, 2.f, -1.f,
							-1.f, 2.f, 1.f,		1.f, 2.f, -1.f,		-1.f, 2.f, -1.f,
							-1.f, 0.f, 1.f,		-1.f, 0.f, -1.f,	1.f, 0.f, -1.f,
							-1.f, 0.f, 1.f,		1.f, 0.f, -1.f,		1.f, 0.f, 1.f };

	GLfloat uniquevertices[] = {5,0,0,
								5,1,0,
								6,0,0,
								6,1,0,
								6,1,-1,
								6,0,-1,
								5,1,-1,
								5,0,-1};

	GLubyte indices[] = {0,2,1,		1,2,3,		//Front
						3,2,5,		3,5,4,		//Right	
						1,3,4,		1,4,6,		//Top
						4,5,7,		4,7,6,		//Back
						7,0,1,		7,1,6,		//Left	
						7,2,0,		7,5,2};		//Bottom

	GLfloat uv[] = {0.f,1.f,	0.f,0.f,	1.f,0.f,
					0.f,1.f,	1.f,0.f,	1.f,1.f,
					0.f,1.f,	0.f,0.f,	1.f,0.f,
					0.f,1.f,	1.f,0.f,	1.f,1.f,
					0.f,1.f,	0.f,0.f,	1.f,0.f,
					0.f,1.f,	1.f,0.f,	1.f,1.f,
					0.f,1.f,	0.f,0.f,	1.f,0.f,
					0.f,1.f,	1.f,0.f,	1.f,1.f,
					0.f,1.f,	0.f,0.f,	1.f,0.f,
					0.f,1.f,	1.f,0.f,	1.f,1.f,
					0.f,1.f,	0.f,0.f,	1.f,0.f,
					0.f,1.f,	1.f,0.f,	1.f,1.f};
	


	//Direct mode cube -- For texture testing
	/*glEnable(GL_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, lennaTest);
	glBegin(GL_TRIANGLES);
	for (int i = 0, j = 0; i < 36*3; j+=2, i += 3)
	{
		glTexCoord2f(uv[j], uv[j + 1]);
		glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
	}
	glEnd();
	glDisable(GL_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);*/
	
	//Load textures
	glGenBuffers(1, (GLuint*)&uv_id);
	glBindBuffer(GL_ARRAY_BUFFER, uv_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24 * 3, &uv[0], GL_STATIC_DRAW);



	
	glGenBuffers(1, (GLuint*)&cube_id);
	glBindBuffer(GL_ARRAY_BUFFER, cube_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, &vertices[0], GL_STATIC_DRAW);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, lennaTest);
	glBindBuffer(GL_ARRAY_BUFFER, uv_id);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, cube_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);



	glGenBuffers(1, (GLuint*)&unique_cube_id);
	glBindBuffer(GL_ARRAY_BUFFER, unique_cube_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24 * 3, &uniquevertices[0], GL_STATIC_DRAW);
	
	glGenBuffers(1, (GLuint*)&indices_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 36, &indices[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, unique_cube_id);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	// draw a cube
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, NULL);

	// deactivate vertex arrays after drawing
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glDeleteBuffers(1, &cube_id);
	glDeleteBuffers(1, &indices_id);
	glDeleteBuffers(1, &unique_cube_id);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	col_test_BodyA->Render(App->renderer3D->face_normals);
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	LOGC("Hit!");
}

