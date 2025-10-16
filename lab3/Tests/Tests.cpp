#define BOOST_TEST_MODULE main_tests

#include <utils/input_parsing.h>

#include <boost/test/included/unit_test.hpp>
#include <boost/regex.hpp>

#define USE_WINAPI

#if defined (USE_WINAPI)

#include <utils/marker_parameter_data.h>
#include <utils/marker_procedures.h>

#include <windows.h>

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

struct SingleMarkerFixtureWin
{
	MarkerParameterData* test_data;

	HANDLE thread;

	DWORD id;

	CoutRedirect redirect;

	boost::test_tools::output_test_stream out;

	const size_t sleep_for;
	const size_t wait_for;

	SingleMarkerFixtureWin(size_t size = 10) : 
		sleep_for(30), 
		wait_for(100),
		redirect(out.rdbuf())
	{
		test_data = new MarkerParameterData();

		test_data->array_data = new int[size];
		test_data->array_size = size;
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

	void ResetEvents() 
	{
		ResetEvent(test_data->start_threads_event);
		ResetEvent(test_data->exit_thread_event);
		ResetEvent(test_data->thread_stopped_event);
	}
};

struct MultipleMarkerFixtureWin 
{

};

BOOST_FIXTURE_TEST_SUITE(marker_tests_single_win, SingleMarkerFixtureWin)

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
		marker_result,
		* boost::unit_test::depends_on("marker_tests_single_win/marker_execute")
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

BOOST_FIXTURE_TEST_SUITE(marker_tests_multiple_win, MultipleMarkerFixtureWin)


BOOST_AUTO_TEST_SUITE_END()

#elif defined (USE_BOOST)

#endif
