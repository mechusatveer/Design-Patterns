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
	
void CalculateFib(std::size_t n);

int main()
{
  boost::asio::io_service io_service;
  boost::optional<boost::asio::io_service::work> work =       // '. 1
      boost::in_place(boost::ref(io_service));                // .'

  boost::thread_group worker_threads;                         // -.
  for(int x = 0; x < 2; ++x)                                  //   :
  {                                                           //   '.
    worker_threads.create_thread(                             //     :- 2
      boost::bind(&boost::asio::io_service::run, &io_service) //   .'
    );                                                        //   :
  }                                                           // -'

  io_service.post(boost::bind(CalculateFib, 3));              // '.
  io_service.post(boost::bind(CalculateFib, 4));              //   :- 3
  io_service.post(boost::bind(CalculateFib, 5));              // .'

  work = boost::none;                                         // 4
  worker_threads.join_all();                                  // 5
}


The io_service::run() will run operations as long as there are asynchronous operations
to perform. If, at any time, there are no asynchronous operations pending (or handlers being invoked),
the run() call will return.

However, there are some designs that would prefer that the run() call not exit until all work is done
AND the io_service has explicitly been instructed that it's okay to exit. That's what io_service::work 
is used for. By creating the work object (I usually do it on the heap and a shared_ptr), the io_service
considers itself to always have something pending, and therefore the run() method will not return. 
Once I want the service to be able to exit (usually during shutdown), I will destroy the work object.
