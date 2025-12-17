#include <base/server_boost.h>
#include <utils/input_parsing.h>
#include <base/message.h>

#include <boost/process/v2/process.hpp>
#include <boost/process/v2/windows/creation_flags.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>

Server::Server() : output_log_mutex(), active_connections(), client_processes_io_contexts(), database(nullptr) {}

Server::~Server()
{
	for (auto temp : client_processes_io_contexts)
	{
		delete temp;
	}
	delete database;
}

unsigned short Server::new_client_connection()
{
	unsigned short temp = static_cast<unsigned short>(active_connections.size());
	boost::thread* temp_thread = new boost::thread(client_connection, temp, &output_log_mutex, database);
	active_connections.add_thread(temp_thread);
	return temp;
}

unsigned short Server::new_client_process()
{
	unsigned short temp = static_cast<unsigned short>(client_processes_io_contexts.size());
	client_processes_io_contexts.push_back(new boost::asio::io_context());
	boost::process::process* temp_proc = new boost::process::process
	(
		client_processes_io_contexts[temp]->get_executor(),
		"Client.exe",
		{
			std::to_string(temp).c_str(),
			"message_queue_"
		},
		boost::process::v2::windows::create_new_console
	);
	temp_proc->detach();
	return temp;
}

void Server::log(std::string message)
{
	boost::unique_lock<boost::mutex> lock(output_log_mutex);
	std::cout << message << '\n';
}

void Server::Operate()
{
	std::string file_name = QueryFileName("Enter binary file name:", "");

	size_t number_of_employees = Query<size_t>("Enter the number of employees in the file:");

	std::vector<Employee> data;

	std::string temp_string;
	size_t temp_id;

	for (size_t i = 0; i < number_of_employees; i++)
	{
		temp_id = Query<size_t>("Enter employee ID:");
		data.push_back(Employee(temp_id, "", 0));
		temp_string = Query<std::string>("Enter employee name:");
		strcpy_s(data[i].name, temp_string.c_str());
		data[i].hours = Query<double>("Enter employee hours:");
	}

	database = new EmployeeDB(file_name, data);

	log("Initializing clients ...");

	unsigned short number_of_clients = Query<unsigned short>("Enter number of client processes:");
	unsigned short index;

	for (unsigned short i = 0; i < number_of_clients; i++)
	{
		index = new_client_connection();
		boost::this_thread::sleep_for(boost::chrono::milliseconds(20));
		new_client_process();

		log(std::string("CLIENT PROCESS [" + std::to_string(index) + "] created!"));
	}

	active_connections.join_all();
}

