#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <queue>

#include "server_instance.h"

using std::cout;
using std::cin;
using std::endl;
using std::list;
using std::string;
using std::size_t;
using std::to_string;
using boost::asio::streambuf;
using boost::asio::ip::tcp;
using boost::asio::io_service;
constexpr const auto ENABLE_CONNECTION_RECEIVING = true;
constexpr const auto CLOSE_AFTER_CLIENT_CONNECT = false;
constexpr const auto PRINT_SERVICE_INFO = false;

constexpr const auto THREAD_POOL_ACCEPTORS_AMOUNT = 100;
constexpr const auto THREAD_POOL_CONNECTION_TALKERS_AMOUNT = 100;
constexpr const auto THREAD_POOL_CALCULATORS_AMOUNT = 100;

void Server_instance::Process_Conection_Request_Handler(tcp::socket *session_socket_for_acceptor)
{
	string print_data;
	//print_data = boost::lexical_cast<std::string>(boost::this_thread::get_id()) + "=" + thread_id_os + "("
		//+ to_string(thread_id_internal) + ") accepted connection request on port " + to_string(port) + ".\n";
	//print_queue->Push(print_data);
	static int connection_id = -1;
	boost::mutex::scoped_lock lock(mutex_point_connections_list);
	connection_id++;
	connections_list.emplace_back(&io_service_calculations, print_queue, server_info, session_socket_for_acceptor, connection_id);
	io_service_connection_talk.post(boost::bind(&Server_Connection_Instance::Initialization, &connections_list.back()));
	lock.unlock();
	Connections_Amount_Change(true);
	server_info->Connections_Total_Amount_Change(true);
	tcp::socket *sesion_socket_temp;
	sesion_socket_temp = new tcp::socket(io_service_connection_talk);
	acceptor->async_accept(*sesion_socket_temp, boost::bind(&Server_instance::Process_Conection_Request_Handler, this, sesion_socket_temp));
	return;
}

void Server_instance::Process_Conections()
{
	boost::thread_group thread_pool_acceptors;
	boost::thread_group thread_pool_connection_talk;
	boost::thread_group thread_calculations;

	boost::asio::io_service::work work_acception(io_service_connection_accept);
	boost::asio::io_service::work work_connection_talk(io_service_connection_talk);
	boost::asio::io_service::work work_calculations(io_service_calculations);

	for (int i = 0; i < THREAD_POOL_ACCEPTORS_AMOUNT; i++)
	{
		thread_pool_acceptors.create_thread(boost::bind(&io_service::run, &io_service_connection_accept));
	}
	for (int i = 0; i < THREAD_POOL_CONNECTION_TALKERS_AMOUNT; i++)
	{
		thread_pool_connection_talk.create_thread(boost::bind(&io_service::run, &io_service_connection_talk));
	}
	for (int i = 0; i < THREAD_POOL_CALCULATORS_AMOUNT; i++)
	{
		thread_calculations.create_thread(boost::bind(&io_service::run, &io_service_calculations));
	}

	tcp::socket *sesion_socket_temp;
	for (int i = 0; i < THREAD_POOL_ACCEPTORS_AMOUNT; i++)
	{
		sesion_socket_temp = new tcp::socket(io_service_connection_talk);
		acceptor->async_accept(*sesion_socket_temp, boost::bind(&Server_instance::Process_Conection_Request_Handler, this, sesion_socket_temp));
	}
	//string temp_string;
	while (ENABLE_CONNECTION_RECEIVING)
	{
		try {
			boost::this_thread::sleep_for(boost::chrono::milliseconds(5000));
		}
		catch (boost::thread_interrupted&)
		{
			break;
		}
		//temp_string = "List of clients on port: " + to_string(port) + "\n";
		
		boost::mutex::scoped_lock lock(mutex_point_connections_list);
		list <Server_Connection_Instance>::iterator i = connections_list.begin();
		while (i != connections_list.end())
		{
			if (i->Can_Be_Deleted_Get())
			{
				//temp_string += "Delete client: " + to_string(i->Id_Get()) + "\n";
				Connections_Amount_Change(false);
				server_info->Connections_Total_Amount_Change(false);
				i = connections_list.erase(i);
			}else
			{
				//temp_string += "Client: " + to_string(i->Id_Get()) + "\n";
				i++;
			}
		}
		lock.unlock();
		
		//temp_string += "\n";
		//print_queue->Push(temp_string);
	}
	io_service_connection_accept.stop();
	io_service_connection_talk.stop();
	io_service_calculations.stop();
	thread_pool_acceptors.join_all();
	thread_pool_connection_talk.join_all();
	thread_calculations.join_all();
	sesion_socket_temp->close();
	delete sesion_socket_temp;
	acceptor->close();
	//temp_string = "Acceptor (thread " + thread_id_os + ") on port " + to_string(port) + " closed\n";
	//print_queue->Push(temp_string);
	return;
}

string Server_instance::Thread_ID_OS_Get()
{
	return thread_id_os;
}

int Server_instance::Start()
{
	thread_id_os = boost::lexical_cast<std::string>(boost::this_thread::get_id());
	target = new tcp::endpoint(tcp::v4(), port);
	acceptor = new tcp::acceptor(io_service_connection_accept, *target);
	is_initialized = true;
	Process_Conections();
	return 0;
}

int Server_instance::Thread_Create_Start()
{
	boost::mutex::scoped_lock lock(mutex_point);
	if (!is_started)
	{
		thread = new boost::thread(boost::bind(&Server_instance::Start, this));
		is_started = true;
	}
	return 0;
}

void Server_instance::Connections_Amount_Change(int new_number)
{
	boost::mutex::scoped_lock lock(mutex_point_connections_amount);
	connections_amount = new_number;
}

void Server_instance::Connections_Amount_Change(bool increase)
{
	boost::mutex::scoped_lock lock(mutex_point_connections_amount);
	if (increase)
	{
		connections_amount++;
	}
	else
	{
		connections_amount--;
	}
}

int Server_instance::Connections_Amount_Get()
{
	boost::mutex::scoped_lock lock(mutex_point_connections_amount);
	return connections_amount;
}

void Server_instance::Interrupt()
{
	thread->interrupt();
	return;
}

void Server_instance::Wait_Join()
{
	thread->join();
	return;
}

unsigned short Server_instance::Port_Get()
{
	return port;
}

Server_instance::Server_instance(unsigned short port_new, Thread_safe_print_queue *print_queue_new, Thread_safe_server_info *server_info_new, int new_internal_id)
{
	port = port_new;
	print_queue = print_queue_new;
	server_info = server_info_new;
	thread_id_internal = new_internal_id;
}

Server_instance::Server_instance(const Server_instance &obj)
{
	port = obj.port;
	print_queue = obj.print_queue;
	server_info = obj.server_info;
	thread_id_internal = obj.thread_id_internal;
}

Server_instance::~Server_instance()
{
	/*
	delete target;
	acceptor->close();
	delete acceptor;
	delete thread;
	*/
}

