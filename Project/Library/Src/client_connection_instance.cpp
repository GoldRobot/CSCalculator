#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <queue>

#include "client_connection_instance.h"
#include "states_messages.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::size_t;
using std::to_string;
using boost::asio::streambuf;
using boost::asio::ip::tcp;
using boost::asio::io_service;
using boost::asio::placeholders::error;
using boost::asio::placeholders::bytes_transferred;
using boost::asio::buffer;

constexpr const auto PRINT_TALK = false;
constexpr const auto PRINT_SERVICE_INFO = true;


string Read_And_Consume_From_Streambuf(streambuf &buffer, size_t message_size, size_t delimiter_size)
{
	using boost::asio::buffers_begin;
	string result(buffers_begin(buffer.data()), buffers_begin(buffer.data()) + message_size - delimiter_size);
	buffer.consume(message_size);
	return result;
}

int Client_Connection_Instance::Command_Recognition(std::string message)
{
	if (message.find(MESSAGE_TYPE.TO_CLIENT.TEXT.RESULT) == 0)
	{
		return MESSAGE_TYPE.TO_CLIENT.CODE.RESULT;
	}
	if (message.find(MESSAGE_TYPE.TO_CLIENT.TEXT.ERR) == 0)
	{
		return MESSAGE_TYPE.TO_CLIENT.CODE.ERR;
	}
	if (message.find(MESSAGE_TYPE.TO_CLIENT.TEXT.TIMEOUT) == 0)
	{
		return MESSAGE_TYPE.TO_CLIENT.CODE.TIMEOUT;
	}
	if (message.find(MESSAGE_TYPE.TO_CLIENT.TEXT.NO_MONEY) == 0)
	{
		return MESSAGE_TYPE.TO_CLIENT.CODE.NO_MONEY;
	}
	if (message.find(MESSAGE_TYPE.TO_CLIENT.TEXT.BUSY) == 0)
	{
		return MESSAGE_TYPE.TO_CLIENT.CODE.BUSY;
	}
	if (message.find(MESSAGE_TYPE.TO_CLIENT.TEXT.RIGHT) == 0)
	{
		return MESSAGE_TYPE.TO_CLIENT.CODE.RIGHT;
	}
	if (message.find(MESSAGE_TYPE.TO_CLIENT.TEXT.WRONG) == 0)
	{
		return MESSAGE_TYPE.TO_CLIENT.CODE.WRONG;
	}
	return MESSAGE_TYPE.TO_SERVER.CODE.UNKNOWN;
}

