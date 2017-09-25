#include "Profiler.h"

Profiler::Profiler()
{
}

Profiler::Profiler(int size)
{
	profiler_timeline.reserve(size);
	this->size = size;
	for (int i = 0; i < size; i++)
		profiler_timeline.push_back(0.0f);
	// Need to fill all values with 0?
	int x = 0;
}

Profiler::~Profiler()
{
	profiler_timeline.clear();
}

void Profiler::StartTimer()
{
	clock.Start();
}

void Profiler::AddNewFrame()
{
	if (!first_frame) curren_frame++;
	else first_frame = !first_frame;

	if (do_record && curren_frame == size)
	{
		size += increment_factor;
		profiler_timeline.resize(size);
	}

	if(curren_frame == size)
	{
		for (int i = 0; i < size - 1; i++)
		{
			profiler_timeline[i] = profiler_timeline[i + 1];
		}
		curren_frame--;
		profiler_timeline[curren_frame] = 0.0f;
	}
	
	AddTimeToFrame();
}

void Profiler::AddTimeToFrame()
{
	float value = clock.Read();

	profiler_timeline[curren_frame] += value;
}


// Record Frames Up to 300 Frames ---------------
void Profiler::StartRecording(int resize)
{
	increment_factor = resize;
	do_record = true;
}

// Keep Last "resize" values and free space; 
void Profiler::StopRecording(int resize)
{
	if (curren_frame > resize)
	{
		int remove = size - resize-1;
		for (int i = 0; i < resize; i++)
		{
			profiler_timeline[i] = profiler_timeline[remove + i];
		}
		curren_frame = resize-1;
	}
	size = resize;
	profiler_timeline.resize(resize);
}
