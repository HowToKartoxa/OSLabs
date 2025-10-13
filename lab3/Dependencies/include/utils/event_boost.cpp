#include <utils/event_boost.h>

Event::Event() : status(false) {}

Event::Event(bool _status) : status(_status) {}

void Event::Set() 
{
	boost::unique_lock<boost::mutex> lock(mutex);
	status = true;
	condition_var.notify_all();
}

void Event::Reset() 
{
	boost::unique_lock<boost::mutex> lock(mutex);
	status = false;
}

void Event::Wait() 
{
	boost::unique_lock<boost::mutex> lock(mutex);
	while (!status) 
	{
		condition_var.wait(lock);
	}
}

MultiEvent::MultiEvent() : status(0) {}

MultiEvent::MultiEvent(size_t count) : status(count) {}

MultiEvent::MultiEvent(std::vector<bool> _status) : status(_status) {}

void MultiEvent::Set(size_t index)
{
	boost::unique_lock<boost::mutex> lock(mutex);
	if (index >= status.size()) throw std::out_of_range("event index out of range");
	status[index] = true;
	condition_var.notify_all();
}

void MultiEvent::SetAll() 
{
	boost::unique_lock<boost::mutex> lock(mutex);
	for (size_t i = 0; i < status.size(); i++) 
	{
		status[i] = true;
	}
	condition_var.notify_all();
}

void MultiEvent::Reset(size_t index) 
{
	boost::unique_lock<boost::mutex> lock(mutex);
	if (index >= status.size()) throw std::out_of_range("event index out of range");
	status[index] = false;
}

void MultiEvent::ResetAll() 
{
	boost::unique_lock<boost::mutex> lock(mutex);
	for (size_t i = 0; i < status.size(); i++)
	{
		status[i] = false;
	}
}

size_t MultiEvent::WaitOne() 
{
	boost::unique_lock<boost::mutex> lock(mutex);
	size_t res;
	bool found = false;
	for (size_t i = 0; i < status.size(); i++) 
	{
		if (status[i] == true) 
		{
			res = i;
			found = true;
			break;
		}
	}
	while (!found) 
	{
		condition_var.wait(lock);
		for (size_t i = 0; i < status.size(); i++)
		{
			if (status[i] == true)
			{
				res = i;
				found = true;
				break;
			}
		}
	}
	return res;
}

void MultiEvent::WaitSpecific(size_t index) 
{
	boost::unique_lock<boost::mutex> lock(mutex);
	if (index >= status.size()) throw std::out_of_range("event index out of range");
	while (!status[index])
	{
		condition_var.wait(lock);
	}
}

void MultiEvent::WaitAll() 
{
	boost::unique_lock<boost::mutex> lock(mutex);
	bool found = false;
	for (size_t i = 0; i < status.size(); i++)
	{
		if (status[i] == false)
		{
			found = true;
			break;
		}
	}
	while (found)
	{
		condition_var.wait(lock);
		found = false;
		for (size_t i = 0; i < status.size(); i++)
		{
			if (status[i] == false)
			{
				found = true;
				break;
			}
		}
	}
}