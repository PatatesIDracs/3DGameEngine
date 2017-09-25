#ifndef __PROFILER_H__
#define __PROFILER_H__

#include "Timer.h"
#include <vector>

#define RECORD_LIMIT 300

class Profiler
{
public:
	Profiler();
	Profiler(int size);
	~Profiler();

	void StartTimer();

	// Add Time to a current frame;
	void AddNewFrame();
	void AddTimeToFrame();

	void StartRecording(int size);
	void StopRecording(int size);


private:
	
	Timer clock;

	int	curren_frame = 0;
	int	size = 0;
	int increment_factor = 1;

	bool first_frame = true;
	bool do_record = false;
	
	std::vector<float> profiler_timeline;
};

#endif // !__PROFILER_H__
