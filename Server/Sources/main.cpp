#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include "server_main.h"
#include "console_output.h"

#define NUMBER_OF_PORT_THREADS 1
#define START_PORT 42042

#define MAIN_THREADS

using namespace boost::asio;
using std::cout;
using std::cin;
using std::endl;
using std::string;

int main()
{
	string threads_id_main = boost::lexical_cast<std::string>(boost::this_thread::get_id());
	io_service io_service_object;
	Queue_thread_safe queue_data;
	boost::thread talk_thread = boost::thread(boost::bind(Queue_Process, boost::ref(queue_data)));
#ifdef MAIN_THREADS
	std::vector <Thread_connection_receiving> threads_connections_acceptor;
	string *threads_id_acceptors;
	threads_id_acceptors = new string[NUMBER_OF_PORT_THREADS];
	for (int i = 0; i < NUMBER_OF_PORT_THREADS; i++)
	{
		threads_connections_acceptor.push_back(Thread_connection_receiving(&io_service_object, START_PORT + i, &queue_data));
	}
	boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
	for (int i = 0; i < NUMBER_OF_PORT_THREADS; i++)
	{
		threads_connections_acceptor[i].Create_Start();
		boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
		threads_id_acceptors[i] = threads_connections_acceptor[i].Get_Thread_ID();
	}
	Add_Main_Info(queue_data, threads_id_main, NUMBER_OF_PORT_THREADS, threads_id_acceptors);
	for (int i = 0; i < NUMBER_OF_PORT_THREADS; i++)
	{
		threads_connections_acceptor[i].Wait_Stop();
	}
#endif
	talk_thread.join();
	return 0;
}

