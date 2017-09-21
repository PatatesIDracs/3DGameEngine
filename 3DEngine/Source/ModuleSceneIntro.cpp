#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"

#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_sdl_gl3.h"

#include "Glew\include\glew.h"

//#include "Math.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOGC("Loading Intro assets");
	bool ret = true;

	glewInit();
	ImGui_ImplSdlGL3_Init(App->window->window);
	ImGuiIO& io = ImGui::GetIO();

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	vec origin = { 0, 0, 0 };
	col_test_BodyA = new Sphere(origin, 1.0f);
	origin = { 0,1,0 };
	col_test_BodyB = new Sphere(origin, 1.5f);
	origin = { 2,2,0 };
	col_test_BodyC = new Sphere(origin, 1.0f);

	random_test = false;
	collision_test = false;


	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOGC("Unloading Intro scene");

	delete col_test_BodyA;
	delete col_test_BodyB;
	delete col_test_BodyC;

	ImGui_ImplSdlGL3_Shutdown();

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	
	//Temp Imgui stuff-----------------

	ImGui_ImplSdlGL3_NewFrame(App->window->window);

	//Open a Gui window
	ImGui::BeginMainMenuBar();

	//Buttons to close the app and show the test window
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Show Demo")) showtestwindow = !showtestwindow;
	

		//Interrupt update and close the app
		if (ImGui::MenuItem("close app")) return UPDATE_STOP;


		ImGui::EndMenu();
	}


	if (showtestwindow) ImGui::ShowTestWindow();

	ImGui::EndMainMenuBar();

	//Testing out some MathGeoLib Features
	ImGui::Begin("MathGeoLib Tests Window");
	{
		if (ImGui::TreeNode("Algorithm Test"))
		{
			ImGui::Text("Generate:");
			ImGui::BulletText("Random Float [0,1]");
			ImGui::BulletText("Random Int");
			//Random Numbers Generator Test
			if (ImGui::Button("LCG Number Randomizer"))
			{
				LCG randomLCG;
				float_rand_test = randomLCG.Float();
				int_rand_test = randomLCG.Int();
				random_test = true;
			}
			if (random_test)
			{
				ImGui::Text("\n\tFloat:\t%.2f\n\tInt:  \t%d\n", float_rand_test, int_rand_test);
			}
			ImGui::TreePop();
		}
		ImGui::Separator();
		//Matrix Multiplication Test
		if (ImGui::TreeNode("Math Test"))
		{
			ImGui::Text("Calculate Matrix C = A*B\n A:\n1 2 1\n2,1,2\n1,2,1\n\n B:\n1 1 1\n2 2 2\n3 3 3");
			
			if (ImGui::Button("Test Matrix Multiplication"))
			{
				float3x3 mat_A(1, 2, 1, 2, 1, 2, 1, 2, 1);
				float3x3 mat_B(1, 1, 1, 2, 2, 2, 3, 3, 3);

				float3x3 mat_C = mat_A*mat_B;
				LOGC("Mat_A x Mat_B = [%.2f, %.2f, %.2f; %.2f, %.2f, %.2f; %.2f, %.2f, %.2f] ", mat_C.At(0, 0), mat_C.At(0, 1), mat_C.At(0, 2),
					mat_C.At(1, 0), mat_C.At(1, 1), mat_C.At(1, 2), mat_C.At(2, 0), mat_C.At(2, 1), mat_C.At(2, 2));
			}
			ImGui::TreePop();
		}
		ImGui::Separator();
		//Collision Test
		if (ImGui::TreeNode("Geometry Test"))
		{
			ImGui::Text("Intersection Between Spheres:");
			ImGui::BulletText("Sphere 1:\nposition (0,0,0), rad = 1.0");
			ImGui::BulletText("Sphere 2:\nposition (0,1,0), rad = 1.5");
			ImGui::BulletText("Sphere 3:\nposition (2,2,0), rad = 1.0");
			
			if (ImGui::Button("Sphere 1 vs Sphere 2"))
			{
				collision_test = false;
				if(col_test_BodyA->Intersects((const Sphere)*col_test_BodyB))
				{
					collision_test = true;
				}
			}
			if (ImGui::Button("Sphere 1 vs Sphere 3"))
			{
				collision_test = false;
				if (col_test_BodyA->Intersects((const Sphere)*col_test_BodyC))
				{
					collision_test = true;
				}
			}
			if (collision_test) ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 0.5f), "\n\tCollision Success");
			else ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 0.5f), "\n\tCollision Fail");
			ImGui::TreePop();
		}

	}
	ImGui::End();

	


	//Matrix Multiplication
	float3x3 mat_A(1, 2, 1, 2, 1, 2, 1, 2, 1);
	float3x3 mat_B(1, 1, 1, 2, 2, 2, 3, 3, 3);

	float3x3 mat_C = mat_A*mat_B;



	//Rendre the UI
	ImGui::Render();

	//-----------------------------------

	oldPlane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	LOGC("Hit!");
}

