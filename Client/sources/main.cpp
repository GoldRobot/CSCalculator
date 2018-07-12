#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <iostream>
#include <fstream>
#include <string>

#define ADRESS_IP "127.0.0.1"

using std::cout;
using std::cin;
using std::endl;
using std::string;
using namespace boost::asio;

void client_process(ip::tcp::socket *target_socket, int session_id)
{
	string data;
	//data = "Session ID: " + std::to_string(session_id);
	data = std::to_string(session_id);
	while (true)
	{
		try
		{
			//target_socket->send(buffer(data));
		}
		catch (boost::system::system_error e)
		{
			return; //unnoknw error
		}
		boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
	}
}

int main()
{
	std::ofstream file;
	file.open("log.txt");
	file.clear();
	io_service service_io_stream; //Объект потока IO
	ip::tcp::endpoint target_point(ip::address::from_string(ADRESS_IP), 42042);
	ip::tcp::socket target_socket(service_io_stream);
	try
	{
		target_socket.connect(target_point);
	}
	catch (boost::system::system_error e)
	{
		cout << "Connection failed (" << e.code().value() << ")." << endl;
		file << "Connection failed (" << e.code().value() << " -- " << e.code().message() << ")." << endl;
		//system("pause");
		file.close();
		system("pause");
		return -1;
	}
	int session_id = 0;
	try
	{
		target_socket.read_some(buffer(&session_id, sizeof(session_id)));
	}
	catch (boost::system::system_error e)
	{
		if (e.code().value() == 2)
		{
			cout << "\nConnection aborted." << endl;
			e.~system_error();
			system("pause");
			return 0;
		}
		else
		{
			cout << "\nUnnoknw exeption." << endl;
			e.~system_error();
			system("pause");
			return -1;
		}
	}
	boost::thread(boost::bind(client_process, &target_socket, session_id));
	cout << "Session ID: " << session_id << endl;
	cout << "Connected." << endl;
	system("pause");
	target_socket.close();
	cout << "Disconnected." << endl;
	file.close();
	return 0;

}

