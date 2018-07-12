#include "server_main.h"

#define ENABLE_CONNECTION_RECEIVING true
#define CLOSE_AFTER_CLIENT_CONNECT false

int Client_Sesion_Process(ip::tcp::socket *sesion_socket, Queue_thread_safe &queue_data)
{
	string output_data;
	output_data = "Sesion estabelished.\n";
	queue_data.Push(output_data);
	if (CLOSE_AFTER_CLIENT_CONNECT)
	{
		sesion_socket->close();
		output_data = "Sesion closed.\n";
		queue_data.Push(output_data);
		//sesion_socket->release();

	}
	else {
		while (true)
		{

		}
	}
	sesion_socket->close();
	//sesion_socket->release();
	delete sesion_socket;
	return 0;
}

int Thread_connection_receiving::Process_Connections_Handler()
{
	boost::thread(boost::bind(&Client_Sesion_Process, sesion_socket_current, boost::ref(*queue_data)));
	sesion_socket_current = new ip::tcp::socket(*service_io_stream);
	output_data = thread_id + " awaiting connection...\n";
	queue_data->Push(output_data);
	acceptor->async_accept(*sesion_socket_current, boost::bind(&Thread_connection_receiving::Process_Connections_Handler, this));
	output_data = thread_id + " accepted connection...\n";
	queue_data->Push(output_data);
	return 0;
}

inline void Thread_connection_receiving::Test_Inc()
{
	test++;
}

inline int Thread_connection_receiving::Get_Test()
{
	return test;
}

int Thread_connection_receiving::Process_Connections()
{
	int temp_i = 0;
	sesion_socket_current = new ip::tcp::socket(*service_io_stream);
	output_data = thread_id + " awaiting connection...\n";
	queue_data->Push(output_data);
	acceptor->async_accept(*sesion_socket_current, boost::bind(&Thread_connection_receiving::Process_Connections_Handler, this));
	while (ENABLE_CONNECTION_RECEIVING)
	{
		service_io_stream->run_one_for(chrono::milliseconds(1000));		
	}
	acceptor->close();
	return 0;
}


Thread_connection_receiving::Thread_connection_receiving(io_service * service_io_stream_New, unsigned short port_New, Queue_thread_safe *queue_data_new)
{
	queue_data = queue_data_new;
	service_io_stream = service_io_stream_New;
	port = port_New;
}

void Thread_connection_receiving::Set_Thread_Id()
{
	if (is_started)
	{
		thread_id = boost::lexical_cast<std::string>(boost::this_thread::get_id());
	}
}

string Thread_connection_receiving::Get_Thread_ID()
{
	return thread_id;
}

int Thread_connection_receiving::Start()
{
	Set_Thread_Id();
	target = new ip::tcp::endpoint(ip::tcp::v4(), port);
	acceptor = new ip::tcp::acceptor(*service_io_stream, *target);
	timer_current = new deadline_timer(*service_io_stream);
	is_initialized = true;
	boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
	Process_Connections();
	return 0;
}

int Thread_connection_receiving::Create_Start()
{
	if (!is_started)
	{
		thread = new boost::thread(boost::bind(&Thread_connection_receiving::Start, this));
		is_started = true;
	}
	return 0;
}

int Thread_connection_receiving::Wait_Stop()
{
	thread->join();
	return 0;
}

boost::thread::id Thread_connection_receiving::Get_ID()
{
	return thread->get_id();
}


