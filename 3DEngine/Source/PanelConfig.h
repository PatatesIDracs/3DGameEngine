#ifndef __PANELCONFIG_H__
#define __PANELCONFIG_H__

#include "Panel.h"
#include <list>

class Module;

class PanelConfig : public Panel
{
public:
	PanelConfig();
	~PanelConfig();

	void Draw() override;

private:
	void DrawAppConfig();
	void DrawHardware();
private:
	const std::list<Module*>* module_list = nullptr;

	//App names and capped frames option
	char str_name[512];
	char str_org[512];
	bool uncapped_fps = true;

	//Hardware and software info (so we don't have to use the functions every update)
	//Assimp version
	int assimp_major;
	int assimp_minor;
	int assimp_revision;

	//Devil version
	int devil_version = 0;

	//CPU info
	int		cpu_cores = 0;
	int		cpu_cache_size = 0;
	float	ram = 0;
	bool	has_3Dnow = false;
	bool	has_AVX = false;
	bool	has_AVX2 = false;
	bool	has_AltiVec = false;
	bool	has_MMX = false;
	bool	has_RDTSC = false;
	bool	has_SSE = false;
	bool	has_SSE2 = false;
	bool	has_SSE3 = false;
	bool	has_SSE41 = false;
	bool	has_SSE42 = false;
};

#endif // !__PANELCONFIG_H__
