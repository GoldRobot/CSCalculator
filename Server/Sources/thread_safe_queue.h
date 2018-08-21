#pragma once
#include <string>
#include <queue>

class Thread_safe_queue_output
{
private:
	std::queue <std::string> data;
	mutable boost::mutex mutex_point;
	boost::condition_variable mutex_point_condition_variable;
public:
	Thread_safe_queue_output();
	bool Is_Empty();
	void Push(std::string const data_to_push);
	void Push_Queue(std::queue <std::string> data_to_push);
	bool Pop_Try(std::string &data_pop_to);
	void Pop_Wait(std::string &data_pop_to);
	void Process_Output_Once();
	void Process_Output_All();
	void Process_Output_Wait_All();
	void Process_Output_Infinity_Loop();
};

void Thread_Safe_Queue_Output_Process(Thread_safe_queue_output &queue_data);
