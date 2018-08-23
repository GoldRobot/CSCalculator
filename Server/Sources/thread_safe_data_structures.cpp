#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

#include "thread_safe_data_structures.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::queue;
using std::to_string;

Thread_safe_print_queue::Thread_safe_print_queue()
{
}

bool Thread_safe_print_queue::Is_Empty()
{
	boost::mutex::scoped_lock lock(mutex_point);
	return data.empty();
}

void Thread_safe_print_queue::Push(string const data_to_push)
{
	boost::mutex::scoped_lock lock(mutex_point);
	data.push(data_to_push);
	lock.unlock();
	mutex_point_condition_variable.notify_one();
}

void Thread_safe_print_queue::Push(queue<string> data_to_push)
{
	boost::mutex::scoped_lock lock(mutex_point);
	while (!data_to_push.empty())
	{
		data.push(data_to_push.front());
		data_to_push.pop();
	}
	lock.unlock();
	mutex_point_condition_variable.notify_one();
}

bool Thread_safe_print_queue::Pop_Try(string & data_pop_to)
{
	boost::mutex::scoped_lock lock(mutex_point);
	if (data.empty())
	{
		return false;
	}
	else
	{
		data_pop_to = data.front();
		data.pop();
		return true;
	}
	return false;
}

void Thread_safe_print_queue::Pop_Wait(string & data_pop_to)
{
	boost::mutex::scoped_lock lock(mutex_point);
	while (data.empty())
	{
		mutex_point_condition_variable.wait(lock);
	}
	data_pop_to = data.front();
	data.pop();
}

void Thread_safe_print_queue::Print_Once()
{
	boost::mutex::scoped_lock lock(mutex_point);
	if (data.empty())
	{
		return;
	}
	cout<< data.front();
	data.pop();
}

void Thread_safe_print_queue::Print_All()
{
	boost::mutex::scoped_lock lock(mutex_point);
	while (!data.empty())
	{
		cout << data.front();
		data.pop();
	}
}

void Thread_safe_print_queue::Print_Wait_All()
{
	boost::mutex::scoped_lock lock(mutex_point);
	while (data.empty())
	{
		mutex_point_condition_variable.wait(lock);
	}
	while (!data.empty())
	{
		cout << data.front();
		data.pop();
	}
}

void Thread_safe_print_queue::Print_Infinity_Loop()
{
	while (true)
	{
		boost::mutex::scoped_lock lock(mutex_point);
		while (data.empty())
		{
			mutex_point_condition_variable.wait(lock);
		}
		while (!data.empty())
		{
			cout << data.front();
			data.pop();
		}
		boost::this_thread::interruption_point();
	}
}

void Thread_Safe_Print_Queue_Process(Thread_safe_print_queue &print_queue)
{
	string thread_current_id = boost::lexical_cast<std::string>(boost::this_thread::get_id());
	string thread_current_name = "TalkThread (ID: " + thread_current_id + ")";
	cout << thread_current_name + " activated.\n";
	print_queue.Print_Infinity_Loop();
}

void Thread_safe_server_info::Connections_Total_Amount_Change(int new_number)
{
	boost::mutex::scoped_lock lock(mutex_point_connections_total_amount);
	connections_total_amount = new_number;
}

void Thread_safe_server_info::Connections_Total_Amount_Change(bool increase)
{
	boost::mutex::scoped_lock lock(mutex_point_connections_total_amount);
	if (increase)
	{
		connections_total_amount++;
	}
	else
	{
		connections_total_amount--;
	}
}

int Thread_safe_server_info::Connections_Total_Amount_Get()
{
	boost::mutex::scoped_lock lock(mutex_point_connections_total_amount);
	return connections_total_amount;
}

void Thread_safe_server_info::Thread_Main_Os_ID_Set(std::string new_id)
{
	boost::mutex::scoped_lock lock(mutex_point_thread_main_os_id);
	thread_main_os_id = new_id;
}

std::string Thread_safe_server_info::Thread_Main_Os_ID_Get()
{
	boost::mutex::scoped_lock lock(mutex_point_thread_main_os_id);
	return thread_main_os_id;
}
