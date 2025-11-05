#include <utils/message_queue_boost.h>

#include <iostream>
#include <stdexcept>

// file layout:

////////////////////////
//      capacity      // 
//        size        // 
//        front       //  
// entry slot 0       // 
// entry slot 1       // 
// entry slot 2       // 
// entry slot 3       // 
// entry slot 4       // 
//         .          //
//         .          //	
//         .          //
// entry slot cap - 3 // 
// entry slot cap - 2 // 
// entry slot cap - 1 // 
////////////////////////

MessageQueue::MessageQueue(std::string _file_name, unsigned int number_of_entries, bool own) :
	file_name(_file_name),
	is_owner(own)
{
	if (is_owner)
	{
		enq_semaphore = new boost::interprocess::named_semaphore(boost::interprocess::create_only, (file_name + "_MSG_Q_ENQ_SEM").c_str(), number_of_entries);
		deq_semaphore = new boost::interprocess::named_semaphore(boost::interprocess::create_only, (file_name + "_MSG_Q_DEQ_SEM").c_str(), 0u);
		file_mutex = new boost::interprocess::named_mutex(boost::interprocess::create_only, (file_name + "_MSG_Q_MTX").c_str());
	
		std::ofstream file(file_name, std::ios::binary);

		Info info(number_of_entries);
		file.write(reinterpret_cast<const char*>(&info), sizeof(Info));

		Message placeholder;
		for (unsigned int i = 0; i < number_of_entries; i++)
		{
			file.write(reinterpret_cast<const char*>(&placeholder), sizeof(Message));
		}
		
		file.close();
	}
	else
	{
		enq_semaphore = new boost::interprocess::named_semaphore(boost::interprocess::open_only, (file_name + "_MSG_Q_ENQ_SEM").c_str());
		deq_semaphore = new boost::interprocess::named_semaphore(boost::interprocess::open_only, (file_name + "_MSG_Q_DEQ_SEM").c_str());
		file_mutex = new boost::interprocess::named_mutex(boost::interprocess::open_only, (file_name + "_MSG_Q_MTX").c_str());
	}
}

MessageQueue::~MessageQueue()
{
	if (is_owner)
	{
		enq_semaphore->remove((file_name + "_MSG_Q_ENQ_SEM").c_str());
		deq_semaphore->remove((file_name + "_MSG_Q_DEQ_SEM").c_str());
		file_mutex->remove((file_name + "_MSG_Q_MTX").c_str());
		std::remove(file_name.c_str());
	}
	delete enq_semaphore;
	delete deq_semaphore;
	delete file_mutex;
}

bool MessageQueue::WEnqueue(Message message)
{
	try
	{
		enq_semaphore->wait();
		file_mutex->lock();
	}
	catch (boost::interprocess::interprocess_exception e)
	{
		return false;
	}

	std::fstream file(file_name, std::ios::binary | std::ios::in | std::ios::out);

	Info info;
	file.read(reinterpret_cast<char*>(&info), sizeof(Info));

	file.seekp(sizeof(Info) + sizeof(Message) * ((info.front + info.size) % info.capacity), std::ios::beg);
	file.write(reinterpret_cast<const char*>(&message), sizeof(Message));

	info.size++;
	file.seekp(0, std::ios::beg);
	file.write(reinterpret_cast<const char*>(&info), sizeof(Info));

	file.close();
	
	file_mutex->unlock();
	deq_semaphore->post();
	return true;
}

bool MessageQueue::WDequeue(Message& destination)
{
	try
	{
		deq_semaphore->wait();
		file_mutex->lock();
	}
	catch (boost::interprocess::interprocess_exception e)
	{
		return false;
	}

	std::fstream file(file_name, std::ios::binary | std::ios::in | std::ios::out);

	Info info;
	file.read(reinterpret_cast<char*>(&info), sizeof(Info));

	file.seekg(sizeof(Info) + sizeof(Message) * info.front, std::ios::beg);
	file.read(reinterpret_cast<char*>(&destination), sizeof(Message));

	info.front = (info.front + 1) % info.capacity;
	info.size--;
	file.seekp(0, std::ios::beg);
	file.write(reinterpret_cast<const char*>(&info), sizeof(Info));

	file.close();
	
	file_mutex->unlock();
	enq_semaphore->post();
	return true;
}

bool MessageQueue::WEnqueue(Message message, boost::posix_time::ptime wait_for)
{
	try
	{
		if (!enq_semaphore->timed_wait(wait_for))
		{
			return false;
		}
		file_mutex->lock();
	}
	catch (boost::interprocess::interprocess_exception e)
	{
		return false;
	}

	std::fstream file(file_name, std::ios::binary | std::ios::in | std::ios::out);

	Info info;
	file.read(reinterpret_cast<char*>(&info), sizeof(Info));

	file.seekp(sizeof(Info) + sizeof(Message) * ((info.front + info.size) % info.capacity), std::ios::beg);
	file.write(reinterpret_cast<const char*>(&message), sizeof(Message));

	info.size++;
	file.seekp(0, std::ios::beg);
	file.write(reinterpret_cast<const char*>(&info), sizeof(Info));

	file.close();

	file_mutex->unlock();
	deq_semaphore->post();
	return true;
}

bool MessageQueue::WDequeue(Message& destination, boost::posix_time::ptime wait_for)
{
	try
	{
		if (!deq_semaphore->timed_wait(wait_for))
		{
			return false;
		}
		file_mutex->lock();
	}
	catch (boost::interprocess::interprocess_exception e)
	{
		return false;
	}

	std::fstream file(file_name, std::ios::binary | std::ios::in | std::ios::out);

	Info info;
	file.read(reinterpret_cast<char*>(&info), sizeof(Info));

	file.seekg(sizeof(Info) + sizeof(Message) * info.front, std::ios::beg);
	file.read(reinterpret_cast<char*>(&destination), sizeof(Message));

	info.front = (info.front + 1) % info.capacity;
	info.size--;
	file.seekp(0, std::ios::beg);
	file.write(reinterpret_cast<const char*>(&info), sizeof(Info));

	file.close();

	file_mutex->unlock();
	enq_semaphore->post();
	return true;
}
