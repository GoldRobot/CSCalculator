#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "server_instance.h"
#include "thread_safe_data_structures.h"

#define NUMBER_OF_PORT_THREADS 5
#define START_PORT 42042
#define PRINT_MAIN_SERVICE_INFO true
#define MAIN_LOOP_ENABLE true


//#define CLOSE_AFTER_START

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::to_string;

int main()
{
	Thread_safe_print_queue print_queue;
	boost::thread print_thread = boost::thread(boost::bind(Thread_Safe_Print_Queue_Process, boost::ref(print_queue)));
	Thread_safe_server_info server_info;
	server_info.Thread_Main_Os_ID_Set(boost::lexical_cast<std::string>(boost::this_thread::get_id()));
	std::vector <Server_instance> threads_connections_acceptor;
	for (int i = 0; i < NUMBER_OF_PORT_THREADS; i++)
	{
		threads_connections_acceptor.emplace_back(START_PORT + i, &print_queue, &server_info, i);
	}
	for (int i = 0; i < NUMBER_OF_PORT_THREADS; i++)
	{
		boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
		threads_connections_acceptor[i].Thread_Create_Start();
	}
#ifdef CLOSE_AFTER_START
	boost::this_thread::sleep_for(boost::chrono::milliseconds(3000));
	for (int i = 0; i < NUMBER_OF_PORT_THREADS; i++)
	{
		threads_connections_acceptor[i].Interrupt();
	}
	boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
	print_thread.interrupt();
#endif
	string temp_string;
	while (MAIN_LOOP_ENABLE)
	{
		if (PRINT_MAIN_SERVICE_INFO)
		{

			temp_string = "Connections: " + to_string(server_info.Connections_Total_Amount_Get()) + "\n";
			for (int i = 0; i < NUMBER_OF_PORT_THREADS; i++)
			{
				temp_string += "Port " + to_string(threads_connections_acceptor[i].Port_Get()) + ":" + to_string(threads_connections_acceptor[i].Connections_Amount_Get());
				if (i!= (NUMBER_OF_PORT_THREADS-1))
				{
					temp_string += " -|- ";
				}
			}
			temp_string += "\n\n";
			print_queue.Push(temp_string);
		}
		boost::this_thread::sleep_for(boost::chrono::milliseconds(2000));
	}
	for (int i = 0; i < NUMBER_OF_PORT_THREADS; i++)
	{
		threads_connections_acceptor[i].Wait_Join();
	}
	print_thread.join();
	return 0;
}

