#define BOOST_TEST_MODULE main_tests

#include <boost/test/included/unit_test.hpp>
#include <fstream>

#if defined(USE_WINAPI)

#include <utils/message_queue.h>

#include <windows.h>

struct MessageQueueSingleFixtureWin
{
	MessageQueue queue;
	MessageQueueSingleFixtureWin() : queue("test_binary_file", 5l) {}
};

struct MessageQueueDupFixtureWin
{
	MessageQueue queue;
	MessageQueue queue_dup;
	MessageQueueDupFixtureWin() : queue("test_binary_file", 5l), queue_dup("test_binary_file", 0l, false) {}
};

BOOST_AUTO_TEST_SUITE(message_queue_win)


	BOOST_AUTO_TEST_CASE(message_queue_init, * boost::unit_test::timeout(2))
	{
		MessageQueue q("test_binary_file", 10);
		HANDLE res = OpenSemaphoreA(SYNCHRONIZE, FALSE, "test_binary_file_MSG_Q_ENQ_SEM");
		bool res_bool = (res != NULL);
		BOOST_TEST(res_bool);
		if(res_bool) CloseHandle(res);
		res = OpenSemaphoreA(SYNCHRONIZE, FALSE, "test_binary_file_MSG_Q_DEQ_SEM");
		res_bool = (res != NULL);
		BOOST_TEST(res_bool);
		if (res_bool) CloseHandle(res);
		res = OpenMutexA(SYNCHRONIZE, FALSE, "test_binary_file_MSG_Q_MTX");
		res_bool = (res != NULL);
		BOOST_TEST(res_bool);
		if (res_bool) CloseHandle(res);
		std::fstream file("test_binary_file", std::ios::in | std::ios::out);
		BOOST_TEST(file.is_open() == true);
	}
	
	BOOST_FIXTURE_TEST_CASE(message_queue_single_enq_deq, MessageQueueSingleFixtureWin, * boost::unit_test::timeout(2))
	{
		MessageQueue::Message input("test_message", 0);
		MessageQueue::Message result;
		BOOST_TEST(queue.WEnqueue(input) == true);
		BOOST_TEST(queue.WDequeue(result) == true);
		BOOST_TEST(result.author == input.author);
		BOOST_TEST(std::strcmp(result.data, input.data) == 0);
	}
	
	BOOST_FIXTURE_TEST_CASE(message_queue_single_empty, MessageQueueSingleFixtureWin, * boost::unit_test::timeout(2))
	{
		MessageQueue::Message res;
		BOOST_TEST(queue.WDequeue(res, 100) == false);
	}

	BOOST_FIXTURE_TEST_CASE(message_queue_single_full, MessageQueueSingleFixtureWin, * boost::unit_test::timeout(2))
	{
		MessageQueue::Message input(" _test_message", 0);
		for (char i = 0; i < 5; i++)
		{
			input.data[0] = i + '0';
			queue.WEnqueue(input);
			Sleep(100);
		}
		BOOST_TEST(queue.WEnqueue(input, 100) == false);
	}

	BOOST_AUTO_TEST_CASE(message_queue_file_already_exists, * boost::unit_test::timeout(4))
	{
		std::ofstream out("test_binary_file");
		for (int i = 0; i < 1000; i++)
		{
			out.write(reinterpret_cast<const char*>(&i), sizeof(int));
		}
		out.close();
		MessageQueue q("test_binary_file", 10);
		MessageQueue::Message input("test_message", 0);
		MessageQueue::Message result;
		BOOST_TEST(q.WEnqueue(input) == true);
		Sleep(100);
		BOOST_TEST(q.WDequeue(result) == true);
		BOOST_TEST(result.author == input.author);
		BOOST_TEST(std::strcmp(result.data, input.data) == 0);
	}

	BOOST_AUTO_TEST_CASE(message_queue_dup_init, * boost::unit_test::timeout(2))
	{
		MessageQueue q("test_binary_file", 10);
		MessageQueue q_dup("test_binary_file", 10, false);
		HANDLE res = OpenSemaphoreA(SYNCHRONIZE, FALSE, "test_binary_file_MSG_Q_ENQ_SEM");
		BOOST_TEST(res != nullptr);
		if (res != nullptr) CloseHandle(res);
		res = OpenSemaphoreA(SYNCHRONIZE, FALSE, "test_binary_file_MSG_Q_DEQ_SEM");
		BOOST_TEST(res != nullptr);
		if (res != nullptr) CloseHandle(res);
		res = OpenMutexA(SYNCHRONIZE, FALSE, "test_binary_file_MSG_Q_MTX");
		BOOST_TEST(res != nullptr);
		if (res != nullptr) CloseHandle(res);
		std::fstream file("test_binary_file", std::ios::in | std::ios::out);
		BOOST_TEST(file.is_open() == true);
	}

	BOOST_FIXTURE_TEST_CASE(message_queue_dup_enq_deq, MessageQueueDupFixtureWin, * boost::unit_test::timeout(2))
	{
		MessageQueue::Message input1("test_message_1", 0);
		MessageQueue::Message result;
		BOOST_TEST(queue.WEnqueue(input1) == true);
		Sleep(100);
		BOOST_TEST(queue_dup.WDequeue(result) == true);
		BOOST_TEST(result.author == input1.author);
		BOOST_TEST(std::strcmp(result.data, input1.data) == 0);
		MessageQueue::Message input2("test_message_2", 1);
		BOOST_TEST(queue_dup.WEnqueue(input2) == true);
		Sleep(100);
		BOOST_TEST(queue.WDequeue(result) == true);
		BOOST_TEST(result.author == input2.author);
		BOOST_TEST(std::strcmp(result.data, input2.data) == 0);
	}

	BOOST_AUTO_TEST_CASE(message_queue_already_created, * boost::unit_test::timeout(2))
	{
		MessageQueue q("test_binary_file", 10);
		try
		{
			MessageQueue q_new("test_binary_file", 10);
		}
		catch (std::runtime_error e)
		{
			BOOST_TEST(std::strcmp(e.what(), "Failed to create enqueue semaphore in Myqueue constructor because semaphore with this name already exists") == 0);
		}
	}
	
