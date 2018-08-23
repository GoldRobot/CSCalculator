#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <queue>

#include "thread_safe_data_structures.h"
#include "client_instance.h"
#include "calculator.hpp"

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

constexpr const auto ENABLE_CONNECTION_RECEIVING = true;
constexpr const auto CLOSE_AFTER_CLIENT_CONNECT = false;
constexpr const auto PRINT_TALK = false;
constexpr const auto PRINT_SERVICE_INFO = false;

constexpr const auto THREAD_POOL_ACCEPTORS_AMOUNT = 10;
constexpr const auto THREAD_POOL_CONNECTION_TALKERS_AMOUNT = 3;
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

const enum COMMANDS
{
	COMMAND_UNKNOWN,
	COMMAND_LOGIN,
	COMMAND_LOGOUT,
	COMMAND_PASSWORD,
	COMMAND_CALCULATE,
	COMMAND_DISCONNECT,
	COMMAND_RESULT
};

string Read_And_Consume_From_Streambuf(streambuf &buffer, size_t message_size, size_t delimiter_size)
{
	using boost::asio::buffers_begin;
	string result(buffers_begin(buffer.data()), buffers_begin(buffer.data()) + message_size - delimiter_size);
	buffer.consume(message_size);
	return result;
}

static const struct MESSAGES_INFORMATION
{
	struct text_of_messages
	{
		string DELIMITER = "\r\n";
		string LOGIN = "login ";
		string LOGOUT = "logout ";
		string PASSWORD = "password ";
		string CALC = "calc ";
		string RESULT = "result ";
	} TEXT;

	struct sizes_of_messages
	{
		size_t DELIMITER = string("\r\n").size();
		size_t LOGIN = string("login ").size();
		size_t LOGOUT = string("logout ").size();
		size_t PASSWORD = string("password ").size();
		size_t CALC = string("calc ").size();
		size_t RESULT = string("result ").size();
	} SIZE;

}MESSAGE;

int Client_Instance::Command_Recognition(string message)
{
	if (message.find(MESSAGE.TEXT.LOGIN) == 0)
	{
		return COMMAND_LOGIN;
	}
	if (message.find(MESSAGE.TEXT.PASSWORD) == 0)
	{
		return COMMAND_PASSWORD;
	}
	if (message.find(MESSAGE.TEXT.CALC) == 0)
	{
		return COMMAND_CALCULATE;
	}
	if (message.compare(MESSAGE.TEXT.LOGOUT) == 0)
	{
		return COMMAND_LOGOUT;
	}
	return COMMAND_UNKNOWN;
}

void Client_Instance::Calculate_Expression()
{
	string data_to_print;
	State_Work_In_Progress_Set(true);
	try {
		calculation_result = calculator::eval(expression);
		data_to_print = "Calculated (" + expression + "): " + to_string(calculation_result) + "\n";
		State_Work_Ready_Set(true);
	}
	catch (calculator::error&)
	{
		data_to_print = "Expression " + expression + " are not correct\n";
		State_Work_Ready_Set(false);
	}
	if (PRINT_TALK)
	{
		print_queue->Push(data_to_print);
	}
	State_Work_In_Progress_Set(false);
	Can_Be_Deleted_Set();
}

