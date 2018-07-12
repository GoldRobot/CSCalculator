#pragma once
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "console_output.h"

using namespace boost::asio;
using std::cout;
using std::cin;
using std::endl;
using std::string;

typedef boost::shared_ptr <ip::tcp::socket> Socket_PTR;

class Thread_connection_receiving
{
	Queue_thread_safe *queue_data;
	int test = 0;
	bool is_initialized = false;
	bool is_started = false;
	unsigned short port = 0;
	io_service *service_io_stream;
	boost::thread *thread;
	string thread_id;
	string output_data;
	ip::tcp::endpoint *target;
	ip::tcp::acceptor *acceptor;
	int Start();
	deadline_timer *timer_current;
	ip::tcp::socket *sesion_socket_current;
public:
	Thread_connection_receiving(io_service *service_io_stream_New, unsigned short port_New, Queue_thread_safe *queue_data_new);
	void Set_Thread_Id();
	string Get_Thread_ID();
	int Create_Start();
	int Wait_Stop();
	boost::thread::id Get_ID();
	int Process_Connections();
	int Process_Connections_Handler();
	void Test_Inc();
	int Get_Test();
};

