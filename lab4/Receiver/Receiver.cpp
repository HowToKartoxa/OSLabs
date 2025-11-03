#include <utils/input_parsing.h>

#include <iostream>
#include <string>

#include <windows.h>

#ifndef USE_BOOST

#define USE_BOOST

#endif

#if defined(USE_WINAPI)

#include <utils/message_queue.h>

#elif defined(USE_BOOST)

#include <utils/message_queue_boost.h>

#endif


int main(int argc, char** argv)
{
	std::string temp_string;
	std::string file_name;
	long number_of_entries;
	unsigned short number_of_senders;

	std::cout << "Enter the binary file name:\n";
	std::getline(std::cin, file_name);
	while (!CheckFileName(file_name, ""))
	{
		std::cout << "Enter the binary file name:\n";
		std::getline(std::cin, file_name);
	}

	std::cout << "Enter the number of message entries in the binary file:\n";
	std::getline(std::cin, temp_string);
	while (!CheckIfPositiveLong(temp_string))
	{
		std::cout << "Enter the number of message entries in the binary file:\n";
		std::getline(std::cin, temp_string);
	}
	number_of_entries = std::stol(temp_string);

	MessageQueue queue(file_name, number_of_entries);

	std::cout << "Enter the number of sender processes:\n";
	std::getline(std::cin, temp_string);
	while (!CheckIfUnsignedShort(temp_string))
	{
		std::cout << "Enter the number of sender processes:\n";
		std::getline(std::cin, temp_string);
	}
	number_of_senders = std::stoi(temp_string);

	HANDLE* senders = new HANDLE[number_of_senders];
	HANDLE* senders_started_events = new HANDLE[number_of_senders];
	STARTUPINFOA* senders_stuartup_info = new STARTUPINFOA[number_of_senders];
	PROCESS_INFORMATION* senders_process_info = new PROCESS_INFORMATION[number_of_senders];

	for (unsigned short i = 0; i < number_of_senders; i++)
	{
		ZeroMemory(&senders_stuartup_info[i], sizeof(STARTUPINFOA));
		senders_stuartup_info[i].cb = sizeof(STARTUPINFOA);
		senders_started_events[i] = CreateEventA(NULL, TRUE, FALSE, (std::string("SENDER_") + std::to_string(i) + "_STARTED").c_str());
		if (senders_started_events[i] == NULL)
		{
			std::cout << "Failed to create start event for sender process " << i << " !";
			system("pause");
			return -1;
		}
		if (!CreateProcessA
		(
			NULL,
			const_cast<char*>((std::string("Sender.exe ") + file_name + ' ' + std::to_string(i)).c_str()),
			NULL,
			NULL,
			FALSE,
			CREATE_NEW_CONSOLE,
			NULL,
			NULL,
			&senders_stuartup_info[i],
			&senders_process_info[i]
		))
		{
			std::cout << "Failed to create sender process " << i << " !";
			system("pause");
			return -1;
		}
	}

	WaitForMultipleObjects(number_of_senders, senders_started_events, TRUE, INFINITE);

	std::cout << "Enter 0 to exit, othervise enter a number of messages to wait for: ";
	std::getline(std::cin, temp_string);
	while (!CheckIfUnsignedShort(temp_string))
	{
		std::cout << "Enter 0 to exit, othervise enter a number of messages to wait for: ";
		std::getline(std::cin, temp_string);
	}
	unsigned short user_input = std::stoi(temp_string);

	if (user_input)
	{
		MessageQueue::Message msg;
		while (true)
		{
			if (!user_input)
			{
				std::cout << "Enter 0 to exit, othervise enter a number of messages to wait for: ";
				std::getline(std::cin, temp_string);
				while (!CheckIfUnsignedShort(temp_string))
				{
					std::cout << "Enter 0 to exit, othervise enter a number of messages to wait for: ";
					std::getline(std::cin, temp_string);
				}
				user_input = std::stoi(temp_string);
				if (!user_input)
				{
					break;
				}
			}
			if (!queue.WDequeue(msg))
			{
				std::cout << "Failed to wait for a message!";
				system("pause");
				return -1;
			}
			std::cout << "MESSAGE FROM [ " << msg.author << " ]: " << msg.data << '\n';
			user_input--;
		}
	}
	system("pause");
	return 0;
}