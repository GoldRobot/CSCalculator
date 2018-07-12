#include "server_main.h"

#define ENABLE_CONNECTION_RECEIVING true
#define CLOSE_AFTER_CLIENT_CONNECT false

int Client_Sesion_Process(ip::tcp::socket *sesion_socket)
{

	//cout << "Thread " << boost::this_thread::get_id() << " got the connection!\n" << endl;
	cout << "Socket adress: " << sesion_socket << endl;
	if (CLOSE_AFTER_CLIENT_CONNECT)
	{
		sesion_socket->close();
		//sesion_socket->release();

	}
	else {
		while (true)
		{

			boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
		}
	}
	sesion_socket->close();
	//sesion_socket->release();
	delete sesion_socket;
	return 0;
}

int Thread_connection_receiving::Process_Connections_Handler()
{
	boost::thread(boost::bind(&Client_Sesion_Process, sesion_socket_current));
	//cout << "sesion_socket_current adress: " << sesion_socket_current << endl;
	sesion_socket_current = new ip::tcp::socket(*service_io_stream);
	//cout << "sesion_socket_current adress: " << sesion_socket_current << endl;
	acceptor->async_accept(*sesion_socket_current, boost::bind(&Thread_connection_receiving::Process_Connections_Handler, this));
	
	return 0;
}

int Thread_connection_receiving::Process_Connections()
{
	int temp_i = 0;
	sesion_socket_current = new ip::tcp::socket(*service_io_stream);
	acceptor->async_accept(*sesion_socket_current, boost::bind(&Thread_connection_receiving::Process_Connections_Handler, this));
	while (ENABLE_CONNECTION_RECEIVING)
	{
		service_io_stream->run_one_for(chrono::milliseconds(1000));		
	}
	acceptor->close();
	Test_Inc();
	Test_Inc();
	//cout << "Test: " << Get_Test() << endl;
	return 0;
}


Thread_connection_receiving::Thread_connection_receiving(io_service * service_io_stream_New, unsigned short port_New)
{
	service_io_stream = service_io_stream_New;
	port = port_New;
}

int Thread_connection_receiving::Start()
{
	//cout << "Start of thread ID: " << boost::this_thread::get_id() << endl;
	target = new ip::tcp::endpoint(ip::tcp::v4(), port);
	acceptor = new ip::tcp::acceptor(*service_io_stream, *target);
	timer_current = new deadline_timer(*service_io_stream);
	is_initialized = true;
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


