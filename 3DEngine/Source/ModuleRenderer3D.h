#ifndef __MODULERENDERER_H__
#define __MODULERENDERER_H__



#include "Module.h"
#include "glmath.h"
#include "Light.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

	// JSON Save/Load Configuration
	void LoadModuleConfig(Config_Json& config);
	void SaveModuleConfig(Config_Json& config);

	void DrawConfig();

private:

	void CheckConfig();

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;


private:

	//Render options 
	bool depth_test = false;
	bool cull_face = false;
	int	 cull_face_mode = 0;
	bool lighting = false;
	bool color_material = false;
	bool texture_2d = false;
	bool wireframe = false;

	float LightModelAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };


};

#endif // !__MODULERENDERER_H__