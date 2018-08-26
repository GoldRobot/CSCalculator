#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <queue>

#include "server_connection_instance.h"
#include "calculator.hpp"
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

constexpr const auto ENABLE_CONNECTION_RECEIVING = true;
constexpr const auto CLOSE_AFTER_CLIENT_CONNECT = false;
constexpr const auto PRINT_TALK = false;
constexpr const auto PRINT_SERVICE_INFO = true;

constexpr const auto THREAD_POOL_ACCEPTORS_AMOUNT = 20;
constexpr const auto THREAD_POOL_CONNECTION_TALKERS_AMOUNT = 500;
constexpr const auto THREAD_POOL_CALCULATORS_AMOUNT = 100;

/*
const string MESSAGE_DELIMITER("\r\n");
const string MESSAGE_CMD_LOGIN("login ");
const string MESSAGE_CMD_LOGOUT("logout ");
const string MESSAGE_CMD_PASSWORD("password ");
const string MESSAGE_CMD_CALC("calc ");
const string MESSAGE_CMD_RESULT("result ");

const size_t MESSAGE_DELIMITER_SIZE(MESSAGE_DELIMITER.size());
const size_t MESSAGE_CMD_LOGIN_SIZE(MESSAGE_CMD_LOGIN.size());
const size_t MESSAGE_CMD_LOGOUT_SIZE(MESSAGE_CMD_LOGOUT.size());
const size_t MESSAGE_CMD_PASSWORD_SIZE(MESSAGE_CMD_PASSWORD.size());
const size_t MESSAGE_CMD_CALC_SIZE(MESSAGE_CMD_CALC.size());
const size_t MESSAGE_CMD_RESULT_SIZE(MESSAGE_CMD_RESULT.size());
*/



string Read_And_Consume_From_Streambuf(streambuf &buffer, size_t message_size, size_t delimiter_size)
{
	using boost::asio::buffers_begin;
	string result(buffers_begin(buffer.data()), buffers_begin(buffer.data()) + message_size - delimiter_size);
	buffer.consume(message_size);
	return result;
}

int Server_Connection_Instance::Command_Recognition(string message)
{
	if (message.find(MESSAGE_TYPE.TO_SERVER.TEXT.LOGIN) == 0)
	{
		return MESSAGE_TYPE.TO_SERVER.CODE.LOGIN;
	}
	if (message.find(MESSAGE_TYPE.TO_SERVER.TEXT.PASSWORD) == 0)
	{
		return MESSAGE_TYPE.TO_SERVER.CODE.PASSWORD;
	}
	if (message.find(MESSAGE_TYPE.TO_SERVER.TEXT.CALC) == 0)
	{
		return MESSAGE_TYPE.TO_SERVER.CODE.CALC;
	}
	if (message.compare(MESSAGE_TYPE.TO_SERVER.TEXT.LOGOUT) == 0)
	{
		return MESSAGE_TYPE.TO_SERVER.CODE.LOGOUT;
	}
	if (message.compare(MESSAGE_TYPE.TO_SERVER.TEXT.RESULT) == 0)
	{
		return MESSAGE_TYPE.TO_SERVER.CODE.RESULT;
	}
	return MESSAGE_TYPE.TO_SERVER.CODE.UNKNOWN;
}

void Server_Connection_Instance::Calculate_Expression()
{
	string data_to_send;
	State_Work_In_Progress_Set(true);
	try {
		calculation_result = calculator::eval(expression);
		data_to_send = MESSAGE_TYPE.TO_CLIENT.TEXT.RESULT + "Calculated :" + to_string(calculation_result);
		State_Work_Ready_Set(true);
	}
	catch (calculator::error&)
	{
		data_to_send = MESSAGE_TYPE.TO_CLIENT.TEXT.WRONG + "Wrong expression: " + expression;
		State_Work_Ready_Set(false);
	}
	if (PRINT_TALK)
	{
		print_queue->Push(data_to_send + "\n");
	}
	session_socket->send(buffer(data_to_send + MESSAGE_TYPE.DELIMITER));
	State_Work_In_Progress_Set(false);
	Can_Be_Deleted_Set();
}

