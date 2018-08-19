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
#include <istream>

using namespace boost::asio;
using std::cout;
using std::cin;
using std::endl;
using std::string;

typedef boost::shared_ptr <ip::tcp::socket> Socket_PTR;

class Thread_connection_receiving
{
	Queue_thread_safe *queue_data;
	bool is_initialized = false;
	bool is_started = false;
	unsigned short port = 0;
	string output_data;
	io_service *service_io_stream;
	boost::thread *thread;
	string thread_id;
	ip::tcp::endpoint *target;
	ip::tcp::acceptor *acceptor;
	ip::tcp::socket *sesion_socket_current;
	deadline_timer *timer_current;
	int Start();
	int Process_Connections();
	int Process_Connections_Handler();
public:
	Thread_connection_receiving(io_service *service_io_stream_New, unsigned short port_New, Queue_thread_safe *queue_data_new);
	void Set_Thread_Id();
	string Get_Thread_ID();
	int Create_Start();
	int Wait_Stop();
	boost::thread::id Get_ID();
};

