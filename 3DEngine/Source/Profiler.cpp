#include "Profiler.h"

Profiler::Profiler()
{
	std::vector<float> newvect(size);
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
	if (is_paused)return false;
	
	clock.Start();
	return true;
}

void Profiler::AddTimeToFrame()
{
	if (is_paused)return;
	
	if (curren_function == fnames_size)
	{
		curren_function = 0;
		profiler_timeline[curren_function][59] = clock.Read();
		for (int function = 0; function < fnames_size; function++)
		{
			for (int count = 0; count < size - 1; count++)
			{
				profiler_timeline[function][count] = profiler_timeline[function][count + 1];
			}
		}
	
	}	
	else profiler_timeline[curren_function][59] = clock.Read();

	curren_function++;
}

void Profiler::PauseRecord()
{
	is_paused = !is_paused;
	curren_function = 0;
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
	std::vector<float> newvector(size);
	profiler_timeline.push_back(newvector);
	function_names.push_back((char*)function_name);
	fnames_size++;
}

std::vector<float>* Profiler::GetFunctionTimeline(const char * function_name)
{
	for (int count = 0; count < fnames_size; count++)
	{
		if (strcmp(function_names[count], function_name) == 0)
		{
			return &(profiler_timeline[count]);
		}
	}
	return &profiler_timeline[0];
}

std::vector<char*>* Profiler::GetFunctionNames()
{
	return &function_names;
}
