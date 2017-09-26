#include "Profiler.h"

Profiler::Profiler()
{
	std::vector<int> newvect(60);
	profiler_timeline.push_back(newvect);
}

Profiler::~Profiler()
{
	for (int i = 0; i < profiler_timeline.size(); i++)
	{
		profiler_timeline[i].clear();
	}
	profiler_timeline.clear();

	// Need to delete chars?
	function_names.clear();
}

bool Profiler::StartTimer()
{
	if (!is_recording)return false;
	
	clock.Start();
	return true;
}

void Profiler::AddTimeToFrame()
{
	if (!is_recording)return;
	
	curren_function++;
	if (curren_function == fnames_size)
	{
		curren_frame++;
		curren_function = 0;
	}

	if (curren_frame == size)
	{
		is_recording = false;
		return;
	}	

	int value = clock.Read();
	profiler_timeline[curren_function][curren_frame] = value;
}

/*Passar String*/
// Record Frames Up to 300 Frames ---------------
void Profiler::StartRecording(int seconds, int framerate)
{
	size = seconds*framerate;
	curren_frame = 0;
	curren_function = 0;
	fnames_size = 0;
	loop_closed = false;
	is_recording = true;

	for (int i = 0; i < profiler_timeline.size(); i++)
	{
		profiler_timeline[i].clear();
	}
	profiler_timeline.clear();
	function_names.clear();

}

bool Profiler::CheckState()
{
	return is_recording;
}


void Profiler::SetTitle(const char * function_name)
{
	if (loop_closed) return;

	fnames_size = function_names.size();
	for (int count = 0; count < fnames_size; count++)
	{
		if (strcmp(function_names[count], function_name) == 0)
		{
			loop_closed = true;
			return;
		}
	}
	std::vector<int> newvector(size);
	profiler_timeline.push_back(newvector);
	function_names.push_back((char*)function_name);
	fnames_size++;
}

std::vector<int>* Profiler::GetFunctionTimeline(const char * function_name)
{
	for (int count = 0; count < fnames_size; count++)
	{
		if (strcmp(function_names[count], function_name) == 0)
		{
			return &profiler_timeline[count];
		}
	}
	return &profiler_timeline[0];
}
