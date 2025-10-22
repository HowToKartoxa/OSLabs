#define BOOST_TEST_MODULE main_tests

#include <utils/input_parsing.h>

#include <boost/test/included/unit_test.hpp>
#include <boost/regex.hpp>

struct CoutRedirect
{
	CoutRedirect(std::streambuf* new_buf) : old_buf(std::cout.rdbuf(new_buf)) {}
	~CoutRedirect()
	{
		std::cout.rdbuf(old_buf);
	}
	void Switch() 
	{
		old_buf = std::cout.rdbuf(old_buf);
	}
private:
	std::streambuf* old_buf;
};

#if defined (USE_WINAPI)

#include <utils/marker_parameter_data.h>
#include <utils/thread_manager.h>
#include <utils/marker_procedures.h>

#include <windows.h>

struct SingleMarkerFixtureWin
{
	MarkerParameterData test_data;

	HANDLE thread;

	DWORD id;

	boost::test_tools::output_test_stream out;

	CoutRedirect redirect;

	const DWORD sleep_for;
	const DWORD wait_for;

	SingleMarkerFixtureWin(size_t size = 40) : 
		sleep_for(30), 
		wait_for(2000),
		out(),
		thread(NULL),
		id(0),
		redirect(out.rdbuf()),
		test_data()
	{
		test_data.array_data = new int[size];
		test_data.array_size = size;
		for (size_t i = 0; i < test_data.array_size; i++)
		{
			test_data.array_data[i] = 0;
		}
		test_data.thread_number = 1;

		test_data.output_mutex = CreateMutexA(NULL, FALSE, "OUTPUT_MUTEX_TEST");
		test_data.array_mutex = CreateMutexA(NULL, FALSE, "ARRAY_MUTEX_TEST");

		test_data.start_threads_event = CreateEventA(NULL, TRUE, FALSE, "START_THREAD_TEST_EVENT");
		test_data.exit_thread_event = CreateEventA(NULL, TRUE, FALSE, "EXIT_THREAD_TEST_EVENT");
		test_data.thread_stopped_event = CreateEventA(NULL, TRUE, FALSE, "THREAD_STOPPED_TEST_EVENT");
	}
	~SingleMarkerFixtureWin()
	{
		CloseHandle(thread);

		CloseHandle(test_data.output_mutex);
		CloseHandle(test_data.array_mutex);

		CloseHandle(test_data.start_threads_event);
		CloseHandle(test_data.exit_thread_event);
		CloseHandle(test_data.thread_stopped_event);

		delete[] test_data.array_data;
	}
};

BOOST_AUTO_TEST_SUITE(marker_tests_single_win)
	
	BOOST_FIXTURE_TEST_CASE
	(
		marker_init, 
		SingleMarkerFixtureWin
	)
	{
		thread = CreateThread(NULL, NULL, marker, reinterpret_cast<LPVOID*>(&test_data), NULL, &id);
		Sleep(sleep_for);
		BOOST_TEST(thread != nullptr);
	}
	
	BOOST_FIXTURE_TEST_CASE
	(
		marker_execute, 
		SingleMarkerFixtureWin
	)
	{
		thread = CreateThread(NULL, NULL, marker, reinterpret_cast<LPVOID*>(&test_data), NULL, &id);
		Sleep(sleep_for);
		PulseEvent(test_data.start_threads_event);
		DWORD wait_res = WaitForSingleObject(test_data.thread_stopped_event, wait_for);
		BOOST_TEST(wait_res == 0);
		SetEvent(test_data.exit_thread_event);
		Sleep(sleep_for);
	}
	
	BOOST_FIXTURE_TEST_CASE
	(
		marker_result,
		SingleMarkerFixtureWin
	)
	{
		thread = CreateThread(NULL, NULL, marker, reinterpret_cast<LPVOID*>(&test_data), NULL, &id);
		Sleep(sleep_for);
		PulseEvent(test_data.start_threads_event);
		WaitForSingleObject(test_data.thread_stopped_event, INFINITE);

		std::string res = out.str();
		std::cout.flush();

		size_t count = 0;
		for (size_t i = 0; i < test_data.array_size; i++) 
		{
			if (test_data.array_data[i]) 
			{
				BOOST_TEST(test_data.array_data[i] == test_data.thread_number);
				count++;
			}
		}

		boost::regex expression("[0-9]+");
		
		boost::sregex_iterator it(res.begin(), res.end(), expression);
		boost::sregex_iterator end;
		
		bool result = it != end;
		BOOST_TEST(result == true);
		BOOST_TEST(it->str() == std::to_string(test_data.thread_number));
		++it;
		result = it != end;
		BOOST_TEST(result);
		BOOST_TEST(it->str() == std::to_string(count));
		++it;
		result = it != end;
		BOOST_TEST(result);
		BOOST_TEST(test_data.array_data[StringToSizeT(it->str())] != 0);

		SetEvent(test_data.exit_thread_event);
		Sleep(sleep_for); 
	}
	
	BOOST_FIXTURE_TEST_CASE
	(
		marker_exit,
		SingleMarkerFixtureWin
	)
	{
		thread = CreateThread(NULL, NULL, marker, reinterpret_cast<LPVOID*>(&test_data), NULL, &id);
		Sleep(sleep_for);
		PulseEvent(test_data.start_threads_event);
		WaitForSingleObject(test_data.thread_stopped_event, INFINITE);

		SetEvent(test_data.exit_thread_event);
		DWORD wait_res = WaitForSingleObject(thread, wait_for);

		BOOST_TEST(wait_res == 0);
		for (size_t i = 0; i < test_data.array_size; i++)
		{
			BOOST_TEST(test_data.array_data[i] == 0);
		}
	}
	
