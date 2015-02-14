

There are 3 threads each calling 

fun1
fun2
fun3

how we can make it so always they get called in seq


lock1.lock();
lock2.lock();


tid0 = pthread_t(&tid,NULL,&fun1,NULL);
tid2 = pthread_t(&tid,NULL,&fun2,NULL);
tid3 = pthread_t(&tid,NULL,&fun3,NULL);

fun1()
{
   lock3.lock():
   do your work first;
   lock1.unlock();
}

fun2()
{
    lock1.lock();
    do your job here
    lock2.unlokc();
}

fun3()
{
   lock2.lock();
   do your job here
   lock3.unlock();
}

only two mutex are sufficient here
