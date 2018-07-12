#include "console_output.h"


Queue_thread_safe::Queue_thread_safe()
{
}

void Queue_thread_safe::Push(string const data_to_push)
{
	boost::mutex::scoped_lock lock(lock_point_mutex);
	data.push(data_to_push);
	lock.unlock();
	lock_point_condition_variable.notify_one();
}

bool Queue_thread_safe::Try_Pop(string & data_to_pop)
{
	boost::mutex::scoped_lock lock(lock_point_mutex);
	if (data.empty())
	{
		return false;
	}
	else
	{
		data_to_pop = data.front();
		data.pop();
		return true;
	}
	return true;
}

bool Queue_thread_safe::Is_Empty()
{
	boost::mutex::scoped_lock lock(lock_point_mutex);
	return data.empty();
}

void Queue_thread_safe::Wait_Pop(string & data_to_pop)
{
	boost::mutex::scoped_lock lock(lock_point_mutex);
	while (data.empty())
	{
		lock_point_condition_variable.wait(lock);
	}
	data_to_pop = data.front();
	data.pop();
}

void Queue_thread_safe::Push_Queue(queue<string> data_to_push)
{
	boost::mutex::scoped_lock lock(lock_point_mutex);
	while (!data_to_push.empty())
	{
		data.push(data_to_push.front());
		data_to_push.pop();
	}
	lock.unlock();
	lock_point_condition_variable.notify_one();
}

void Queue_Process(Queue_thread_safe &queue_data)
{
	string data;
	data = "TalkThread activated.\n";
	cout << data;
	while (true)
	{
		queue_data.Wait_Pop(data);
		cout << data;
	}
}

void Add_Main_Info(Queue_thread_safe & queue_data, string thread_id_main, int number_of_threads, string *thread_id_acceptors)
{
	string data_line;
	queue <string> data_full;
	data_line = "-----------------\n";
	data_full.push(data_line);
	data_line = "ID of threads:\n";
	data_full.push(data_line);
	data_line = "Main thread: " + thread_id_main + "\n";
	data_full.push(data_line);
	for (int i = 0; i < number_of_threads; i++)
	{
		data_line = to_string(i + 1) + " thread: " + thread_id_acceptors[i] + "\n";
		data_full.push(data_line);
	}
	data_line = "-----------------\n";
	data_full.push(data_line);
	queue_data.Push_Queue(data_full);
}