void Client_Connection_Instance::Received_Message_Handler(const boost::system::error_code & error, size_t byte_received)
{	

	string data_to_print;
	if (error.value() != 0)
	{
		if (PRINT_SERVICE_INFO)
		{
			print_queue->Push(string(to_string(id) + "Client dissconected (" + error.message() + ").\n"));
		}
		session_socket->close();
		return;
	}
	string received_message = Read_And_Consume_From_Streambuf(*read_buffer, byte_received, MESSAGE_TYPE.DELIMITER_SIZE);
	int command = Command_Recognition(received_message);
	switch (command)
	{
	case MESSAGE_TYPE.TO_CLIENT.CODE.RESULT:
		data_to_print = "Serv: " + received_message + "\n";
		if (!PRINT_TALK)
		{
			print_queue->Push(data_to_print);
		}
		break;
	case MESSAGE_TYPE.TO_CLIENT.CODE.ERR:
		data_to_print = "Serv: " + received_message + "\n";
		break;
	case MESSAGE_TYPE.TO_CLIENT.CODE.TIMEOUT:
		data_to_print = "Serv: " + received_message + "\n";
		break;
	case MESSAGE_TYPE.TO_CLIENT.CODE.NO_MONEY:
		data_to_print = "Serv: " + received_message + "\n";
		break;
	case MESSAGE_TYPE.TO_CLIENT.CODE.BUSY:
		data_to_print = "Serv: " + received_message + "\n";
		break;
	case MESSAGE_TYPE.TO_CLIENT.CODE.RIGHT:
		data_to_print = "Serv: " + received_message + "\n";
		break;
	case MESSAGE_TYPE.TO_CLIENT.CODE.WRONG:
		data_to_print = "Serv: " + received_message + "\n";
		break;
	case MESSAGE_TYPE.TO_CLIENT.CODE.UNKNOWN:
		data_to_print = "Serv: " + received_message + "\n";
		break;
	default:
		data_to_print = to_string(id) + "WHAT? DEFAULT SWITCH? WHY? Received unknown message: " + received_message + "\n";
		break;
	}
	if (PRINT_TALK)
	{
		print_queue->Push(data_to_print);
	}
	string send_data;
	string print_data;
	try
	{
		switch (test_cycle)
		{
		case 0:
			send_data = "abasdf";
			session_socket->send(buffer(send_data + MESSAGE_TYPE.DELIMITER));
			test_cycle++;
			break;
		case 1:
			send_data = MESSAGE_TYPE.TO_SERVER.TEXT.LOGIN + "GOLDROBOT";
			session_socket->send(buffer(send_data + MESSAGE_TYPE.DELIMITER));
			test_cycle++;
			break;
		case 2:
			send_data = MESSAGE_TYPE.TO_SERVER.TEXT.PASSWORD + "AVBD";
			session_socket->send(buffer(send_data + MESSAGE_TYPE.DELIMITER));
			test_cycle++;
			break;
		case 3:
			send_data = MESSAGE_TYPE.TO_SERVER.TEXT.CALC + "(2+3+2)*3*(2304023+23423*3235^4323)+32394+42";
			session_socket->send(buffer(send_data + MESSAGE_TYPE.DELIMITER));
			test_cycle++;
			break;
		case 4:
			send_data = MESSAGE_TYPE.TO_SERVER.TEXT.RESULT;
			session_socket->send(buffer(send_data + MESSAGE_TYPE.DELIMITER));
			test_cycle++;
			break;
		case 5:
			send_data = MESSAGE_TYPE.TO_SERVER.TEXT.LOGOUT;
			session_socket->send(buffer(send_data + MESSAGE_TYPE.DELIMITER));
			test_cycle++;
			break;
		case 6:
			send_data = MESSAGE_TYPE.TO_SERVER.TEXT.CALC + "(2+3+2)*3*(2304023+23423*3235^4323)+32394+42";
			session_socket->send(buffer(send_data + MESSAGE_TYPE.DELIMITER));
			test_cycle++;
			break;
		case 7:
			send_data = MESSAGE_TYPE.TO_SERVER.TEXT.RESULT;
			session_socket->send(buffer(send_data + MESSAGE_TYPE.DELIMITER));
			test_cycle++;
			break;
		default:
			send_data = MESSAGE_TYPE.TO_SERVER.TEXT.LOGOUT;
			session_socket->send(buffer(send_data + MESSAGE_TYPE.DELIMITER));
			test_cycle = 0;
			break;
		}
		if (PRINT_TALK)
		{
			print_data = to_string(id) + " Client " + send_data + "\n\n";
			print_queue->Push(print_data);
		}
	}
	catch (boost::system::system_error e)
	{
		print_data = to_string(id) + " Client disconected\n";
		print_queue->Push(print_data);
		return;
	}
	boost::this_thread::sleep_for(boost::chrono::milliseconds(50));
	async_read_until(*session_socket, *read_buffer, MESSAGE_TYPE.DELIMITER, boost::bind(&Client_Connection_Instance::Received_Message_Handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

int Client_Connection_Instance::Initialization()
{
	read_buffer = new streambuf(512);
	session_socket = new tcp::socket(*io_service_connection);
	try
	{
		session_socket->connect(*target_point);
	}
	catch (boost::system::system_error e)
	{
		print_queue->Push(string(to_string(id) + " Connection failed (" + to_string(e.code().value()) + ")\n"));
		return -1;
	}
	if (PRINT_SERVICE_INFO)
	{
		print_queue->Push(string("Client connected " + to_string(id) + "\n"));
	}
	async_read_until(*session_socket, *read_buffer, MESSAGE_TYPE.DELIMITER, boost::bind(&Client_Connection_Instance::Received_Message_Handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

Client_Connection_Instance::Client_Connection_Instance(boost::asio::io_service *io_service_connection_new, Thread_safe_print_queue *print_queue_new, std::string adress_new, unsigned int port_new, int id_new)
{
	id = id_new;
	port = port_new;
	adress = adress_new;
	target_point = new boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(adress), port);
	print_queue = print_queue_new;
	io_service_connection = io_service_connection_new;
}

Client_Connection_Instance::Client_Connection_Instance(const Client_Connection_Instance & obj)
{
	id = obj.id;
	port = obj.port;
	adress = obj.adress;
	target_point = obj.target_point;
	print_queue = obj.print_queue;
	io_service_connection = obj.io_service_connection;
}

Client_Connection_Instance::~Client_Connection_Instance()
{
	session_socket->close();
	delete session_socket;
	delete read_buffer;
	delete target_point;
	string data_to_print;
	if (PRINT_SERVICE_INFO)
	{
		print_queue->Push(string("Client destroyed.\n"));
	}
}