void client_connection(unsigned short connection_number, boost::mutex* output_log_mutex, EmployeeDB* database)
{
	output_log_mutex->lock();
	std::cout << "CONNECTION [" << connection_number << "] initialized!\n";
	output_log_mutex->unlock();

	boost::interprocess::message_queue::remove((std::string("message_queue_out_") + std::to_string(connection_number)).c_str());
	boost::interprocess::message_queue::remove((std::string("message_queue_in_") + std::to_string(connection_number)).c_str());

	boost::interprocess::message_queue out(boost::interprocess::create_only, (std::string("message_queue_out_") + std::to_string(connection_number)).c_str(), 1, sizeof(message));
	boost::interprocess::message_queue in(boost::interprocess::create_only, (std::string("message_queue_in_") + std::to_string(connection_number)).c_str(), 1, sizeof(message));
	

	message buffer(message_types::FOUND, 0, Employee(0, "", 0));
	size_t bytes_recieved;
	size_t locked_at, found_at;
	unsigned int priority = 0;
	bool operational = true;

	while (operational)
	{
		try
		{
			in.receive(&buffer, sizeof(message), bytes_recieved, priority);
		}
		catch (boost::interprocess::interprocess_exception e)
		{
			output_log_mutex->lock();
			std::cout << "CONNECTION [" << connection_number << "] terminated with error: " << e.what() << '\n';
			output_log_mutex->unlock();
			break;
		}
		switch (buffer.type)
		{
			case message_types::GET_SHARED:
			{
				output_log_mutex->lock();
				std::cout << "{GET SHARED} " << buffer.id << " FROM [" << connection_number << "]\n";
				output_log_mutex->unlock();

				if (database->WGetShared(buffer.id, buffer.data, locked_at))
				{
					buffer.type = message_types::NOT_FOUND;
				}
				else
				{
					buffer.type = message_types::FOUND;
				}

				try
				{
					out.send(&buffer, sizeof(message), priority);
				}
				catch (boost::interprocess::interprocess_exception e)
				{
					output_log_mutex->lock();
					std::cout << "CONNECTION [" << connection_number << "] terminated with error: " << e.what() << '\n';
					output_log_mutex->unlock();
					operational = false;
				}
				break;
			}
			case message_types::GET_EXCLUSIVE:
			{
				output_log_mutex->lock();
				std::cout << "{GET EXCLUSIVE} " << buffer.id << " FROM [" << connection_number << "]\n";
				output_log_mutex->unlock();

				if (database->WGetExclusive(buffer.id, buffer.data, locked_at, found_at))
				{
					buffer.type = message_types::NOT_FOUND;
				}
				else
				{
					buffer.type = message_types::FOUND;
				}

				try
				{
					out.send(&buffer, sizeof(message), priority);
				}
				catch (boost::interprocess::interprocess_exception e)
				{
					output_log_mutex->lock();
					std::cout << "CONNECTION [" << connection_number << "] terminated with error: " << e.what() << '\n';
					output_log_mutex->unlock();
					operational = false;
				}
				break;
			}
			case message_types::SET:
			{
				output_log_mutex->lock();
				std::cout << "{SET} " << buffer.id << " FROM [" << connection_number << "]\n";
				output_log_mutex->unlock();

				database->Set(buffer.data, found_at);

				buffer.type = message_types::FOUND;

				try
				{
					out.send(&buffer, sizeof(message), priority);
				}
				catch (boost::interprocess::interprocess_exception e)
				{
					output_log_mutex->lock();
					std::cout << "CONNECTION [" << connection_number << "] terminated with error: " << e.what() << '\n';
					output_log_mutex->unlock();
					operational = false;
				}
				break;
			}
			case message_types::UNLOCK_SHARED:
			{
				output_log_mutex->lock();
				std::cout << "{UNLOCK SHARED} " << buffer.id << " FROM [" << connection_number << "]\n";
				output_log_mutex->unlock();

				database->UnlockShared(locked_at);

				buffer.type = message_types::FOUND;
			
				try
				{
					out.send(&buffer, sizeof(message), priority);
				}
				catch (boost::interprocess::interprocess_exception e)
				{
					output_log_mutex->lock();
					std::cout << "CONNECTION [" << connection_number << "] terminated with error: " << e.what() << '\n';
					output_log_mutex->unlock();
					operational = false;
				}
				break;
			}
			case message_types::UNLOCK_EXCLUSIVE:
			{
				output_log_mutex->lock();
				std::cout << "{UNLOCK EXCLUSIVE} " << buffer.id << " FROM [" << connection_number << "]\n";
				output_log_mutex->unlock();

				database->UnlockExclusive(locked_at);

				buffer.type = message_types::FOUND;
			
				try
				{
					out.send(&buffer, sizeof(message), priority);
				}
				catch (boost::interprocess::interprocess_exception e)
				{
					output_log_mutex->lock();
					std::cout << "CONNECTION [" << connection_number << "] terminated with error: " << e.what() << '\n';
					output_log_mutex->unlock();
					operational = false;
				}
				break;
			}
			case message_types::SHUTDOWN:
			{
				buffer.type = message_types::FOUND;
			
				try
				{
					out.send(&buffer, sizeof(message), priority);
				}
				catch (boost::interprocess::interprocess_exception e)
				{
					output_log_mutex->lock();
					std::cout << "CONNECTION [" << connection_number << "] terminated with error: " << e.what() << '\n';
					output_log_mutex->unlock();
					operational = false;
				}

				operational = false;

				output_log_mutex->lock();
				std::cout << "CONNECTION[" << connection_number << "] terminated!\n";
				output_log_mutex->unlock();

				break;
			}
		}
	}

	boost::interprocess::message_queue::remove((std::string("message_queue_out_") + std::to_string(connection_number)).c_str());
	boost::interprocess::message_queue::remove((std::string("message_queue_in_") + std::to_string(connection_number)).c_str());
}