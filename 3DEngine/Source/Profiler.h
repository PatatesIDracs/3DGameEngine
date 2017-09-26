#ifndef __PROFILER_H__
#define __PROFILER_H__

#include "Timer.h"
#include <vector>

class Profiler
{
public:
	Profiler();
	~Profiler();

	bool StartTimer();

	// Add Time to a current frame;
	void AddTimeToFrame();

	void StartRecording(int seconds = 5, int framerate = 60);
	
	// Return true if Profiler is Recording 
	bool CheckState();

	// Set Up Functions
	// Set new function to record
	void SetTitle(const char* function_name);

	// Get Timeline;
	std::vector<int>* GetFunctionTimeline(const char* function_name);
	

private:
	
	Timer clock;

	int	curren_frame = 0;
	int curren_function = 0;
	int	size = 0;
	int fnames_size = 0;

	bool is_recording = false;
	bool loop_closed = false;
	
	std::vector<std::vector<int>> profiler_timeline;
	std::vector<char*> function_names;
};

#endif // !__PROFILER_H__
