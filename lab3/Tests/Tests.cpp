#define BOOST_TEST_MODULE main_tests

#include <utils/input_parsing.h>

#include <boost/test/included/unit_test.hpp>
#include <boost/regex.hpp>

#define USE_BOOST

struct CoutRedirect
{
	CoutRedirect(std::streambuf* new_buf) : old_buf(std::cout.rdbuf(new_buf)) {}
	~CoutRedirect()
	{
		std::cout.rdbuf(old_buf);
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
	MarkerParameterData* test_data;

	HANDLE thread;

	DWORD id;

	CoutRedirect redirect;

	boost::test_tools::output_test_stream out;

	const size_t sleep_for;
	const size_t wait_for;

	SingleMarkerFixtureWin(size_t size = 40) : 
		sleep_for(30), 
		wait_for(100),
		out(),
		redirect(out.rdbuf())
	{
		test_data = new MarkerParameterData();

		test_data->array_data = new int[size];
		test_data->array_size = size;
		ClearArray();

		test_data->thread_number = 1;

		test_data->output_mutex = CreateMutexA(NULL, FALSE, "OUTPUT_MUTEX");
		test_data->array_mutex = CreateMutexA(NULL, FALSE, "ARRAY_MUTEX");

		test_data->start_threads_event = CreateEventA(NULL, TRUE, FALSE, "START_THREAD_EVENT");
		test_data->exit_thread_event = CreateEventA(NULL, TRUE, FALSE, "EXIT_THREAD_EVENT");
		test_data->thread_stopped_event = CreateEventA(NULL, TRUE, FALSE, "THREAD_STOPPED");
	}
	~SingleMarkerFixtureWin()
	{
		delete[] test_data->array_data;

		CloseHandle(test_data->output_mutex);
		CloseHandle(test_data->array_mutex);

		CloseHandle(test_data->start_threads_event);
		CloseHandle(test_data->exit_thread_event);
		CloseHandle(test_data->thread_stopped_event);

		delete[] test_data;
	}

	void ClearArray() 
	{
		for (size_t i = 0; i < test_data->array_size; i++) 
		{
			test_data->array_data[i] = 0;
		}
	}

	void Resize_array(size_t new_size) 
	{
		delete test_data->array_data;
		test_data->array_data = new int[new_size];
		test_data->array_size = new_size;
	}

	void ResetEvents() 
	{
		ResetEvent(test_data->start_threads_event);
		ResetEvent(test_data->exit_thread_event);
		ResetEvent(test_data->thread_stopped_event);
	}
};

BOOST_FIXTURE_TEST_SUITE
(
	marker_tests_single_win, 
	SingleMarkerFixtureWin
)

	BOOST_AUTO_TEST_CASE(marker_init) 
	{
		thread = CreateThread(NULL, NULL, marker, reinterpret_cast<LPVOID*>(&test_data), NULL, &id);
		Sleep(sleep_for);
		BOOST_TEST(thread != NULL);
		CloseHandle(thread);
	}

	BOOST_AUTO_TEST_CASE
	(
		marker_execute,
		* boost::unit_test::depends_on("marker_tests_single_win/marker_init")
	) 
	{
		thread = CreateThread(NULL, NULL, marker, reinterpret_cast<LPVOID*>(&test_data), NULL, &id);
		PulseEvent(test_data->start_threads_event);
		DWORD wait_res = WaitForSingleObject(test_data->thread_stopped_event, wait_for);
		BOOST_TEST(wait_res == 0);

		CloseHandle(thread);
		ResetEvents();
		ClearArray();
	}

	BOOST_AUTO_TEST_CASE
	(
		marker_result,
		* boost::unit_test::depends_on("marker_tests_single_win/marker_execute")
	)
	{
		thread = CreateThread(NULL, NULL, marker, reinterpret_cast<LPVOID*>(&test_data), NULL, &id);
		PulseEvent(test_data->start_threads_event);
		WaitForSingleObject(test_data->thread_stopped_event, INFINITE);

		size_t count = 0;
		for (size_t i = 0; i < test_data->array_size; i++) 
		{
			if (test_data->array_data[i]) 
			{
				BOOST_TEST(test_data->array_data[i] == test_data->thread_number);
				count++;
			}
		}

		std::string res = out.str();

		boost::regex expression("[0-9]+");
		boost::sregex_iterator it(res.begin(), res.end(), expression);
		boost::sregex_iterator end;

		BOOST_TEST(it != end);
		BOOST_TEST(it->str() == std::to_string(test_data->thread_number));
		++it;
		BOOST_TEST(it != end);
		BOOST_TEST(it->str() == std::to_string(count));
		++it;
		BOOST_TEST(it != end);
		BOOST_TEST(test_data->array_data[StringToSizeT(it->str())] != 0);

		CloseHandle(thread);
		ResetEvents();
		ClearArray();
	}