BOOST_AUTO_TEST_SUITE_END()

#elif defined (USE_BOOST)

#include <utils/event_boost.h>
#include <utils/thread_manager_boost.h>
#include <utils/marker_procedures.h>

struct SingleMarkerFixtureBoost
{
	boost::thread* thread;

	int* array;
	size_t array_size;
	unsigned short thread_number;

	boost::mutex output_mutex;
	boost::mutex array_mutex;

	Event start_threads_event;

	MultiEvent thread_stopped_event;
	MultiEvent** response_events;

	CoutRedirect redirect;

	boost::test_tools::output_test_stream out;

	const size_t sleep_for;

	SingleMarkerFixtureBoost(size_t size = 40) :
		sleep_for(30),
		out(),
		thread(nullptr),
		redirect(out.rdbuf()),
		output_mutex(),
		array_mutex(),
		start_threads_event(),
		thread_stopped_event(1)
	{
		array_size = size;
		array = new int[array_size];
		for (size_t i = 0; i < array_size; i++)
		{
			array[i] = 0;
		}
		thread_number = 1;
		response_events = new MultiEvent * [1];
		response_events[0] = new MultiEvent(2);
	}
	~SingleMarkerFixtureBoost()
	{
		thread->interrupt();
		delete thread;
		delete[] array;
		delete response_events[0];
		delete response_events;
	}
};

BOOST_AUTO_TEST_SUITE(marker_tests_single_boost)

	BOOST_FIXTURE_TEST_CASE
	(
		marker_init_boost,
		SingleMarkerFixtureBoost
	)
	{
		thread = new boost::thread
		(
			marker_boost,
			array,
			array_size,
			thread_number,
			&output_mutex,
			&array_mutex,
			&start_threads_event,
			&thread_stopped_event,
			response_events
		);
		boost::this_thread::sleep_for(boost::chrono::milliseconds(sleep_for));
		BOOST_TEST(thread->joinable() != false);
	}

	BOOST_FIXTURE_TEST_CASE
	(
		marker_execute_boost,
		SingleMarkerFixtureBoost,
		* boost::unit_test::timeout(3)
	)
	{
		thread = new boost::thread
		(
			marker_boost,
			array,
			array_size,
			thread_number,
			&output_mutex,
			&array_mutex,
			&start_threads_event,
			&thread_stopped_event,
			response_events
		);
		boost::this_thread::sleep_for(boost::chrono::milliseconds(sleep_for));
		start_threads_event.Set();
		thread_stopped_event.WaitAll();
	}
	
	BOOST_FIXTURE_TEST_CASE
	(
		marker_result_boost,
		SingleMarkerFixtureBoost
	)
	{
		thread = new boost::thread
		(
			marker_boost,
			array,
			array_size,
			thread_number,
			&output_mutex,
			&array_mutex,
			&start_threads_event,
			&thread_stopped_event,
			response_events
		);
		boost::this_thread::sleep_for(boost::chrono::milliseconds(sleep_for));
		start_threads_event.Set();
		thread_stopped_event.WaitAll();

		std::string res = out.str();
		std::cout.flush();

		size_t count = 0;
		for (size_t i = 0; i < array_size; i++)
		{
			if (array[i])
			{
				BOOST_TEST(array[i] == thread_number);
				count++;
			}
		}

		boost::regex expression("[0-9]+");
		boost::sregex_iterator it(res.begin(), res.end(), expression);
		boost::sregex_iterator end;

		bool result = it != end;
		BOOST_TEST(result == true);
		BOOST_TEST(it->str() == std::to_string(thread_number));
		++it;
		result = it != end;
		BOOST_TEST(result == true);
		BOOST_TEST(it->str() == std::to_string(count));
		++it;
		result = it != end;
		BOOST_TEST(result == true);
		BOOST_TEST(array[StringToSizeT(it->str())] != 0);

		thread->interrupt();
	}
	
	BOOST_FIXTURE_TEST_CASE
	(
		marker_exit_boost,
		SingleMarkerFixtureBoost
	)
	{
		thread = new boost::thread
		(
			marker_boost,
			array,
			array_size,
			thread_number,
			&output_mutex,
			&array_mutex,
			&start_threads_event,
			&thread_stopped_event,
			response_events
		);

		start_threads_event.Set();
		thread_stopped_event.WaitAll();

		response_events[0]->Set(0);
		thread->join();
		for (size_t i = 0; i < array_size; i++) 
		{
			BOOST_TEST(array[i] == 0);
		}
	}

BOOST_AUTO_TEST_SUITE_END()

#endif
