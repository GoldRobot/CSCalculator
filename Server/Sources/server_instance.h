#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <string>
#include <list>

#include "thread_safe_data_structures.h"
#include "client_instance.h"

class Server_instance
{
private:
	std::list <Client_Instance> clients_list;
	boost::mutex mutex_point_client_list;

	Thread_safe_print_queue *print_queue;
	Thread_safe_server_info *server_info;
	boost::mutex mutex_point;

	boost::asio::io_service io_service_connection_accept;
	boost::asio::io_service io_service_connection_talk;
	boost::asio::io_service io_service_calculations;

	bool is_initialized = false;
	bool is_started = false;

	boost::thread *thread;
	std::string thread_id_os = "unknown";
	int thread_id_internal = -1;

	unsigned short port = 0;
	boost::asio::ip::tcp::endpoint *target;
	boost::asio::ip::tcp::acceptor *acceptor;

	int connections_amount = 0;
	boost::mutex mutex_point_connections_amount;

	void Process_Conection_Request_Handler(boost::asio::ip::tcp::socket *sesion_socket_for_acceptor);
	void Process_Conections();
	int Start();
	void Connections_Amount_Change(int new_number);
	void Connections_Amount_Change(bool increase);
public:
	std::string Thread_ID_OS_Get();
	int Thread_Create_Start();
	void Interrupt();
	void Wait_Join();
	unsigned short Port_Get();
	int Connections_Amount_Get();
	Server_instance(unsigned short port_New, Thread_safe_print_queue *print_queue_new, Thread_safe_server_info *server_info_new, int new_internal_id);
	Server_instance(const Server_instance &obj);
	~Server_instance();
};


