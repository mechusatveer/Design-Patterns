#include <boost\asio.hpp>
#include <iostream>
#include <boost\thread\thread.hpp>
#include <boost\bind.hpp>
#include <boost\thread\mutex.hpp>
#include <stack>

using namespace std;
using namespace boost;

class process
{
public:
	virtual void run()=0;
	virtual ~process()=0
	{
	}
};

class processType1:virtual public process
{
	int id;
public:
	processType1(int i)
	{
		id = i;
	}
	virtual void run()
	{
		cout<<"long task task id "<<id<<"\n";
		Sleep(10000);
	}
	virtual ~processType1()
	{

	}
};

class ThreadPool
{
	static int Scount;
	int available;
	thread_group grp;
	mutex mutex_;
	asio::io_service io_service;
	asio::io_service::work work_;

public:
	ThreadPool(int num):work_(io_service)
	{
		mutex::scoped_lock lock(mutex_);
		
		available = num;
	
		if(Scount == 0)
			Scount++;
		else
			return;
		
		for(int i=0 ; i<num ; ++i)
		{
			grp.create_thread(boost::bind(&asio::io_service::run, &io_service));
		}
	}
	void runTask(std::shared_ptr<process>  obj)
	{
		if(available == 0)
		{
			cout<<"run out of thread wait \n";
			return;
		}
		--available;
		try
		{
			io_service.post(boost::bind(&ThreadPool::threadfun, this, obj));
		}
		catch(...)
		{
			cout<<"some error \n";
		}
	}
	
	~ThreadPool()
	{
		io_service.stop();
		grp.join_all();
	}

private:
	void threadfun(std::shared_ptr<process> obj)
	{
		obj->run();
		cout<<"task over \n";
		++available;
	}
};
int ThreadPool::Scount = 0;


int main( int argc, char * argv[] )
{
	ThreadPool pool(5);
	for(int i=0 ;  ; ++i)
	{
		std::shared_ptr<process> obj(new processType1(i));
		pool.runTask(obj);
		Sleep(500);
	}
	return 0;
}
	
