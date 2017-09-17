#ifndef __j1PERFTIMER_H__
#define __j1PERFTIMER_H__

typedef unsigned __int64 uint64;
class j1PerfTimer
{
public:

	// Constructor
	j1PerfTimer();

	void Start();
	double ReadMs() const;
	uint64 ReadTicks() const;

private:
	uint64	started_at;
	static uint64 frequency;
};

#endif //__j1PERFTIMER_H__