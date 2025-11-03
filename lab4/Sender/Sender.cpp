#include <utils/input_parsing.h>

#include <iostream>
#include <windows.h>

#include <windows.h>

#if defined(USE_WINAPI)

#include <utils/message_queue.h>

#elif defined(USE_BOOST)

#include <utils/message_queue_boost.h>

#endif

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		std::cout << "Too few arguments passed!";
		system("pause");
		return -1;
	}

	std::cout << "Sender [ " << argv[2] << " ]\n";
	unsigned short sender_num = std::atoi(argv[2]);

	HANDLE process_start_event = OpenEventA(EVENT_MODIFY_STATE | SYNCHRONIZE, FALSE, (std::string("SENDER_") + argv[2] + "_STARTED").c_str());
	if (process_start_event == NULL)
	{
		std::cout << "Failed to open start event!";
		system("pause");
		return -1;
	}

	SetEvent(process_start_event);

	std::string temp_string;

	std::cout << "Enter 0 to exit, othervise enter a number of messages to send: ";
	std::getline(std::cin, temp_string);
	while (!CheckIfUnsignedShort(temp_string))
	{
		std::cout << "Enter 0 to exit, othervise enter a number of messages to send: ";
		std::getline(std::cin, temp_string);
	}
	unsigned short user_input = std::stoi(temp_string);

	if (user_input)
	{
		MessageQueue queue(argv[1], 0l, false);
		MessageQueue::Message msg;

		while (true)
		{
			if (!user_input)
			{
				std::cout << "Enter 0 to exit, othervise enter a number of messages to send: ";
				std::getline(std::cin, temp_string);
				while (!CheckIfUnsignedShort(temp_string))
				{
					std::cout << "Enter 0 to exit, othervise enter a number of messages to send: ";
					std::getline(std::cin, temp_string);
				}
				user_input = std::stoi(temp_string);
				if (!user_input)
				{
					break;
				}
			}
			std::cout << "--> ";
			std::getline(std::cin, temp_string);
			if (temp_string.length() > 19)
			{
				std::cout << "Message too long, will send only the first 19 characters!\n";
			}
			msg = MessageQueue::Message(temp_string, sender_num);
			if (!queue.WEnqueue(msg))
			{
				std::cout << "Failed to wait for a free space in the message queue!";
				system("pause");
				return -1;
			}
			user_input--;
		}
	}
	system("pause");
	return 0;
}