	BOOST_AUTO_TEST_CASE
	(
		marker_exit,
		* boost::unit_test::depends_on("marker_tests_single_win/marker_result")
	)
	{
		thread = CreateThread(NULL, NULL, marker, reinterpret_cast<LPVOID*>(&test_data), NULL, &id);
		PulseEvent(test_data->start_threads_event);
		WaitForSingleObject(test_data->thread_stopped_event, INFINITE);

		SetEvent(test_data->exit_thread_event);
		DWORD wait_res = WaitForSingleObject(thread, wait_for);

		BOOST_TEST(wait_res == 0);

		CloseHandle(thread);
		ResetEvents();
		ClearArray();
	}

BOOST_AUTO_TEST_SUITE_END()

#elif defined (USE_BOOST)

#include <utils/event_boost.h>

BOOST_AUTO_TEST_CASE
(
	boost_event,
	* boost::unit_test::timeout(3)
) 
{
	Event event;
	bool signaled = false;
	boost::thread thread([](Event* event, bool* signaled) { boost::this_thread::sleep_for(boost::chrono::milliseconds(1000)); event->Set(); *signaled = true; }, &event, &signaled);
	event.Wait();
	BOOST_TEST(signaled == true);
}

BOOST_AUTO_TEST_CASE(boost_multievent)
{

}

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
		redirect(out.rdbuf()),
		output_mutex(),
		array_mutex(),
		start_threads_event(),
		thread_stopped_event(1)
	{
		array_size = size;
		array = new int[array_size];
		thread_number = 1;
		response_events = new MultiEvent * [1];
		response_events[0] = new MultiEvent(2);
	}
	~SingleMarkerFixtureBoost()
	{
		delete[] array;
		delete response_events[0];
	}

	void ClearArray()
	{
		for (size_t i = 0; i < array_size; i++)
		{
			array[i] = 0;
		}
	}

	void Resize_array(size_t new_size)
	{
		delete array;
		array = new int[new_size];
		array_size = new_size;
	}

	void ResetEvents()
	{
		start_threads_event.Reset();
		thread_stopped_event.ResetAll();
		response_events[0]->ResetAll();
	}
};

BOOST_FIXTURE_TEST_SUITE
(
	marker_tests_single_boost, 
	SingleMarkerFixtureBoost
)

	BOOST_AUTO_TEST_CASE
	(
		marker_init_boost,
		* boost::unit_test::depends_on("boost_event")
		* boost::unit_test::depends_on("boost_multievent")
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

	BOOST_AUTO_TEST_CASE
	(
		marker_execute_boost,
		* boost::unit_test::depends_on("marker_tests_single_boost/marker_init")
		* boost::unit_test::timeout(3)
		* boost::unit_test::depends_on("boost_event")
		* boost::unit_test::depends_on("boost_multievent")
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

		thread->interrupt();
		ResetEvents();
		ClearArray();
	}

	BOOST_AUTO_TEST_CASE
	(
		marker_result_boost,
		*boost::unit_test::depends_on("marker_tests_single_boost/marker_execute")
		* boost::unit_test::depends_on("boost_event")
		* boost::unit_test::depends_on("boost_multievent")
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

		size_t count = 0;
		for (size_t i = 0; i < array_size; i++)
		{
			if (array[i])
			{
				BOOST_TEST(array[i] == thread_number);
				count++;
			}
		}

		std::string res = out.str();

		boost::regex expression("[0-9]+");
		boost::sregex_iterator it(res.begin(), res.end(), expression);
		boost::sregex_iterator end;

		BOOST_TEST(it != end);
		BOOST_TEST(it->str() == std::to_string(thread_number));
		++it;
		BOOST_TEST(it != end);
		BOOST_TEST(it->str() == std::to_string(count));
		++it;
		BOOST_TEST(it != end);
		BOOST_TEST(array[StringToSizeT(it->str())] != 0);

		thread->interrupt();
		ResetEvents();
		ClearArray();
	}

	BOOST_AUTO_TEST_CASE
	(
		marker_exit_boost,
		* boost::unit_test::depends_on("marker_tests_single_boost/marker_execute")
		* boost::unit_test::timeout(3)
		* boost::unit_test::depends_on("boost_event")
		* boost::unit_test::depends_on("boost_multievent")
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

		ResetEvents();
		ClearArray();
	}

BOOST_AUTO_TEST_SUITE_END()

#endif
