#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "server_main.h"

#define NUMBER_OF_PORT_THREADS 1
#define START_PORT 42042

using namespace boost::asio;
using std::cout;
using std::cin;
using std::endl;
using std::string;

int main()
{
	cout << "Main ID: " << boost::this_thread::get_id() << endl;
	io_service io_service_object;
	std::vector <Thread_connection_receiving> threads_connections_acceptor;
	for (int i = 0; i < NUMBER_OF_PORT_THREADS; i++)
	{
		threads_connections_acceptor.push_back(Thread_connection_receiving(&io_service_object, START_PORT + i));
	}
	for (int i = 0; i < NUMBER_OF_PORT_THREADS; i++)
	{
		threads_connections_acceptor[i].Create_Start();
	}
	boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
	cout << "Threads IDs form main: " << endl;
	for (int i = 0; i < NUMBER_OF_PORT_THREADS; i++)
	{
		cout << threads_connections_acceptor[i].Get_ID();
		cout << " Test " << threads_connections_acceptor[i].Get_Test() << endl;
	}
	cout << endl;
	for (int i = 0; i < NUMBER_OF_PORT_THREADS; i++)
	{
		threads_connections_acceptor[i].Wait_Stop();
	}
	system("pause");
	return 0;
}