void Server_Connection_Instance::Initialization()
{
	state = CONNECTION_STATE_CONNECTED;
	if (PRINT_SERVICE_INFO)
	{
		//print_queue->Push(string("Client " + to_string(id) + " initialized.\n"));
	}
	read_buffer = new streambuf(512);
	string data_to_send = "Hello from server!";
	session_socket->send(buffer(data_to_send + MESSAGE_TYPE.DELIMITER));
	async_read_until(*session_socket, *read_buffer, MESSAGE_TYPE.DELIMITER, boost::bind(&Server_Connection_Instance::Received_Message_Handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void Server_Connection_Instance::Received_Message_Handler(const boost::system::error_code & error, size_t byte_received)
{
	string data_to_print;
	if (error.value() != 0)
	{
		if (PRINT_SERVICE_INFO)
		{
			print_queue->Push(string("Client dissconected (" + error.message() + ").\n"));
		}
		state = CONNECTION_STATE_DISCONNECTED;
		session_socket->close();
		Can_Be_Deleted_Set();
		return;
	}
	string received_message = Read_And_Consume_From_Streambuf(*read_buffer, byte_received, MESSAGE_TYPE.DELIMITER_SIZE);
	string data_to_send;
	int command = Command_Recognition(received_message);
	try
	{
		switch (command)
		{
		case MESSAGE_TYPE.TO_SERVER.CODE.UNKNOWN:
			data_to_print = "Received unknown message: " + received_message + "\n";
			data_to_send = MESSAGE_TYPE.TO_CLIENT.TEXT.ERR + "Received unknown message";
			session_socket->send(buffer(data_to_send + MESSAGE_TYPE.DELIMITER));
			break;
		case MESSAGE_TYPE.TO_SERVER.CODE.LOGIN:
			if (state != CONNECTION_STATE_CONNECTED)
			{
				data_to_send = MESSAGE_TYPE.TO_CLIENT.TEXT.ERR + "Already logged in with name : " + login;
				data_to_print = data_to_send + "\n";
				session_socket->send(buffer(data_to_send + MESSAGE_TYPE.DELIMITER));
				break;
			}
			login = received_message.substr(MESSAGE_TYPE.TO_SERVER.SIZE.LOGIN);
			state = CONNECTION_STATE_AURIZATION_LOGIN;
			data_to_send = MESSAGE_TYPE.TO_CLIENT.TEXT.RIGHT + "Accepted username: " + login;
			data_to_send += ". Please send password.";
			data_to_print = data_to_send + "\n";
			session_socket->send(buffer(data_to_send + MESSAGE_TYPE.DELIMITER));
			break;
		case MESSAGE_TYPE.TO_SERVER.CODE.PASSWORD:
			if (state == CONNECTION_STATE_AUTORIZED)
			{
				data_to_send = MESSAGE_TYPE.TO_CLIENT.TEXT.ERR + "Already autorzed with name: " + login;
				data_to_print = data_to_send + "\n";
				session_socket->send(buffer(data_to_send + MESSAGE_TYPE.DELIMITER));
				break;
			}
			else
			{
				if (state == CONNECTION_STATE_CONNECTED)
				{
					data_to_send = MESSAGE_TYPE.TO_CLIENT.TEXT.ERR + "Username first" + login;
					data_to_print = data_to_send + "\n";
					session_socket->send(buffer(data_to_send + MESSAGE_TYPE.DELIMITER));
					break;
				}
				else
				{
					if (state == CONNECTION_STATE_DISCONNECTED)
					{
						data_to_print = "WHAT? RECEIVED MESSAGE WHEN DISCONNECTED? HOW? Recieved message:" + received_message + "\n";
						break;
					}
				}
			}
			password = received_message.substr(MESSAGE_TYPE.TO_SERVER.SIZE.PASSWORD);
			state = CONNECTION_STATE_AUTORIZED;
			data_to_send = MESSAGE_TYPE.TO_CLIENT.TEXT.RIGHT + "Autorzed with name " + login + " and password " + password;
			data_to_print = data_to_send + "\n";
			session_socket->send(buffer(data_to_send + MESSAGE_TYPE.DELIMITER));
			break;
		case MESSAGE_TYPE.TO_SERVER.CODE.CALC:
			if (State_Work_In_Progress_Get() == true)
			{
				data_to_send = MESSAGE_TYPE.TO_CLIENT.TEXT.BUSY + "Busy with: " + expression;
				data_to_print = data_to_send + "\n";
				session_socket->send(buffer(data_to_send + MESSAGE_TYPE.DELIMITER));
				break;
			}
			else {
				if (state != CONNECTION_STATE_AUTORIZED)
				{
					data_to_send = MESSAGE_TYPE.TO_CLIENT.TEXT.WRONG + "Autorize to request work";
					data_to_print = data_to_send + "\n";
					session_socket->send(buffer(data_to_send + MESSAGE_TYPE.DELIMITER));
					break;
				}
			}
			expression = received_message.substr(MESSAGE_TYPE.TO_SERVER.SIZE.CALC);
			State_Work_In_Progress_Set(true);
			data_to_send = MESSAGE_TYPE.TO_CLIENT.TEXT.RIGHT + "Start to calculate: " + expression;
			data_to_print = data_to_send + "\n";
			session_socket->send(buffer(data_to_send + MESSAGE_TYPE.DELIMITER));
			io_service_calculations->post(boost::bind(&Server_Connection_Instance::Calculate_Expression, this));
			break;
		case MESSAGE_TYPE.TO_SERVER.CODE.RESULT:
			if (state != CONNECTION_STATE_AUTORIZED)
			{
				data_to_send = MESSAGE_TYPE.TO_CLIENT.TEXT.WRONG + "Autorize to request result";
				data_to_print = data_to_send + "\n";
				session_socket->send(buffer(data_to_send + MESSAGE_TYPE.DELIMITER));
				break;
			}
			else {
				if (State_Work_Ready_Get() == false)
				{
					data_to_send = MESSAGE_TYPE.TO_CLIENT.TEXT.WRONG + "Work in progress";
					data_to_print = data_to_send + "\n";
					session_socket->send(buffer(data_to_send + MESSAGE_TYPE.DELIMITER));
					break;
				}
			}
			data_to_send = MESSAGE_TYPE.TO_CLIENT.TEXT.RIGHT + "Result: " + to_string(calculation_result);
			data_to_print = data_to_send + "\n";
			session_socket->send(buffer(data_to_send + MESSAGE_TYPE.DELIMITER));
			break;
		case MESSAGE_TYPE.TO_SERVER.CODE.LOGOUT:
			state = CONNECTION_STATE_CONNECTED;
			data_to_send = MESSAGE_TYPE.TO_CLIENT.TEXT.RIGHT + "Received logout request";
			data_to_print = data_to_send + "\n";
			session_socket->send(buffer(data_to_send + MESSAGE_TYPE.DELIMITER));
			login.clear();
			password.clear();
			break;
		default:
			data_to_print = "WHAT? DEFAULT SWITCH? WHY? Received unknown message: " + received_message + "\n";
			break;
		}
		if (PRINT_TALK)
		{
			print_queue->Push(data_to_print);
		}
	}
	catch (boost::system::system_error e)
	{
		if (PRINT_SERVICE_INFO)
		{
			print_queue->Push(string("Client dissconected (" + error.message() + ").\n"));
		}
		state = CONNECTION_STATE_DISCONNECTED;
		session_socket->close();
		Can_Be_Deleted_Set();
		return;
	}
	async_read_until(*session_socket, *read_buffer, MESSAGE_TYPE.DELIMITER, boost::bind(&Server_Connection_Instance::Received_Message_Handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void Server_Connection_Instance::Disconnect()
{
	session_socket->close();
}

bool Server_Connection_Instance::Can_Be_Deleted_Get()
{
	boost::mutex::scoped_lock lock(mutex_point_can_be_deleted);
	return can_be_deleted;
}

void Server_Connection_Instance::State_Work_In_Progress_Set(bool state_new)
{
	boost::mutex::scoped_lock lock(mutex_point_can_be_deleted);
	state_work_in_progress = state_new;
}

bool Server_Connection_Instance::State_Work_In_Progress_Get()
{
	boost::mutex::scoped_lock lock(mutex_point_can_be_deleted);
	return state_work_in_progress;
}

void Server_Connection_Instance::State_Work_Ready_Set(bool state_new)
{
	boost::mutex::scoped_lock lock(mutex_point_state_work_ready);
	state_work_ready = state_new;
}

bool Server_Connection_Instance::State_Work_Ready_Get()
{
	boost::mutex::scoped_lock lock(mutex_point_state_work_ready);
	return state_work_ready;
}

int Server_Connection_Instance::Id_Get()
{
	return id;
}

void Server_Connection_Instance::Can_Be_Deleted_Set()
{
	boost::mutex::scoped_lock lock(mutex_point_can_be_deleted);
	if ((state_work_in_progress == false) && (state==CONNECTION_STATE_DISCONNECTED))
	{
		can_be_deleted = true;
	}
	else {
		can_be_deleted = false;
	}
}

Server_Connection_Instance::Server_Connection_Instance(io_service *io_service_calculations_new, Thread_safe_print_queue *print_queue_new, Thread_safe_server_info *server_info_new, tcp::socket *sesion_socket_new, int id_new)
{
	id = id_new;
	print_queue = print_queue_new;
	server_info = server_info_new;
	session_socket = sesion_socket_new;
	io_service_calculations = io_service_calculations_new;
	state = CONNECTION_STATE_CONNECTED;
}

Server_Connection_Instance::Server_Connection_Instance(const Server_Connection_Instance & obj)
{
	id = obj.id;
	print_queue = obj.print_queue;
	server_info = obj.server_info;
	session_socket = obj.session_socket;
	io_service_calculations = obj.io_service_calculations;
	state = CONNECTION_STATE_CONNECTED;
}

Server_Connection_Instance::~Server_Connection_Instance()
{
	session_socket->close();
	delete session_socket;
	string data_to_print;
	delete read_buffer;
	if (PRINT_SERVICE_INFO)
	{
		print_queue->Push(string("Client "+to_string(id) +" destroyed.\n"));
	}
}