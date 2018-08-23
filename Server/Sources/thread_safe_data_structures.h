#pragma once
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <string>
#include <queue>

class Thread_safe_print_queue
{
private:
	std::queue <std::string> data;
	boost::mutex mutex_point;
	boost::condition_variable mutex_point_condition_variable;
public:
	Thread_safe_print_queue();
	bool Is_Empty();
	void Push(std::string const data_to_push);
	void Push(std::queue <std::string> data_to_push);
	bool Pop_Try(std::string &data_pop_to);
	void Pop_Wait(std::string &data_pop_to);
	void Print_Once();
	void Print_All();
	void Print_Wait_All();
	void Print_Infinity_Loop();
};

void Thread_Safe_Print_Queue_Process(Thread_safe_print_queue &print_queue);

class Thread_safe_server_info
{
private:
	int connections_total_amount = 0;
	boost::mutex mutex_point_connections_total_amount;

	std::string thread_main_os_id = "unknown";
	boost::mutex mutex_point_thread_main_os_id;

public:
	void Connections_Total_Amount_Change(int new_number);
	void Connections_Total_Amount_Change(bool increase);
	int Connections_Total_Amount_Get();
	void Thread_Main_Os_ID_Set(std::string new_id);
	std::string Thread_Main_Os_ID_Get();
};
