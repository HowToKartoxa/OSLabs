#ifndef _MARKER_PROCEDURES
#define _MARKER_PROCEDURES

#include <utils/event_boost.h>

#include <boost/thread.hpp>

#include <windows.h>

DWORD WINAPI marker(LPVOID);

void marker_boost
(
	int*,
	size_t,
	unsigned short,
	boost::mutex*,
	boost::mutex*,
	Event*,
	MultiEvent*,
	MultiEvent*
);

#endif