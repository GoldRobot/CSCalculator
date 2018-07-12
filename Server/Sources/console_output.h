#pragma once
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <iostream>
#include <string>
#include <queue>


using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::queue;
using std::to_string;

class Queue_thread_safe
{
private:
	queue <string> data;
	mutable boost::mutex lock_point_mutex;
	boost::condition_variable lock_point_condition_variable;
public:
	Queue_thread_safe();
	void Push(string const data_to_push);
	bool Try_Pop(string &data_to_pop);
	bool Is_Empty();
	void Wait_Pop(string &data_to_pop);
	void Push_Queue(queue <string> data_to_push);
};

void Queue_Process(Queue_thread_safe &queue_data);

void Add_Main_Info(Queue_thread_safe &queue_data, string thread_id_main, int number_of_threads, string *thread_id_acceptors);