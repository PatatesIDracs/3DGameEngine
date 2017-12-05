#ifndef __PANELPROFILER_H__
#define __PANELPROFILER_H__

#include "Panel.h"
#include <vector>

class Profiler;

class PanelProfiler : public Panel
{
public:
	PanelProfiler();
	~PanelProfiler();

	void Draw() override;

private:
	bool recordpaused = true;
	int current_module = 0;
	const std::vector<Profiler*>* app_profiler = nullptr;
};

#endif // !__PANELPROFILER_H__