void Client_Instance::Initialization()
{
	state = CLIENT_STATE_CONNECTED;
	if (PRINT_SERVICE_INFO)
	{
		print_queue->Push(string("Client " + to_string(id) + " initialized.\n"));
	}
	read_buffer = new streambuf(2048);
	async_read_until(*session_socket, *read_buffer, MESSAGE.TEXT.DELIMITER, boost::bind(&Client_Instance::Received_Message_Handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void Client_Instance::Received_Message_Handler(const boost::system::error_code & error, size_t byte_received)
{
	string data_to_print;
	if (error.value() != 0)
	{
		if (PRINT_SERVICE_INFO)
		{
			print_queue->Push(string("Client dissconected (" + error.message() + ").\n"));
		}
		state = CLIENT_STATE_DISCONNECTED;
		session_socket->close();
		Can_Be_Deleted_Set();
		return;
	}
	string received_message = Read_And_Consume_From_Streambuf(*read_buffer, byte_received, MESSAGE.SIZE.DELIMITER);
	int command = Command_Recognition(received_message);
	switch (command)
	{
	case COMMAND_UNKNOWN:
		data_to_print = "Received unknown message: " + received_message + "\n";
		break;
	case COMMAND_LOGIN:
		if (state != CLIENT_STATE_CONNECTED)
		{
			data_to_print = "Already logged in with name: " + login + "\n";
			break;
		}
		login = received_message.substr(MESSAGE.SIZE.LOGIN);
		data_to_print = "Received login: " + login + " \n";
		state = CLIENT_STATE_AURIZATION_LOGIN;
		break;
	case COMMAND_PASSWORD:
		if (state == CLIENT_STATE_AUTORIZED)
		{
			data_to_print = "Already autorized with name: " + login + "\n";
			break;
		}
		else
		{
			if (state == CLIENT_STATE_CONNECTED)
			{
				data_to_print = "Login first, recieved message: " + received_message + "\n";
				break;
			}
			else
			{
				if (state == CLIENT_STATE_DISCONNECTED)
				{
					data_to_print = "WHAT? RECEIVED MESSAGE WHEN DISCONNECTED? HOW? Recieved message:" + received_message + "\n";
					break;
				}
			}
		}
		password = received_message.substr(MESSAGE.SIZE.PASSWORD);
		data_to_print = "Received password: " + password + " \n";
		state = CLIENT_STATE_AUTORIZED;
		break;
	case COMMAND_CALCULATE:
		if (State_Work_In_Progress_Get() == true)
		{
			data_to_print = "There is a work in proggress already, be patient, please.\nExpression in work: " + expression + "\n";
			print_queue->Push(data_to_print);
			break;
		}
		else {
			if (state != CLIENT_STATE_AUTORIZED)
			{
				data_to_print = "Need to autorize fistly, recieved message: " + received_message + "\n";
				print_queue->Push(data_to_print);
				break;
			}
		}
		expression = received_message.substr(MESSAGE.SIZE.CALC);
		State_Work_In_Progress_Set(true);
		data_to_print = "Calculate: " + expression + " \n";
		io_service_calculations->post(boost::bind(&Client_Instance::Calculate_Expression, this));
		break;
	case COMMAND_RESULT:
		if (state != CLIENT_STATE_AUTORIZED)
		{
			data_to_print = "Need to autorize fistly, recieved message: " + received_message + "\n";
			break;
		}
		else {
			if (State_Work_Ready_Get() == false)
			{

				data_to_print = "Result: " + to_string(calculation_result) + "\n";
				break;
			}
		}
		break;
	case COMMAND_LOGOUT:
		data_to_print = "Received logout request\n";
		state = CLIENT_STATE_CONNECTED;
		break;
	case COMMAND_DISCONNECT:

		break;
	default:
		data_to_print = "WHAT? DEFAULT SWITCH? WHY? Received unknown message: " + received_message + "\n";
		break;
	}
	if (PRINT_TALK)
	{
		print_queue->Push(data_to_print);
	}
	async_read_until(*session_socket, *read_buffer, MESSAGE.TEXT.DELIMITER, boost::bind(&Client_Instance::Received_Message_Handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void Client_Instance::Disconnect()
{
	session_socket->close();
}

bool Client_Instance::Can_Be_Deleted_Get()
{
	boost::mutex::scoped_lock lock(mutex_point_can_be_deleted);
	return can_be_deleted;
}

void Client_Instance::State_Work_In_Progress_Set(bool state_new)
{
	boost::mutex::scoped_lock lock(mutex_point_can_be_deleted);
	state_work_in_progress = state_new;
}

bool Client_Instance::State_Work_In_Progress_Get()
{
	boost::mutex::scoped_lock lock(mutex_point_can_be_deleted);
	return state_work_in_progress;
}

void Client_Instance::State_Work_Ready_Set(bool state_new)
{
	boost::mutex::scoped_lock lock(mutex_point_state_work_ready);
	state_work_ready = state_new;
}

bool Client_Instance::State_Work_Ready_Get()
{
	boost::mutex::scoped_lock lock(mutex_point_state_work_ready);
	return state_work_ready;
}

int Client_Instance::Id_Get()
{
	return id;
}

void Client_Instance::Can_Be_Deleted_Set()
{
	boost::mutex::scoped_lock lock(mutex_point_can_be_deleted);
	if ((state_work_in_progress == false) && (state==CLIENT_STATE_DISCONNECTED))
	{
		can_be_deleted = true;
	}
	else {
		can_be_deleted = false;
	}
}

Client_Instance::Client_Instance(io_service *io_service_calculations_new, Thread_safe_print_queue *queue_print_new, Thread_safe_server_info *server_info_new, tcp::socket *sesion_socket_new, int id_new)
{
	id = id_new;
	print_queue = queue_print_new;
	server_info = server_info_new;
	session_socket = sesion_socket_new;
	io_service_calculations = io_service_calculations_new;
}

Client_Instance::Client_Instance(const Client_Instance & obj)
{
	id = obj.id;
	print_queue = obj.print_queue;
	server_info = obj.server_info;
	session_socket = obj.session_socket;
	io_service_calculations = obj.io_service_calculations;
}

Client_Instance::~Client_Instance()
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