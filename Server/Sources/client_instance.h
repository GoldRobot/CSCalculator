#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/thread/mutex.hpp>
#include <string>

enum CLIENT_STATES
{
	CLIENT_STATE_CONNECTED,
	CLIENT_STATE_AURIZATION_LOGIN,
	CLIENT_STATE_AUTORIZED,
	CLIENT_STATE_DISCONNECTED
};

class Client_Instance
{
private:
	boost::asio::io_service *io_service_calculations;
	Thread_safe_print_queue *print_queue;
	Thread_safe_server_info *server_info;

	boost::asio::streambuf *read_buffer;
	boost::asio::ip::tcp::socket *session_socket;

	int id=-1;
	int state = CLIENT_STATE_CONNECTED;
	bool state_work_in_progress = false;
	bool state_work_ready = false;
	boost::mutex mutex_point_state_work_ready;

	std::string login, password, expression;
	double calculation_result;
	bool can_be_deleted = false;
	boost::mutex mutex_point_can_be_deleted;

	void Can_Be_Deleted_Set();
	void State_Work_In_Progress_Set(bool state_new);
	bool State_Work_In_Progress_Get();
	void State_Work_Ready_Set(bool state_new);
	bool State_Work_Ready_Get();
public:
	int Command_Recognition(std::string message);
	void Calculate_Expression();
	void Initialization();
	void Received_Message_Handler(const boost::system::error_code & error, size_t byte_received);
	void Disconnect();
	bool Can_Be_Deleted_Get();
	int Id_Get();
	Client_Instance(boost::asio::io_service *io_service_calculations_new, Thread_safe_print_queue *print_queue_new, Thread_safe_server_info *server_info_new, boost::asio::ip::tcp::socket *sesion_socket_new, int ID_new);
	Client_Instance(const Client_Instance &obj);
	~Client_Instance();
};