BOOST_AUTO_TEST_SUITE_END()

#elif defined(USE_BOOST)

#include <utils/message_queue_boost.h>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

struct MessageQueueSingleFixtureBoost
{
	MessageQueue queue;
	MessageQueueSingleFixtureBoost(unsigned int number_of_entries = 5) : queue("test_binary_file", number_of_entries) {}
	~MessageQueueSingleFixtureBoost() {}
};

struct MessageQueueDupFixtureBoost
{
	MessageQueue queue;
	MessageQueue queue_dup;
	MessageQueueDupFixtureBoost(unsigned int number_of_entries = 5) : queue("test_binary_file", number_of_entries), queue_dup("test_binary_file", 0u, false) {}
	~MessageQueueDupFixtureBoost() {}
};

BOOST_AUTO_TEST_SUITE(message_queue_boost)

	BOOST_AUTO_TEST_CASE(message_queue_init, * boost::unit_test::timeout(2))
	{
		MessageQueue q("test_binary_file", 10);
		try
		{
			boost::interprocess::named_semaphore enq_semaphore(boost::interprocess::open_only, "test_binary_file_MSG_Q_ENQ_SEM");
			boost::interprocess::named_semaphore deq_semaphore(boost::interprocess::open_only, "test_binary_file_MSG_Q_DEQ_SEM");
			boost::interprocess::named_mutex file_mutex(boost::interprocess::open_only, "test_binary_file_MSG_Q_MTX");
		}
		catch (boost::interprocess::interprocess_exception e)
		{
			BOOST_TEST(false);
		}
		std::fstream file("test_binary_file", std::ios::in | std::ios::out);
		BOOST_TEST(file.is_open() == true);
	}

	BOOST_FIXTURE_TEST_CASE(message_queue_single_enq_deq, MessageQueueSingleFixtureBoost, * boost::unit_test::timeout(2))
	{
		MessageQueue::Message input("test_message", 0);
		MessageQueue::Message result;
		BOOST_TEST(queue.WEnqueue(input) == true);
		BOOST_TEST(queue.WDequeue(result) == true);
		BOOST_TEST(result.author == input.author);
		BOOST_TEST(std::strcmp(result.data, input.data) == 0);
	}

	BOOST_FIXTURE_TEST_CASE(message_queue_single_empty, MessageQueueSingleFixtureBoost, * boost::unit_test::timeout(2))
	{
		MessageQueue::Message res;
		BOOST_TEST(queue.WDequeue(res, boost::chrono::milliseconds(100)) == false);
	}

	BOOST_FIXTURE_TEST_CASE(message_queue_single_full, MessageQueueSingleFixtureBoost, * boost::unit_test::timeout(2))
	{
		MessageQueue::Message input(" _test_message", 0);
		for (char i = 0; i < 5; i++)
		{
			input.data[0] = i + '0';
			queue.WEnqueue(input);
		}
		BOOST_TEST(queue.WEnqueue(input, boost::chrono::milliseconds(100)) == false);
	}

	BOOST_AUTO_TEST_CASE(message_queue_file_already_exists, * boost::unit_test::timeout(4))
	{
		std::ofstream out("test_binary_file");
		for (int i = 0; i < 1000; i++)
		{
			out.write(reinterpret_cast<const char*>(&i), sizeof(int));
		}
		out.close();
		MessageQueue q("test_binary_file", 10);
		MessageQueue::Message input("test_message", 0);
		MessageQueue::Message result;
		BOOST_TEST(q.WEnqueue(input) == true);
		BOOST_TEST(q.WDequeue(result) == true);
		BOOST_TEST(result.author == input.author);
		BOOST_TEST(std::strcmp(result.data, input.data) == 0);
	}

	BOOST_AUTO_TEST_CASE(message_queue_dup_init, * boost::unit_test::timeout(2))
	{
		MessageQueue q("test_binary_file", 10);
		MessageQueue q_dup("test_binary_file", 10, false);
		try
		{
			boost::interprocess::named_semaphore enq_semaphore(boost::interprocess::open_only, "test_binary_file_MSG_Q_ENQ_SEM");
			boost::interprocess::named_semaphore deq_semaphore(boost::interprocess::open_only, "test_binary_file_MSG_Q_DEQ_SEM");
			boost::interprocess::named_mutex file_mutex(boost::interprocess::open_only, "test_binary_file_MSG_Q_MTX");
		}
		catch (boost::interprocess::interprocess_exception e)
		{
			BOOST_TEST(false);
		}
		std::fstream file("test_binary_file", std::ios::in | std::ios::out);
		BOOST_TEST(file.is_open() == true);
	}

	BOOST_FIXTURE_TEST_CASE(message_queue_dup_enq_deq, MessageQueueDupFixtureBoost, * boost::unit_test::timeout(2))
	{
		MessageQueue::Message input1("test_message_1", 0);
		MessageQueue::Message result;
		BOOST_TEST(queue.WEnqueue(input1) == true);
		BOOST_TEST(queue_dup.WDequeue(result) == true);
		BOOST_TEST(result.author == input1.author);
		BOOST_TEST(std::strcmp(result.data, input1.data) == 0);
		MessageQueue::Message input2("test_message_2", 1);
		BOOST_TEST(queue_dup.WEnqueue(input2) == true);
		BOOST_TEST(queue.WDequeue(result) == true);
		BOOST_TEST(result.author == input2.author);
		BOOST_TEST(std::strcmp(result.data, input2.data) == 0);
	}

	BOOST_AUTO_TEST_CASE(message_queue_already_created, * boost::unit_test::timeout(2))
	{
		MessageQueue q("test_binary_file", 10);
		try
		{
			MessageQueue q_new("test_binary_file", 10);
		}
		catch (boost::interprocess::interprocess_exception e)
		{
			BOOST_TEST(true);
		}
		BOOST_TEST(false);
	}

BOOST_AUTO_TEST_SUITE_END()

#endif
