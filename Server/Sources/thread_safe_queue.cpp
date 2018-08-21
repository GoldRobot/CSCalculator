#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <iostream>

#include "thread_safe_queue.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::queue;
using std::to_string;

Thread_safe_queue_output::Thread_safe_queue_output()
{
}

bool Thread_safe_queue_output::Is_Empty()
{
	boost::mutex::scoped_lock lock(mutex_point);
	return data.empty();
}

void Thread_safe_queue_output::Push(string const data_to_push)
{
	boost::mutex::scoped_lock lock(mutex_point);
	data.push(data_to_push);
	lock.unlock();
	mutex_point_condition_variable.notify_one();
}

void Thread_safe_queue_output::Push_Queue(queue<string> data_to_push)
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

bool Thread_safe_queue_output::Pop_Try(string & data_pop_to)
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

void Thread_safe_queue_output::Pop_Wait(string & data_pop_to)
{
	boost::mutex::scoped_lock lock(mutex_point);
	while (data.empty())
	{
		mutex_point_condition_variable.wait(lock);
	}
	data_pop_to = data.front();
	data.pop();
}

void Thread_safe_queue_output::Process_Output_Once()
{
	boost::mutex::scoped_lock lock(mutex_point);
	if (data.empty())
	{
		return;
	}
	cout<< data.front();
	data.pop();
}

void Thread_safe_queue_output::Process_Output_All()
{
	boost::mutex::scoped_lock lock(mutex_point);
	while (!data.empty())
	{
		cout << data.front();
		data.pop();
	}
}

void Thread_safe_queue_output::Process_Output_Wait_All()
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

void Thread_safe_queue_output::Process_Output_Infinity_Loop()
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

void Thread_Safe_Queue_Output_Process(Thread_safe_queue_output &queue_data)
{
	string thread_current_id = boost::lexical_cast<std::string>(boost::this_thread::get_id());
	string thread_current_name = "TalkThread (ID: " + thread_current_id + ")";
	cout << thread_current_name + " activated.\n";
	queue_data.Process_Output_Infinity_Loop();
}
