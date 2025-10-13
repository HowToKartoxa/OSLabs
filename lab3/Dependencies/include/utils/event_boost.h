#ifndef _EVENT_BOOST
#define _EVENT_BOOST

#include <boost/thread.hpp>
#include <vector>

class Event 
{
private:
	boost::mutex mutex;
	boost::condition_variable condition_var;
	bool status;

public:
	Event();
	Event(bool);

	void Set();
	void Reset();
	void Wait();
};

class MultiEvent 
{
private:
	boost::mutex mutex;
	boost::condition_variable condition_var;
	std::vector<bool> status;

public:
	MultiEvent();
	MultiEvent(size_t);
	MultiEvent(std::vector<bool>);

	void Set(size_t);
	void SetAll();
	void Reset(size_t);
	void ResetAll();

	size_t WaitOne();
	void WaitSpecific(size_t);
	void WaitAll();
};

#endif
