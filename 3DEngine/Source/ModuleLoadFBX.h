#ifndef __MODULE_LOAD_FBX__
#define __MODULE_LOAD_FBX__

#include "Module.h"

class ModuleLoadFBX : public Module
{
public:
	ModuleLoadFBX(Application* app, bool start_enabled = true);
	~ModuleLoadFBX();

	bool Init();

	bool Start();

	update_status PreUpdate(float dt);

	update_status Update(float dt);

	update_status PostUpdate(float dt);

	bool CleanUp();

	void SetUpFile(const char* file_name);
	bool LoadFile();

private:

	bool		file_detected = false;
	std::string file_name;


};

#endif // !__MODULE_LOAD_FBX__
