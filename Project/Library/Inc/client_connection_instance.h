#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <string>

#include "thread_safe_data_structures.h"

class Client_Connection_Instance
{
private:
	
	unsigned int port;
	int id;
	int test_cycle = 0;
	std::string adress;
	boost::asio::ip::tcp::endpoint *target_point;
	boost::asio::io_service *io_service_connection;
	Thread_safe_print_queue *print_queue;

	boost::asio::streambuf *read_buffer;
	boost::asio::ip::tcp::socket *session_socket;

	std::string login, password, expression;
	int Command_Recognition(std::string message);

public:
	void Received_Message_Handler(const boost::system::error_code & error, size_t byte_received);
	int Initialization();
	Client_Connection_Instance(boost::asio::io_service *io_service_connection_new, Thread_safe_print_queue *print_queue_new, std::string adress_new, unsigned int port_new, int id_new);
	Client_Connection_Instance(const Client_Connection_Instance &obj);
	~Client_Connection_Instance();
};