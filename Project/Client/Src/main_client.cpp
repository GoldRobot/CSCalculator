#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <iostream>
#include <string>

#include "client_connection_instance.h"
#include "thread_safe_data_structures.h"

constexpr auto ADRESS_IP = "127.0.0.1";
constexpr auto PORT = 42042;

using std::cout;
using std::cin;
using std::endl;
using std::string;
using namespace boost::asio;

#define NUMBER_OF_CONNECTIONS 5000

#define NUMBER_OF_THREADS 400

#define TEST_MODE true

#define NUMBER_OF_PORTS 5

int main()
{
	Thread_safe_print_queue print_queue;
	boost::thread print_thread = boost::thread(boost::bind(Thread_Safe_Print_Queue_Process, boost::ref(print_queue)));

	io_service io_service_connection;
	boost::thread_group thread_pool_connection;
	boost::asio::io_service::work work_connection(io_service_connection);
	
	for (int i = 0; i < NUMBER_OF_THREADS; i++)
	{
		thread_pool_connection.create_thread(boost::bind(&io_service::run, &io_service_connection));
	}
	
	Client_Connection_Instance *client;
	for (int i = 0; i < NUMBER_OF_CONNECTIONS; i++)
	{
		for (int i_port = 0; i_port < NUMBER_OF_PORTS; i_port++)
		{

			client = new Client_Connection_Instance(&io_service_connection, &print_queue, ADRESS_IP, PORT+ i_port, i);
			client->Initialization();
		}
	}

	while (true)
	{
		boost::this_thread::sleep_for(boost::chrono::milliseconds(10000));
	}
	io_service_connection.stop();
	thread_pool_connection.join_all();
	return 0;
}

